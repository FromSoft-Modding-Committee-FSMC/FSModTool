#ifndef KF2_PLAYERLEVEL_H
#define KF2_PLAYERLEVEL_H

#include <cstdint>

namespace KF2
{

/*!
 * \brief Holds HP, MP, Str and Magic Power Differences and Experience for the next level for each level the player can achieve.
 */
struct LevelCurveEntry
{
    uint16_t maxHP;
    uint16_t maxMP;
    uint16_t physicalIncrement;
    uint16_t magicalIncrement;
    uint32_t expForNext;
} __attribute__((packed, aligned(1)));

} // namespace KF2

#endif // KF2_PLAYERLEVEL_H
