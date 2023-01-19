#include "kfmtfile.h"
#include "kfmterror.h"
#include "utilities.h"

KFMTFile::KFMTFile(const QString& path, const KFMTFileListEntry* fileListEntry)
    : m_path(path), m_fileListEntry(fileListEntry)
{
    QFile fileHandle(path);
    switch (fileListEntry->m_fileType)
    {
        case FileType::Folder:
            break;
        case FileType::MIX:
            loadMIX(fileHandle);
            break;
        case FileType::Raw: {
            if (!fileHandle.open(QIODevice::ReadOnly)) return;
            m_rawData = fileHandle.readAll();
            break;
        }
        case FileType::T:
            loadT(fileHandle);
            break;
    }
}

void KFMTFile::extractTo(const QDir& outDir)
{
    if (m_fileListEntry->m_dataType != DataType::Container)
    {
        KFMTError::error("KFMTFile::extractTo: Called on non-container file!");
        return;
    }

    size_t fileIndex = 0;
    for (const auto& subFile : m_subFiles) {
        auto fn = m_path.mid(m_path.lastIndexOf(QRegularExpression(QStringLiteral("[\\/]"))) + 1)
                  + QString::number(fileIndex) + '.' + subFile.fileExtension();
        QFile output(outDir.filePath(fn));
        if (!output.open(QIODevice::WriteOnly))
            KFMTError::fatalError("Unable to open output file for " + output.fileName());
        output.write(subFile.m_rawData);
        output.close();
        fileIndex++;
    }
}

const QString& KFMTFile::fileExtension() const
{
    static const auto data = QStringLiteral("data");
    static const auto exe = QStringLiteral("exe");
    static const auto gamedb = QStringLiteral("gamedb");
    static const auto mapdb = QStringLiteral("mapdb");
    static const auto mapscript = QStringLiteral("mapscript");
    static const auto maptile = QStringLiteral("maptile");
    static const auto mim = QStringLiteral("mim");
    static const auto mo = QStringLiteral("mo");
    static const auto rtmd = QStringLiteral("rtmd");
    static const auto rtim = QStringLiteral("rtim");
    static const auto tim = QStringLiteral("tim");
    static const auto tmd = QStringLiteral("tmd");

    if (Utilities::fileIsPSXEXE(m_rawData))
        return exe;
    if (Utilities::fileIsTMD(m_rawData))
        return tmd;
    if (Utilities::fileIsTIM(m_rawData))
        return tim;
    if (Utilities::fileIsRTIM(m_rawData))
        return rtim;
    if (Utilities::fileIsRTMD(m_rawData))
        return rtmd;
    if (Utilities::fileIsMIM(m_rawData))
        return mim;
    if (Utilities::fileIsMO(m_rawData))
        return mo;
    if (Utilities::fileIsMAP1(m_rawData))
        return maptile;
    if (Utilities::fileIsMAP2(m_rawData))
        return mapdb;
    if (Utilities::fileIsMAP3(m_rawData))
        return mapscript;
    if (Utilities::fileIsKF2GameDB(m_rawData))
        return gamedb;

    return data;
}

uint32_t KFMTFile::indexInContainer()
{
    if (m_fileListEntry->m_dataType == DataType::Container)
        KFMTError::fatalError("KFMTFile::indexInContainer(): called for container!");

    return m_path.mid(m_path.lastIndexOf(':') + 1).toUInt();
}

void KFMTFile::recalculateChecksum()
{
    // FIXME: Use direct pointer stuff or std::accumulate to take advantage of vectorization.
    // Should also help avoid QDataStream overhead.
    auto ints = m_rawData.size() / 4 - 1;
    QDataStream fileStream(&m_rawData, QIODevice::ReadWrite);
    fileStream.setByteOrder(QDataStream::LittleEndian);
    quint32 checksum = 0x12345678;
    quint32 curInt = 0;
    for (auto curIntIndex = 0; curIntIndex < ints; curIntIndex++) {
        fileStream >> curInt;
        checksum += curInt;
    }
    fileStream << checksum;
}

