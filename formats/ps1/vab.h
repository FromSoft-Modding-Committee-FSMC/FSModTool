#ifndef VAB_H
#define VAB_H

#include "core/kfmterror.h"
#include "utilities.h"
#include <QByteArray>

namespace PS1
{

class VABHeader
{
public:
    struct ProgAttr
    {
        uint8_t m_vagAttrSetCount;
        uint8_t m_masterVol;
        uint8_t m_priority;
        uint8_t m_sourceMode;
        uint8_t m_pan;
        int8_t m_reserved0;
        int16_t m_attribute;
        uint32_t m_reserved1;
        uint32_t m_reserved2;
    };
    struct VAGAttr
    {
        uint8_t m_priority;
        uint8_t m_sourceMode;
        uint8_t m_volume;
        uint8_t m_pan;
        uint8_t m_centerNote;
        uint8_t m_centerFineTune;
        uint8_t m_minNote;
        uint8_t m_maxNote;
        uint8_t m_vibratoWidth;
        uint8_t m_vibratoPeriod;
        uint8_t m_portamentoWidth;
        uint8_t m_portamentoPeriod;
        uint8_t m_minPitchBend;
        uint8_t m_maxPitchBend;
        uint8_t m_reserved0;
        uint8_t m_reserved1;
        uint16_t m_adsr1;
        uint16_t m_adsr2;
        int16_t m_masterProgram;
        int16_t m_vagId;
        int16_t m_reserved[4];
    };
    
    VABHeader(QByteArray& data, uint32_t offset = 0) : m_data(data), m_dataOffset(offset)
    {
        fsmt_assert(signature() == 0x56414270, "PSX::VABHeader::VABHeader: Signature mismatch!");
    }
    
    uint8_t& bankAttr1() const { return Utilities::as<uint8_t>(m_data, m_dataOffset + 26u); }
    uint8_t& bankAttr2() const { return Utilities::as<uint8_t>(m_data, m_dataOffset + 27u); }
    uint32_t headerFileSize() const
    {
        // 32 for the base header, program attribute count, VAG attribute count and VAG offset count.
        return 32u + (128u * sizeof(ProgAttr)) + (programCount() * 16 * sizeof(VAGAttr)) + 512u;
    }
    uint32_t& id() const { return Utilities::as<uint32_t>(m_data, m_dataOffset + 8u); }
    uint8_t& masterPan() const { return Utilities::as<uint8_t>(m_data, m_dataOffset + 25u); }
    uint8_t& masterVol() const { return Utilities::as<uint8_t>(m_data, m_dataOffset + 24u); }
    ProgAttr& programAttribute(uint8_t index) const
    {
        fsmt_assert(index < programCount(), "PSX::VABHeader::programAttribute: Index over program attribute count!");
        return Utilities::as<ProgAttr>(m_data, m_dataOffset + 32u + sizeof(ProgAttr));
    }
    uint16_t& programCount() const { return Utilities::as<uint16_t>(m_data, m_dataOffset + 18u); }
    uint32_t& signature() const { return Utilities::as<uint32_t>(m_data, m_dataOffset); }
    uint16_t& toneCount() const { return Utilities::as<uint16_t>(m_data, m_dataOffset + 20u); }
    VAGAttr& vagAttribute(uint8_t index) const
    {
        fsmt_assert(index < vagCount(), "PSX::VABHeader::vagAttribute: Index over program attribute count!");
        uint32_t vagAttrsOffset = m_dataOffset + 32u + sizeof(ProgAttr) * programCount();
        return Utilities::as<VAGAttr>(m_data, vagAttrsOffset + sizeof(VAGAttr)*index);
    }
    uint16_t& vagCount() const { return Utilities::as<uint16_t>(m_data, m_dataOffset + 22u); }
    uint32_t waveformFileSize() const
    {
        return Utilities::as<uint32_t>(m_data, m_dataOffset + 12u) - headerFileSize();
    }
    
private:
    QByteArray& m_data;
    uint32_t m_dataOffset;
};

}

#endif // VAB_H
