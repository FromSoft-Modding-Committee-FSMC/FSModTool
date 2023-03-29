#ifndef TIM_H
#define TIM_H

#include "core/kfmterror.h"
#include "utilities.h"
#include <QByteArray>

namespace PS1
{

class TIM
{
public:
    struct CLUTEntry
    {
        uint32_t m_entry;
        bool stp() const { return m_entry & 32768u; }
        uint8_t r() const { return (m_entry & 0x7c00) >> 10; }
        uint8_t g() const { return (m_entry & 0x3e0) >> 5; }
        uint8_t b() const { return (m_entry & 0x1f); }
    };
    
    struct DataHeader
    {
        uint32_t m_length;
        uint16_t m_x;
        uint16_t m_y;
        uint16_t m_h;
        uint16_t m_w;
    };
    
    enum class PixelMode
    {
        CLUT4Bit = 0,
        CLUT8Bit = 1,
        Direct15Bit = 2,
        Direct24Bit = 3,
        Mixed = 4
    };

    TIM(QByteArray& data, uint32_t offset = 0) : m_data(data), m_dataOffset(offset)
    {
        fsmt_assert(signature() == 0x10, "PSX::TIM::TIM: Signature mismatch!");
    }

    uint16_t clutSize() const
    {
        switch (pixelMode())
        {
            case PixelMode::CLUT4Bit: return 16u; break;
            case PixelMode::CLUT8Bit: return 256u; break;
            default: return 0;
        }
    }
    uint32_t fileSize() const
    {
        bool hasClut = clutSize() > 0;
        uint32_t size = 8;

        // Add the length specified in the first data block, be it a CLUT or pixel data.
        size += Utilities::as<DataHeader>(m_data, m_dataOffset + 8).m_length;

        // But if we DO have a CLUT, our size variable doubles as the offset to the pixel data header!
        if (hasClut) size += Utilities::as<DataHeader>(m_data, m_dataOffset + size).m_length;

        return size;
    }
    uint32_t& flag() const { return Utilities::as<uint32_t>(m_data, m_dataOffset + 4); }
    PixelMode pixelMode() const { return static_cast<PixelMode>(flag() & 7u); }
    uint32_t& signature() const { return Utilities::as<uint32_t>(m_data, m_dataOffset); }

private:
    QByteArray& m_data;
    uint32_t m_dataOffset;
};

} // namespace PSX

#endif // TIM_H
