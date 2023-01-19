#ifndef KF1_LEVELCURVEENTRY_H
#define KF1_LEVELCURVEENTRY_H

#include <cstdint>

namespace KF1 {

/*!
 * \brief Holds HP, MP, Str and Magic Power Differences and Experience for the next level for each level the player can achieve.
 */
struct LevelCurveEntry
{
    uint16_t m_hpAtLevel;
    uint16_t m_mpAtLevel;
    uint16_t m_physicalGain;
    uint16_t m_magicalGain;
    uint16_t m_experienceToNext;
} __attribute__((packed, aligned(1)));

} // namespace KF1

#endif // KF1_LEVELCURVEENTRY_H
