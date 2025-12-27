#ifndef MODEL_H
#define MODEL_H

#include "datahandlers/kfmtdatahandler.h"
#include <QDataStream>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QVector4D>
#include <QVector2D>
#include <vector>

class Model : public KFMTDataHandler
{
public:
    // 1. Define MOPacket first (needed by Vec3)
    struct MOPacket
    {
        short x = 0;
        short y = 0;
        short z = 0;
        MOPacket() = default;
        MOPacket(short x_, short y_, short z_) : x(x_), y(y_), z(z_) {}
        MOPacket(const MOPacket& other) : x(other.x), y(other.y), z(other.z) {}
    };

    // 2. Define Vec3 (needed by Mesh)
    struct Vec3
    {
        float x;
        float y;
        float z;
        void applyPacket(const MOPacket& packet);
        void readSVECTOR(QDataStream& in);
        operator QVector3D() const { return {x, y, z}; }
    };

    // 3. Define Primitive (needed by Mesh)
    struct Primitive
    {
        enum class PrimitiveFlag;
        enum class PrimitiveMode;

        PrimitiveFlag flag;
        PrimitiveMode mode;
        uint8_t ilen;
        uint8_t olen;
        uint8_t r0 = 255, g0 = 255, b0 = 255;
        uint8_t r1 = 255, g1 = 255, b1 = 255;
        uint8_t r2 = 255, g2 = 255, b2 = 255;
        uint8_t r3 = 255, g3 = 255, b3 = 255;
        uint8_t alpha = 255;
        uint8_t u0, v0, u1, v1, u2, v2, u3, v3;
        uint16_t cba, tsb;
        uint16_t vertex0, vertex1, vertex2, vertex3;
        uint16_t normal0, normal1, normal2, normal3;

        QVector4D Colour0() const { return {r0 / 255.f, g0 / 255.f, b0 / 255.f, alpha / 255.f}; }
        QVector4D Colour1() const { return {r1 / 255.f, g1 / 255.f, b1 / 255.f, alpha / 255.f}; }
        QVector4D Colour2() const { return {r2 / 255.f, g2 / 255.f, b2 / 255.f, alpha / 255.f}; }
        QVector4D Colour3() const { return {r3 / 255.f, g3 / 255.f, b3 / 255.f, alpha / 255.f}; }

        std::vector<QVector2D> getAdaptedTexCoords() const;
        bool isDoubleSided() const { return (static_cast<uint8_t>(flag) >> 1) & 1; }
        bool isGouraud() const { return ((static_cast<uint8_t>(mode) >> 5) & 1) == 1; }
        bool isGradation() const { return static_cast<uint8_t>(flag) > 3; }
        bool isLit() const { return (static_cast<uint8_t>(flag) & 1) == 0; }
        bool isQuad() const {
            auto mode_ = static_cast<uint8_t>(mode);
            return (mode_ >> 5) && ((mode_ >> 3) & 1);
        }
        bool isSmooth() const { return (static_cast<uint8_t>(mode) >> 4) & 1; }
        bool isTextured() const { return (static_cast<uint8_t>(mode) >> 2) & 1; }
        bool isTriangle() const {
            auto mode_ = static_cast<uint8_t>(mode);
            return (mode_ >> 5) && !((mode_ >> 3) & 1);
        }

        void readFrom(QDataStream& stream);
        void readGradation(QDataStream& stream) {
            if (isGradation()) {
                stream >> r1 >> g1 >> b1; stream.skipRawData(1);
                stream >> r2 >> g2 >> b2; stream.skipRawData(1);
                if (isQuad()) { stream >> r3 >> g3 >> b3; stream.skipRawData(1); }
            }
        }
        void readx20(QDataStream& stream);
        void readx24(QDataStream& stream);
        void readx25(QDataStream& stream);
        void readx28(QDataStream& stream);
        void readx2c(QDataStream& stream);
        void readx30(QDataStream& stream);
        void readx34(QDataStream& stream);
        void readx38(QDataStream& stream);
        void readx3c(QDataStream& stream);
    };

    // 4. Define Mesh (Now it knows what Vec3 and Primitive are!)
    struct Mesh
    {
        std::vector<Vec3> vertices;
        std::vector<Vec3> normals;
        std::vector<Primitive> primitives;
        bool visible = true;
        Vec3& operator[](size_t vertex) { return vertices[vertex]; }
    };

    struct MIMOrMOHeader
    {
        uint32_t animationCount;
        uint32_t tmdOffset;
        uint32_t morphTargetsOffset;
        uint32_t animationsOffset;
    };

    struct MOAnimation { std::vector<size_t> frameIndexes; };

    struct MOFrame
    {
        short Unknown00 = 0, Weight = 0, frameID = 0;
        std::vector<uint16_t> targets;
        MOFrame(short u, short w, short f, short t) : Unknown00(u), Weight(w), frameID(f) { targets.resize(t); }
    };

    explicit Model(KFMTFile& modelFile);
    void saveChanges() override {}

    std::vector<Mesh> baseObjects;
    float scale = 1.0f;
    std::vector<MOAnimation> animations;
    std::vector<MOFrame> animFrames;
    std::vector<Mesh> morphTargets;

private:
    void fixShiftedIndices();
    void loadMIM(const QByteArray& file);
    void loadMO(const QByteArray& file);
    void loadRTMD(const QByteArray& file);
    void loadTMD(const QByteArray& file);
    Model::MIMOrMOHeader readMIMOrMOHeader(QDataStream& stream);
};

QDataStream& operator>>(QDataStream& in, Model::Primitive& primitive);

enum class Model::Primitive::PrimitiveFlag
{
    SingleColorSingleFaceLightSourceNoCalc = 0b00000000,
    SingleColorSingleFaceLightSourceCalc = 0b00000001,
    SingleColorDoubleFaceLightSourceNoCalc = 0b00000010,
    SingleColorDoubleFaceLightSourceCalc = 0b00000011,
    GradationSingleFaceLightSourceNoCalc = 0b00000100,
    GradationSingleFaceLightSourceCalc = 0b00000101,
    GradationDoubleFaceLightSourceNoCalc = 0b00000110,
    GradationDoubleFaceLightSourceCalc = 0b00000111
};

enum class Model::Primitive::PrimitiveMode
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

#endif // MODEL_H
