#ifndef TMD_H
#define TMD_H

#include "core/kfmterror.h"
#include "types/ps1/libgte.h"
#include "utilities.h"
#include <QByteArray>

namespace PS1
{

class TMD
{
public:
    struct TMDObject
    {
        PS1::SVECTOR* vertices;
        PS1::SVECTOR* normals;
        void* primitives;
        uint32_t vertexCount;
        uint32_t normalCount;
        uint32_t primitiveCount;
        int32_t scale;
    };
    
    /*!
     * \brief Enum with terribly long names for the primitive flags.
     * Single/double faced only applies to polygon primitives.
     */
    enum class PrimitiveFlag;

    /*!
     * \brief Enum with terribly long names for the TMD primitive modes.
     */
    enum class PrimitiveMode;

    TMD(QByteArray& data, uint32_t offset = 0) : m_data(data), m_dataOffset(offset)
    {
        // Ensure this has the TMD signature.
        fsmt_assert(signature() == 0x41, "PSX::TMD::TMD: Signature mismatch!");
        // And that we have offsets and not real addresses.
        fsmt_assert(flags() == 0, "PSX::TMD::TMD: TMD has real addresses!");
    }
    
    uint32_t fileSize() const
    {
        auto& lastObject = Utilities::as<NativeObjectInfo>(m_data,
                                                           m_dataOffset + 12u
                                                           + (objectCount() * sizeof(NativeObjectInfo)));
        
        uint32_t curOffset = m_dataOffset + lastObject.primitiveOffset;
        
        for (uint32_t curPrim = 0; curPrim < lastObject.primitiveCount; curPrim++)
        {
            uint16_t ilen = Utilities::as<uint8_t>(m_data, curOffset + 1u);
            curOffset += 4 + (ilen * 4);
        }
        
        return curOffset;
    }
    uint32_t& flags() const { return Utilities::as<uint32_t>(m_data, m_dataOffset + 4u); }
    TMDObject object(uint32_t index)
    {
        auto& nativeObj = Utilities::as<NativeObjectInfo>(m_data,
                                                           m_dataOffset + 12u + (index * sizeof(NativeObjectInfo)));
        
        uint8_t* basePtr = reinterpret_cast<uint8_t*>(m_data.data()) + m_dataOffset + 12u;
         
        return
        {
            reinterpret_cast<PS1::SVECTOR*>(basePtr + nativeObj.vertexOffset),
            reinterpret_cast<PS1::SVECTOR*>(basePtr + nativeObj.normalOffset),
            basePtr + nativeObj.primitiveOffset,
            nativeObj.vertexCount,
            nativeObj.normalCount,
            nativeObj.primitiveCount,
            nativeObj.scale
        };
    }
    uint32_t& objectCount() const { return Utilities::as<uint32_t>(m_data, m_dataOffset + 8); }
    uint32_t& signature() const { return Utilities::as<uint32_t>(m_data, m_dataOffset); }

private:
    /*!
     * \brief Represents the native TMD object table entry.
     */
    struct NativeObjectInfo
    {
        uint32_t vertexOffset;
        uint32_t vertexCount;
        uint32_t normalOffset;
        uint32_t normalCount;
        uint32_t primitiveOffset;
        uint32_t primitiveCount;
        int32_t scale;
    };
    
    QByteArray& m_data;
    uint32_t m_dataOffset;
};

enum class TMD::PrimitiveFlag
{
    SingleColorSingleFaceLightSourceNoCalc = 0b00000000, ///< 0x0
    SingleColorSingleFaceLightSourceCalc = 0b00000001,   ///< 0x1
    SingleColorDoubleFaceLightSourceNoCalc = 0b00000010, ///< 0x2
    SingleColorDoubleFaceLightSourceCalc = 0b00000011,   ///< 0x3
    GradationSingleFaceLightSourceNoCalc = 0b00000100,   ///< 0x4
    GradationSingleFaceLightSourceCalc = 0b00000101,     ///< 0x5
    GradationDoubleFaceLightSourceNoCalc = 0b00000110,   ///< 0x6
    GradationDoubleFaceLightSourceCalc = 0b00000111      ///< 0x7
};

enum class TMD::PrimitiveMode
{
    x20TriFlatNoTexOpaqueLit = 0b00100000,
    x21TriFlatNoTexOpaqueUnlit = 0b00100001,
    x22TriFlatNoTexTranslucentLit = 0b00100010,
    x23TriFlatNoTexTranslucentUnlit = 0b00100011,
    x24TriFlatTexOpaqueLit = 0b00100100,
    x25TriFlatTexOpaqueUnlit = 0b00100101,
    x26TriFlatTexTranslucentLit = 0b00100110,
    x27TriFlatTexTranslucentUnlit = 0b00100111,
    x28QuadFlatNoTexOpaqueLit = 0b00101000,
    x29QuadFlatNoTexOpaqueUnlit = 0b00101001,
    x2aQuadFlatNoTexTranslucentLit = 0b00101010,
    x2bQuadFlatNoTexTranslucentUnlit = 0b00101011,
    x2cQuadFlatTexOpaqueLit = 0b00101100,
    x2dQuadFlatTexOpaqueUnlit = 0b00101101,
    x2eQuadFlatTexTranslucentLit = 0b00101110,
    x2fQuadFlatTexTranslucentUnlit = 0b00101111,
    x30TriGouraudNoTexOpaqueLit = 0b00110000,
    x31TriGouraudNoTexOpaqueUnlit = 0b00110001,
    x32TriGouraudNoTexTranslucentLit = 0b00110010,
    x33TriGouraudNoTexTranslucentUnlit = 0b00110011,
    x34TriGouraudTexOpaqueLit = 0b00110100,
    x35TriGouraudTexOpaqueUnlit = 0b00110101,
    x36TriGouraudTexTranslucentLit = 0b00110110,
    x37TriGouraudTexTranslucentUnlit = 0b00110111,
    x38QuadGouraudNoTexOpaqueLit = 0b00111000,
    x39QuadGouraudNoTexOpaqueUnlit = 0b00111001,
    x3aQuadGouraudNoTexTranslucentLit = 0b00111010,
    x3bQuadGouraudNoTexTranslucentUnlit = 0b00111011,
    x3cQuadGouraudTexOpaqueLit = 0b00111100,
    x3dQuadGouraudTexOpaqueUnlit = 0b00111101,
    x3eQuadGouraudTexTranslucentLit = 0b00111110,
    x3fQuadGouraudTexTranslucentUnlit = 0b00111111,
    x40LineGradationOffOpaque = 0b01000000,
    x42LineGradationOffTranslucent = 0b01000010,
    x50LineGradationOnOpaque = 0b01010000,
    x52LineGradationOnTranslucent = 0b01010010,
    x64SpriteFreeOpaque = 0b01100100,
    x66SpriteFreeTranslucent = 0b01100110,
    x6cSprite1x1Opaque = 0b01101100,
    x6eSprite1x1Translucent = 0b01101110,
    x74Sprite8x8Opaque = 0b01110100,
    x76Sprite8x8Translucent = 0b01110110,
    x7cSprite16x16Opaque = 0b01111100,
    x7eSprite16x16Translucent = 0b01111110
};

} // namespace PSX

#endif // TMD_H
