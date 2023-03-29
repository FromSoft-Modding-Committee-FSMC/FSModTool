#ifndef SEQ_H
#define SEQ_H

#include "core/kfmterror.h"
#include "utilities.h"
#include <QByteArray>

namespace PS1
{

class SEQ
{
public:
    SEQ(QByteArray& data, uint32_t offset = 0) : m_data(data), m_dataOffset(offset)
    {
        fsmt_assert(signature() == 0x53455170, "PSX::SEQ::SEQ: Signature mismatch!");
    }
    
    uint32_t fileSize() const
    {
    }
    uint32_t& signature() const { return Utilities::as<uint32_t>(m_data, m_dataOffset); }
    
private:
    QByteArray& m_data;
    uint32_t m_dataOffset;
};

} // namespace PSX

#endif // SEQ_H
