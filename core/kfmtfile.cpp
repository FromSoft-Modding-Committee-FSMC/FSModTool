#include "kfmtfile.h"
#include "kfmterror.h"
#include "formats/ps1/seq.h"
#include "formats/ps1/tim.h"
#include "formats/ps1/tmd.h"
#include "formats/ps1/vab.h"
#include "utilities.h"

KFMTFile::KFMTFile(const QString& name, const QByteArray& data, KFMTFile* const parent,
                   const FileFormat fileType, const DataType dataType, const QString& prettyName)
    : m_name(name), m_data(data), m_parent(parent), m_prettyName(prettyName), m_fileType(fileType),
      m_dataType(dataType)
{
    switch (fileType)
    {
        case FileFormat::MIMList: loadMIMList(); break;
        case FileFormat::MIX: loadMIX(); break;
        case FileFormat::T: loadT(); break;
        default: break;
    }
}

void KFMTFile::extractTo(const QDir& outDir)
{
    if (m_dataType != DataType::Container)
    {
        KFMTError::error("KFMTFile::extractTo: Called on non-container file!");
        return;
    }

    size_t fileIndex = 0;
    for (const auto& subFile : m_subFiles) {
        auto fn = m_name.mid(m_name.lastIndexOf(QRegularExpression(QStringLiteral("[\\/]"))) + 1)
                  + QString::number(fileIndex) + '.' + subFile.extension();
        QFile output(outDir.filePath(fn));
        if (!output.open(QIODevice::WriteOnly))
            KFMTError::fatalError("Unable to open output file for " + output.fileName());
        output.write(subFile.m_data);
        output.close();
        fileIndex++;
    }
}

const QString& KFMTFile::extension() const
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

    if (Utilities::fileIsPSXEXE(m_data)) return exe;
    if (Utilities::fileIsTMD(m_data)) return tmd;
    if (Utilities::fileIsTIM(m_data)) return tim;
    if (Utilities::fileIsRTIM(m_data)) return rtim;
    if (Utilities::fileIsRTMD(m_data)) return rtmd;
    if (Utilities::fileIsMIM(m_data)) return mim;
    if (Utilities::fileIsMO(m_data)) return mo;
    if (Utilities::fileIsMAP1(m_data)) return maptile;
    if (Utilities::fileIsMAP2(m_data)) return mapdb;
    if (Utilities::fileIsMAP3(m_data)) return mapscript;
    if (Utilities::fileIsKF2GameDB(m_data)) return gamedb;

    return data;
}

void KFMTFile::recalculateChecksum()
{
    // FIXME: Use direct pointer stuff or std::accumulate to take advantage of vectorization.
    // Should also help avoid QDataStream overhead.

    auto ints = m_data.size() / 4 - 1;
    QDataStream fileStream(&m_data, QIODevice::ReadWrite);
    fileStream.setByteOrder(QDataStream::LittleEndian);
    quint32 checksum = 0x12345678;
    quint32 curInt = 0;
    for (auto curIntIndex = 0; curIntIndex < ints; curIntIndex++) {
        fileStream >> curInt;
        checksum += curInt;
    }
    fileStream << checksum;
}

void KFMTFile::loadMIMList()
{
    m_containerType = ContainerType::MIMList;

    auto mimCount = Utilities::as<uint32_t>(m_data);
    //m_subFiles.reserve(mimCount);

    // We start at 4 to skip the MIM count
    uint32_t curOffset = 4;

    for (uint32_t i = 0; i < mimCount; i++)
    {
        auto mimSize = Utilities::as<uint32_t>(m_data, curOffset);

        createChild(QString::number(i),
                    m_data.mid(curOffset, mimSize),
                    FileFormat::Raw,
                    DataType::Model);

        curOffset += mimSize;
    }

    m_data.clear();
}

