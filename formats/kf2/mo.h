#ifndef MO_H
#define MO_H

#include "core/kfmterror.h"
#include "types/ps1/libgte.h"
#include "utilities.h"
#include <QByteArray>

namespace KF2
{

class MO {
public:
    struct Frame
    {
        uint16_t playMode;
        int16_t playRate;
        uint16_t mainTargetIndex;
        uint16_t journeyTargetCount;
        std::vector<uint16_t> journeyTargetIndices;
    };

    struct Animation
    {
        uint32_t frameCount;
        std::vector<Frame> frames;
    };

    MO(QByteArray& data, uint32_t offset) : m_data(data), m_dataOffset(offset)
    {
        uint32_t tmdOffset = Utilities::as<uint32_t>(m_data, m_dataOffset + 8);

        fsmt_assert(Utilities::as<uint32_t>(m_data, m_dataOffset + tmdOffset) == 0x41, "File is not a MO!");
    }

    uint32_t fileSize() const
    {
        return Utilities::as<uint32_t>(m_data, m_dataOffset);
    }

    uint32_t animationCount() const
    {
        return Utilities::as<uint32_t>(m_data, m_dataOffset + 4);
    }

    Animation animation(uint32_t index)
    {
        fsmt_assert(index < animationCount(), "Out of bounds animation index!");

        uint32_t* animationTable = reinterpret_cast<uint32_t*>(m_data.data() + m_dataOffset + Utilities::as<uint32_t>(m_data, m_dataOffset + 16));

        uint32_t  animationFrameCount = Utilities::as<uint32_t>(m_data, m_dataOffset + animationTable[index]);
        uint32_t* animationFrameOffsets = reinterpret_cast<uint32_t*>(m_data.data() + m_dataOffset + animationTable[index] + 4);

        Animation newAnimation;
        for(size_t i = 0; i < animationFrameCount; i++)
        {
            uint16_t* frameData = reinterpret_cast<uint16_t*>(m_data.data() + m_dataOffset + animationFrameOffsets[i]);

            Frame newFrame;
            newFrame.playMode = frameData[0];
            newFrame.playRate = static_cast<int16_t>(frameData[1]);
            newFrame.mainTargetIndex = frameData[2];
            newFrame.journeyTargetCount = frameData[3];

            for(size_t j = 0; j < newFrame.journeyTargetCount; j++)
            {
                newFrame.journeyTargetIndices.push_back(frameData[4 + j]);
            }

            newAnimation.frames.push_back(newFrame);
        }

        return newAnimation;
    }

    uint32_t targetCount() const
    {
        fsmt_assert(animationCount() > 0, "MO File has no animations, and so cannot have targets!");

        uint32_t morphTargetTableStart = Utilities::as<uint32_t>(m_data, m_dataOffset + 12);
        uint32_t morphTargetTableFirst = Utilities::as<uint32_t>(m_data, m_dataOffset + morphTargetTableStart);

        return (((morphTargetTableFirst - morphTargetTableStart) + 3) & 0xFFFFFFFC) / 4;
    }

    std::vector<PS1::SVECTOR> decompressTarget(uint32_t index, std::vector<PS1::SVECTOR> baseTarget)
    {
        std::vector<PS1::SVECTOR> newTarget;
        newTarget.reserve(baseTarget.size());

        //Get the target
        uint32_t morphTargetTableStart = Utilities::as<uint32_t>(m_data, m_dataOffset + 12);
        uint32_t* morphTargetTable = reinterpret_cast<uint32_t*>(m_data.data() + m_dataOffset + morphTargetTableStart);
        int16_t* target = reinterpret_cast<int16_t*>(m_data.data() + m_dataOffset + morphTargetTable[index]);

        //Actual decompression of the target
        uint16_t vertexCount = 0;
        uint16_t packetCount = static_cast<uint16_t>(target[0]++);

        while(packetCount > 0)
        {
            int16_t vertXorID  = target[0];
            int16_t vertYorNum = target[1];

            if(vertXorID == -32768)
            {
                //Copy Vertices
                while(vertYorNum > 0)
                {
                    newTarget[vertexCount] = baseTarget[vertexCount];
                    vertexCount++;
                }

                target += 2;
            }else{
                //Raw Vertex
                int16_t vertZ = target[2];

                newTarget.push_back(PS1::SVECTOR(vertXorID, vertYorNum, vertZ, 0));

                vertexCount++;

                target += 3;
            }

            packetCount--;
        }

        fsmt_assert(vertexCount == baseTarget.size(), "Decoded target vertex count is not equal to the number of vertices in the baseTarget!");

        return newTarget;
    }

private:
    QByteArray& m_data;
    uint32_t m_dataOffset;
};

}
#endif