void KFMTFile::loadMIX(QFile& fileHandle)
{
    if (!fileHandle.open(QIODevice::ReadOnly))
        return;
    const auto file = fileHandle.readAll();

    // MIX files that contain MIMs, TIMs or TMDs don't have a size value before the files
    // So if a MIX file is not recognized as a MIM, TIM or TMD, it does not store the
    // size for each file.

    if (!Utilities::fileIsMIM(file.mid(4)) && !Utilities::fileIsTIM(file)
        && !Utilities::fileIsTMD(file))
        containerType = ContainerType::MIX_HasSizes;
    else if (Utilities::as<uint32_t>(file) == 0x06 || Utilities::as<uint32_t>(file) == 0x07) {
        containerType = ContainerType::MIX_MIMCollection;
        m_mixMimSignature = Utilities::as<uint32_t>(file);
    } else
        containerType = ContainerType::MIX_NoSizes;

    QDataStream stream(file);
    stream.setByteOrder(QDataStream::LittleEndian);

    if (containerType == ContainerType::MIX_HasSizes) {
        size_t fileNo = 0;
        const KFMTFileListEntry* subFileEntry = m_fileListEntry + 1;
        while (!stream.atEnd())
        {
            // FIXME: Can we use QDataStream &operator>>(QDataStream &in, QBitArray &ba) here instead?
            uint32_t size;
            stream >> size;
            QByteArray subFile;
            subFile.resize(size);
            stream.readRawData(subFile.data(), size);
            m_subFiles.emplace_back(m_path + ':' + QString::number(fileNo), subFile, subFileEntry);
            fileNo++;
            subFileEntry++;
        }
    } else {
        // Create a vector of offsets to the files and a variable for finding offsets
        std::vector<size_t> offsets;
        int curOffset = 0;

        // Get all offsets that match the signature
        while (curOffset + 8 < file.size()) {
            auto curOffsetSignature = file.mid(curOffset, 8);
            if (Utilities::fileIsTIM(curOffsetSignature)) {
                // FIXME: I hate this block of code so much
                // Skip the TIM's contents
                uint32_t clutSize = 0;
                uint32_t pDataSize = 0;
                // If the pixel mode is Direct 15 or 24 bit, we have no CLUT.
                uint8_t timFlag = Utilities::as<uint8_t>(file, curOffset + 4u);
                if (timFlag == 0x02 || timFlag == 0x03)
                    pDataSize = Utilities::as<uint32_t>(file, curOffset + 8u);
                else // Otherwise, we do have a CLUT
                {
                    clutSize = Utilities::as<uint32_t>(file, curOffset + 8u);
                    pDataSize = Utilities::as<uint32_t>(file, curOffset + 8u + clutSize);
                }

                if (clutSize + pDataSize > 0)
                    offsets.push_back(curOffset);
                curOffset += clutSize + pDataSize;
            } else if (Utilities::fileIsTMD(curOffsetSignature))
                offsets.push_back(curOffset);
            else if (Utilities::fileIsMIM(
                         file.mid(curOffset, Utilities::as<uint32_t>(curOffsetSignature)))) {
                offsets.push_back(curOffset);
                curOffset += Utilities::as<uint32_t>(curOffsetSignature) - 8;
            }
            curOffset += 4u;
        }

        // Add EOF to list of offsets
        offsets.push_back(file.size());

        // Read the individual files from the MIX file based on the offsets.
        size_t fileNo = 0;
        const KFMTFileListEntry* subFileEntry = m_fileListEntry + 1;
        for (auto offset = offsets.cbegin(), nextOffset = offsets.cbegin() + 1;
             nextOffset != offsets.cend();
             offset++, nextOffset++)
        {
            m_subFiles.emplace_back(m_path + '_' + QString::number(fileNo),
                                    file.mid(*offset, *nextOffset - *offset),
                                    subFileEntry);
            fileNo++;
            subFileEntry++;
        }
    }
}