void KFMTFile::loadMIX()
{
    // FIXME: The commented text below is inaccurate. Don't take it as law please.
    // MIX files that contain MIMs, TIMs or TMDs don't have a size value before the files
    // So if a MIX file is not recognized as a MIM, TIM or TMD, it does not store the
    // size for each file.

    if (Utilities::fileIsTIM(m_data) || Utilities::fileIsTMD(m_data))
        m_containerType = ContainerType::MIX_NoSizes;
    else m_containerType = ContainerType::MIX_HasSizes;

    uint32_t curOffset = 0;
    uint32_t fileNo = 0;

    if (m_containerType == ContainerType::MIX_HasSizes)
    {
        while (curOffset < m_data.size())
        {
            auto size = Utilities::as<uint32_t>(m_data, curOffset);
            if (size == 0) // This happens in the KF2 Game DB due to T file padding.
                break;
            createChild(QString::number(fileNo), m_data.mid(curOffset + 4, size));

            curOffset += 4 + size;
            fileNo++;
        }
    }
    else if (m_containerType == ContainerType::MIX_NoSizes)
    {
        while (curOffset + 8 < m_data.size())
        {
            // Get the signature for the file at the current offset.
            auto curOffsetSignature = m_data.mid(static_cast<int>(curOffset), 8);
            uint32_t size = 0;
            if (Utilities::fileIsTIM(curOffsetSignature))
                size = PS1::TIM(m_data, curOffset).fileSize();
            else if (Utilities::fileIsTMD(curOffsetSignature))
                size = PS1::TMD(m_data, curOffset).fileSize();
            else if (Utilities::fileIsMIM(
                         m_data.mid(curOffset, Utilities::as<uint32_t>(curOffsetSignature))))
                size = Utilities::as<uint32_t>(m_data, curOffset);
            else if (Utilities::fileIsVH(curOffsetSignature))
            {
                // VABs are actually two files in a trenchcoat which are one file split into two trenchcoats.
                // So here we manually create the subfile for the header and then leave the appropriate stuff
                // for the code below to create a subfile for the waveform data.
                auto vh = PS1::VABHeader(m_data, curOffset);
                auto headerSize = vh.headerFileSize();
                auto waveformSize = vh.waveformFileSize();
                fsmt_assert(headerSize != 0, "KFMTFile::loadMIX: VAB has empty header!");
                fsmt_assert(waveformSize != 0, "KFMTFile::loadMIX: VAB has empty waveform!");
                createChild(QString::number(fileNo), m_data.mid(curOffset, headerSize));
                curOffset += headerSize;
                fileNo++;
                size = waveformSize;
            }
            else if (Utilities::fileIsSEQ(curOffsetSignature))
            {
                KFMTError::error(QStringLiteral(u"FSMTFile::loadMIX has found a SEQ file in a "
                                                "sizeless MIX called %1. The SEQ format is a bit"
                                                " hard to implement, so for now we will just bail. "
                                                "\nNO MORE SUBFILES OF %1 WILL BE LOADED!!!")
                                     .arg(m_name));
                break;
            }
            
            fsmt_assert(size != 0, "KFMTFile::loadMIX: Calculated size for file in MIX_NoSizes is 0!");

            createChild(QString::number(fileNo), m_data.mid(curOffset, size));
            curOffset += size;
            fileNo++;
        }
    }

    m_data.clear();
}

void KFMTFile::loadT()
{
    // BUG: KF2 RTMD.T does not load RTMD.T8 for some reason!!!

    m_containerType = ContainerType::T;
    m_tFileMap = std::make_unique<std::map<uint16_t, uint16_t>>();

    QDataStream stream(m_data);
    stream.setByteOrder(QDataStream::LittleEndian);

    uint16_t nFiles;
    stream >> nFiles;
    std::vector<uint32_t> offsets;
    offsets.reserve(nFiles);

    uint16_t offset = 0;
    uint16_t fileNum = 0;
    uint16_t trueFileNum = 0;
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
    //m_subFiles.reserve(trueFileNum);

    // Read the files in the T file
    size_t fileNo = 0;
    for (auto fileOffset = offsets.cbegin(); fileOffset != offsets.end() - 1; fileOffset++)
    {
        createChild(QString::number(fileNo),
                    m_data.mid(*(fileOffset), *(fileOffset + 1) - *fileOffset));
        fileNo++;
    }
}

void KFMTFile::writeFile(const QDir& outDir)
{
    QString dirToCreate = m_name.left(
        m_name.lastIndexOf(QRegularExpression(QStringLiteral("[\\/]"))) + 1);
    outDir.mkpath(dirToCreate);

    QFile output(outDir.filePath(m_name));
    if (!output.open(QIODevice::WriteOnly))
        KFMTError::fatalError("Unable to open output file for " + m_name);

    switch (m_fileType)
    {
        case FileFormat::Folder:
        {
            QDir dir(m_name);
            if (!dir.exists())
                dir.mkdir(".");
            for (auto& file : m_subFiles)
                file.writeFile(dir);
            break;
        }
        case FileFormat::MIMList: writeMIMList(output); break;
        case FileFormat::MIX: writeMIX(output); break;
        case FileFormat::Raw: output.write(m_data); break;
        case FileFormat::Root: break;
        case FileFormat::T: writeT(output); break;
    }

    output.close();
}

void KFMTFile::writeMIMList(QFile& fileHandle)
{
    QDataStream outStream(&fileHandle);
    outStream.setByteOrder(QDataStream::LittleEndian);

    outStream << static_cast<uint32_t>(m_subFiles.size());
    for (const auto& file : m_subFiles)
        outStream.writeRawData(file.m_data.data(), file.m_data.size());
}

void KFMTFile::writeMIX(QFile& fileHandle)
{
    QDataStream outStream(&fileHandle);
    outStream.setByteOrder(QDataStream::LittleEndian);

    for (const auto& file : m_subFiles) {
        if (m_containerType == ContainerType::MIX_HasSizes)
            outStream << static_cast<uint32_t>(file.m_data.size());
        outStream.writeRawData(file.m_data.data(), file.m_data.size());
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
        auto alignedSize = static_cast<uint16_t>(file.m_data.size() / 2048u);
        if (file.m_data.size() % 2048u != 0) alignedSize += 1;

        alignedFile.resize(alignedSize * 2048u);
        alignedFile.fill(0);
        std::memcpy(alignedFile.data(), file.m_data.data(), file.m_data.size());

        newTrueOffsets.push_back((dataBlob.size() + 2048u) / 2048u);
        dataBlob.append(file.m_data);
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
