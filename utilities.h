#ifndef UTILITIES_H
#define UTILITIES_H

#include <cstring>
#include <iostream>
#include <numeric>
#include <QByteArray>
#include <QRegularExpression>
#include <QtGlobal>

namespace Utilities
{
template<class T>
T& as(QByteArray& array, uint32_t offset = 0)
{
    auto* d = array.data();
    d += offset;
    return *reinterpret_cast<T*>(d);
}

template<class T>
const T& as(const QByteArray& array, uint32_t offset = 0)
{
    return *reinterpret_cast<const T*>(array.data() + offset);
}

template<class T>
const T& as(const uint8_t* ptr, uint32_t offset = 0)
{
    return *reinterpret_cast<T*>(ptr + offset);
}

template<class T>
uint8_t clampToByte(T value)
{
    return static_cast<quint8>(qMin(static_cast<T>(0xFF), value));
}

template<class T>
int16_t clampToShort(T value)
{
    return static_cast<int16_t>(qMax(static_cast<T>(-32767), qMin(static_cast<T>(32767), value)));
}

template<class T>
uint16_t clampToUShort(T value)
{
    return static_cast<quint16>(qMin(static_cast<T>(0xFFFF), value));
}

inline bool fileIsKF1GameDB(const QByteArray& file)
{
    return file.left(8).compare(QByteArray::fromHex("A4010000A4010000")) == 0;
}

inline bool fileIsKF2GameDB(const QByteArray& file)
{
    if (file.mid(4, 6).compare(QByteArray::fromHex("4010FF000000")) != 0)
        return false;

    if (file.mid(20, 14).compare(QByteArray::fromHex("00000000000000004010FF000000")) != 0)
        return false;

    return true;
}

/*!
 * \brief Checks whether a file is the first file of a map.
 * This is done in a very naive manner that checks if the first 4 bytes of the file are equal to
 * {0x00, 0xFA, 0x00, 0x00}. FIXME: We can probably do a better heuristic for this.
 * \param file File to check.
 * \return Whether the file is the first file of a map.
 */
inline bool fileIsMAP1(const QByteArray& file)
{
    return file.left(4).compare(QByteArray::fromHex("00FA0000")) == 0;
}

/*!
 * \brief Checks whether a file is the second file of a map.
 * This is done in a very naive manner that checks if the first 4 bytes of the file are equal to
 * {0xC0 0x32 0x00 0x00}. FIXME: We can probably do a better heuristic for this.
 * \param file File to check.
 * \return Whether the file is the second file of a map.
 */
inline bool fileIsMAP2(const QByteArray& file)
{
    return file.left(4).compare(QByteArray::fromHex("C0320000")) == 0;
}

/*!
 * \brief Checks whether a file is the third file of a map.
 * This is done by assuming that we have a table of 32 pointers at the start of the file,
 * as a third map file would. We then check if the most significant byte in all of them is 0x80.
 * If so, this is a third map file.
 * \param file File to check
 * \return Whether the file is the third file of a map.
 */
inline bool fileIsMAP3(const QByteArray& file)
{
    return file.at(0x03) == '\x80' && file.at(0x07) == file.at(0x0b)
           && file.at(0x0b) == file.at(0x0f) && file.at(0x13) == file.at(0x17)
           && file.at(0x17) == file.at(0x1b) && file.at(0x1b) == file.at(0x1f)
        /*&& file.at(0x23) == file.at(0x27) && file.at(0x27) == file.at(0x2b)
          && file.at(0x2b) == file.at(0x2f) && file.at(0x33) == file.at(0x37)
          && file.at(0x37) == file.at(0x3b) && file.at(0x3b) == file.at(0x3f)
          && file.at(0x43) == file.at(0x47) && file.at(0x47) == file.at(0x4b)
          && file.at(0x4b) == file.at(0x4f) && file.at(0x53) == file.at(0x57)
          && file.at(0x57) == file.at(0x5b) && file.at(0x5b) == file.at(0x5f)
          && file.at(0x63) == file.at(0x67) && file.at(0x67) == file.at(0x6b)
          && file.at(0x6b) == file.at(0x6f) && file.at(0x73) == file.at(0x77)
          && file.at(0x77) == file.at(0x7b) && file.at(0x7b) == file.at(0x7f)*/
        ;
}

/*!
 * \brief Checks whether a file is a MO file.
 * This is done by checking if the unsigned int at offset 0x08 in the file points to a section 
 * in the file with the TMD ID (0x41).
 * \param file File to check.
 * \return Whether the file is a MO file.
 */
inline bool fileIsMO(const QByteArray& file)
{
    quint32 tmdOff = *reinterpret_cast<quint32*>(file.mid(8, 4).data());
    quint8 tmdSig = 0;
    if (tmdOff < static_cast<quint32>(file.size())) tmdSig = file.at(tmdOff);

    return tmdSig == 0x41;
}
/*!
 * \brief Checks whether a file is an MIM file.
 * \param file File to check.
 * \return Whether the file is an MIM file
 */
inline bool fileIsMIM(const QByteArray& file)
{
    // If the TMD offset isn't right, we won't even bother.
    // We're calling fileIsMO because that's all it does.
    if (!fileIsMO(file)) return false;

    const auto animCount = *reinterpret_cast<uint32_t*>(file.mid(4, 4).data());
    if (animCount == 0) // I don't think this happens, but I may be wrong
    {
        // I mean, I guess this can be called a MIM file?
        return true;
    }

    // Get the start of the animation data for the first animation
    // This is needed by both branches
    const auto firstAnimOffset = Utilities::as<uint32_t>(file, 0x14);

    if (animCount == 1) // If this file has only one animation
    {
        // For this check we check if the first morph target has a number of vertices equal to the
        // TMD's vertex count.
        const auto morphTargetTableOffset = Utilities::as<uint32_t>(file, 0x0C);
        const auto morphTarget1Offset = Utilities::as<uint32_t>(file, morphTargetTableOffset);
        const auto morphTarget1VertexCount = Utilities::as<uint32_t>(file, morphTarget1Offset + 8);
        const auto tmdOffset = Utilities::as<uint32_t>(file, 8u);
        // 16 = 12 (TMD header size) + 4 (size of vertex list pointer in first object)
        const auto tmdVertexCount = Utilities::as<uint32_t>(file, tmdOffset + 16);

        return morphTarget1VertexCount == tmdVertexCount;

        //        const auto morphTarget2Offset = Utilities::as<uint32_t>(file, morphTargetTableOffset + 4);
        //        // 2. Divide the distance between the morph target offsets by the size of a VDF, obtaining
        //        // the amount of vertices that that morph target alters (should be all of them)
        //        const auto vertexCount = (morphTarget2Offset - morphTarget1Offset) / 20u;
        //        // 3. Check if the vertex count from the TMD is the same as the number obtained in step 2
        //        // If it's the same, we have a MIM. Otherwise, there is a size mismatch and this must have
        //        // vertex compression, so it's not MIM.
        //        const auto tmdOffset = Utilities::as<uint32_t>(file, 8u);
        //        // 16 = 12 (TMD header size) + 4 (size of vertex list pointer in first object)
        //        const auto tmdVertexCount = Utilities::as<uint32_t>(file, tmdOffset + 16);

        //        return vertexCount == tmdVertexCount;
    }
    else if (animCount > 1) // If it has multiple animations
    {
        const auto firstAnimFrameCount = Utilities::as<uint32_t>(file, firstAnimOffset);
        // MIM has a 16-bit frame count and some other 16-bit integer after that.
        // MO has a 32-bit frame count.
        // Therefore, we interpret that 32-bit block as a 32-bit integer. If this is a MO file,
        // there's no way this number will be greater than 0xFFFF. If this is an MIM file,
        // the number will be greater than 0xFFFF because of the unknown 16-bit integer.
        return firstAnimFrameCount > 0xFFFF;
    }
}

/*!
 * \brief Checks whether a file is a PS-X EXE.
 * This is done by checking if the first 9 bytes are the null-terminated string "PS-X EXE".
 * \param file File to check.
 * \return Whether the file is a PS-X EXE.
 */
inline bool fileIsPSXEXE(const QByteArray& file)
{
    return file.left(8) == "PS-X EXE";
}

/*!
 * \brief Checks whether a file is an RTMD file.
 * This is done by checking if the file starts with 4 zero bytes and is the followed by either 
 * 0x12 (4 byte long) or 0x10 (4 byte long).
 * \param file File to check.
 * \return Whether the file is an RTMD file.
 */
inline bool fileIsRTMD(const QByteArray& file)
{
    return file.left(4).compare(QByteArray::fromHex("00000000")) == 0
           && (file.mid(4, 4).compare(QByteArray::fromHex("12000000")) == 0
               || file.mid(4, 4).compare(QByteArray::fromHex("10000000")) == 0);
}

/*!
 * \brief Checks whether a file is an RTIM file (should only be run after checking for TIM).
 * This is done by checking if the file's first 8 bytes are equal to the next 8 bytes.
 * We also make sure the first 4 bytes aren't equal to the next 4 bytes. This is necessary to
 * avoid false positives in the AC proto.
 * \param file File to check.
 * \return Whether the file is an RTIM file.
 */
inline bool fileIsRTIM(const QByteArray& file)
{
    return file.mid(8).left(8) == file.left(8) && file.mid(4).left(4) != file.left(4);
}

/*!
 * \brief Checks whether a file is a SEQ file.
 * This is done by checking if the file's first 4 bytes are equal to the 
 * little-endian string "SEQp".
 * \param file File to check.
 * \return Whether the file is a SEQ file.
 */
inline bool fileIsSEQ(const QByteArray& file)
{
    return file.left(4).compare(QByteArray::fromHex("70514553")) == 0;
}

/*!
 * \brief Checks whether a file is an TIM file.
 * This is done by checking if the file's first 4 bytes as an unsigned int is 
 * equal to the TMD ID (0x10).
 * \param file File to check.
 * \return Whether the file is an TIM file.
 */
inline bool fileIsTIM(const QByteArray& file)
{
    return file.left(4).compare(QByteArray::fromHex("10000000")) == 0 && file.at(4) > 0
           && file.at(4) <= 15 && file.mid(5, 3).compare(QByteArray::fromHex("000000")) == 0;
}

/*!
 * \brief Checks whether a file is a TMD file.
 * This is done by checking if the file's bytes 0 - 3 as an unsigned int are 
 * equal to the TMD ID (0x41) and if the file's bytes 4-7 as an unsigned int
 * are equal to 0 or 1, which are valid values for the FIXP field.
 * \param file File to check.
 * \return Whether the file is a TMD file.
 */
inline bool fileIsTMD(const QByteArray& file)
{
    // FIXME: This is janky. I'm only doing this because I'm really tired.
    return file.left(4).compare(QByteArray::fromHex("41000000")) == 0;
}

/*!
 * \brief Checks whether a file is a Shadow Tower TMD file.
 * This is done by first running the standard TMD check, and if that passes,
 * we check if there's the weird primitive stuff.
 * \param file File to check.
 * \return Whether the file is a Shadow Tower TMD file.
 */
inline bool fileIsSTTMD(const QByteArray& file)
{
    if (!fileIsTMD(file)) return false;

    // I know this looks terrible, but for some reason QByteArray::toUShort isn't working right
    // here. I don't know why and I'm too angry at it to try and figure out.
    const auto tmdPrimCount = *(reinterpret_cast<uint16_t*>(file.mid(0x20, 2).data()));
    uint16_t stPrimCounts[8];

    std::memcpy(&stPrimCounts, file.mid(0x24, 16).data(), 16);

    return tmdPrimCount == std::accumulate(std::begin(stPrimCounts), std::end(stPrimCounts), 0);
}

/*!
 * \brief Checks whether a file is a VB file.
 * This is done by checking if the file's first 16 bytes are zero.
 * FIXME: This is some really shitty detection.
 * \param file File to check.
 * \return Whether the file is a VB file.
 */
inline bool fileIsVB(const QByteArray& file)
{
    return file.left(16).compare(QByteArray::fromHex("00000000000000000000000000000000")) == 0;
}

/*!
 * \brief Checks whether a file is a VH file.
 * This is done by checking if the file's first 4 bytes are equal to the 
 * little-endian string "VABp".
 * \param file File to check.
 * \return Whether the file is a VH file.
 */
inline bool fileIsVH(const QByteArray& file)
{
    return file.left(4).compare(QByteArray::fromHex("70424156")) == 0;
}

extern const QString valNameStr;
inline QString valueAndName_(uint32_t val, const QString name)
{
    return valNameStr.arg(val).arg(name);
}

} // namespace Utilities

#define valueAndName(val, name) \
Utilities::valueAndName_(static_cast<uint32_t>(val), name)

#endif // UTILITIES_H