void KFMTFile::loadT(QFile& fileHandle)
{
    // BUG: KF2 RTMD.T does not load RTMD.T8 for some reason!!!

    containerType = ContainerType::T;

    if (!fileHandle.open(QIODevice::ReadOnly))
        return;

    QDataStream stream(&fileHandle);
    stream.setByteOrder(QDataStream::LittleEndian);

    uint16_t nFiles;
    stream >> nFiles;
    std::vector<uint32_t> offsets;
    offsets.reserve(nFiles);

    uint16_t offset = 0;
    size_t fileNum = 0;
    size_t trueFileNum = 0;
    for (uint16_t i = 0; i <= nFiles; i++) {
        stream >> offset;
        if (offset == 0)
            break;

        // If the offset vector is empty or if this offset isn't a duplicate of the last one,
        // we add it to the vector
        if (offsets.empty() || offsets.back() != static_cast<uint32_t>(offset)) {
            // Offsets must be multiplied by 2048.
            offsets.push_back(static_cast<uint32_t>(offset));

            // If we inserted this offset, we increase the amount of true files, which here
            // also serves as an index to which true file this is
            trueFileNum++;
        }

        // We register what true file this index points to
        (*m_tFileMap)[fileNum] = trueFileNum - 1;

        fileNum++;
    }

    // Multiply all offsets by 2048
    for (auto& o : offsets)
        o *= 2048u;

    // Preallocate the subfile vector to avoid overhead
    m_subFiles.reserve(trueFileNum);

    // Read the files in the T file
    fileHandle.seek(0u);                    // Reset file position...
    const auto blob = fileHandle.readAll(); // and read the whole file into a temporary blob
    size_t fileNo = 0;
    const KFMTFileListEntry* subFileEntry = m_fileListEntry + 1;
    for (auto fileOffset = offsets.cbegin(); fileOffset != offsets.end() - 1; fileOffset++)
    {
        m_subFiles.emplace_back(m_path + ':' + QString::number(fileNo),
                                blob.mid(*(fileOffset), *(fileOffset + 1) - *fileOffset),
                                subFileEntry);
        fileNo++;
        subFileEntry++;
    }
}

void KFMTFile::writeFile(const QDir& outDir)
{
    QString dirToCreate = m_path.left(
        m_path.lastIndexOf(QRegularExpression(QStringLiteral("[\\/]"))) + 1);
    outDir.mkpath(dirToCreate);

    QFile output(outDir.filePath(m_path));
    if (!output.open(QIODevice::WriteOnly))
        KFMTError::fatalError("Unable to open output file for " + m_path);

    switch (m_fileListEntry->m_fileType)
    {
        case FileType::Folder: {
            QDir dir(m_path);
            if (!dir.exists())
                dir.mkdir(".");
            for (auto& file : m_subFiles)
                file.writeFile(dir);
            break;
        }
        case FileType::MIX:
            writeMIX(output);
            break;
        case FileType::Raw:
            output.write(m_rawData);
            break;
        case FileType::T:
            writeT(output);
            break;
    }

    output.close();
}

void KFMTFile::writeMIX(QFile& fileHandle)
{
    QDataStream outStream(&fileHandle);
    outStream.setByteOrder(QDataStream::LittleEndian);

    if (containerType == ContainerType::MIX_MIMCollection)
        outStream << m_mixMimSignature;

    for (const auto& file : m_subFiles) {
        if (containerType == ContainerType::MIX_HasSizes)
            outStream << static_cast<uint32_t>(file.m_rawData.size());
        outStream.writeRawData(file.m_rawData.data(), file.m_rawData.size());
    }
}

void KFMTFile::writeT(QFile& fileHandle)
{
    for (auto& file : m_subFiles)
        file.recalculateChecksum();

    // Create data blob
    QByteArray dataBlob;

    std::vector<uint16_t> newTrueOffsets;

    for (const auto& file : m_subFiles) {
        QByteArray alignedFile;
        auto alignedSize = static_cast<uint16_t>(file.m_rawData.size() / 2048u);
        if (file.m_rawData.size() % 2048u != 0)
            alignedSize += 1;

        alignedFile.resize(alignedSize * 2048u);
        alignedFile.fill(0);
        std::memcpy(alignedFile.data(), file.m_rawData.data(), file.m_rawData.size());

        newTrueOffsets.push_back((dataBlob.size() + 2048u) / 2048u);
        dataBlob.append(file.m_rawData);
    }

    // Add EOF to the offset vector
    newTrueOffsets.push_back((dataBlob.size() + 2048u) / 2048u);

    // Create stream for writing to the file
    QDataStream outStream(&fileHandle);
    outStream.setByteOrder(QDataStream::LittleEndian);
    // Write number of (fake) files
    outStream << static_cast<uint16_t>((*m_tFileMap).size() - 1);

    // Write pointers
    for (size_t fileNum = 0; fileNum < (*m_tFileMap).size(); fileNum++)
        outStream << newTrueOffsets.at((*m_tFileMap).find(fileNum)->second);

    // Pad out pointer table to 2048 bytes.
    // FIXME: I think this is pretty slow, but I can't think of a way around it right now.
    while (outStream.device()->pos() < 2048)
        outStream << static_cast<uint8_t>(0);

    outStream.writeRawData(dataBlob.data(), dataBlob.size());
}
