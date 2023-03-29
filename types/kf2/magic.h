#ifndef KF2_MAGIC_H
#define KF2_MAGIC_H

#include <cstdint>
#include <QString>

namespace KF2
{

// Enums

/*!
 * \brief Enum for the Magic IDs in King's Field.
 */
enum class MagicID : uint8_t
{
    ArcherArrow = 31,
    ArcherParalyzeArrow = 29,
    Breath = 19,
    DarkSlayerMagicA = 44,
    DarkSlayerMagicB = 45,
    Dispoison = 14,
    DoubleFireball = 23,
    EarthHeal = 16,
    EarthWave = 2,
    EnemyEarthMagic = 32,
    EnemyHomingEarthMagic = 33,
    EnemyLightningVolt = 54,
    FireBall = 7,
    FireStorm = 9,
    FireWall = 8,
    Flame = 10,
    FlameSwordMagicA = 49,
    FlameSwordMagicB = 50,
    Flash = 12,
    Freeze = 6,
    IceBladeMagicA = 38,
    IceBladeMagicB = 39,
    IceStorm = 5,
    Light = 18,
    LightningVolt = 11,
    Meteor = 3,
    MissileShield = 17,
    MoonlightSwordMagicA = 42,
    MoonlightSwordMagicB = 43,
    None = 255,
    PsytheCurseOrb = 26,
    ResistFire = 15,
    SKnightPoisonBreath = 22,
    Seath = 13,
    SeathsSwordMagicA = 40,
    ShidenMagicA = 34,
    ShidenMagicB = 35,
    SpiderSwordMagicA = 51,
    SpiderSwordMagicB = 52,
    Stone = 1,
    TarnWindMagic = 24,
    WaterFall = 0,
    WindCutter = 4
};

// Structs

/*!
 * \brief Structure for the definition of magic spells.
 */
struct MagicParams
{
    bool acquired;
    uint8_t focusRecharge;
    uint8_t unknown0x02;
    uint8_t unknown0x03;
    uint8_t statusInflicted;
    uint8_t unknown0x05;
    uint16_t slashOff;
    uint16_t chopOff;
    uint16_t stabOff;
    uint16_t holyOff;
    uint16_t fireOff;
    uint16_t earthOff;
    uint16_t windOff;
    uint16_t waterOff;
    uint16_t mpCost;
    uint8_t unknown0x18;
    uint8_t unknown0x19;
} __attribute__((packed, aligned(1)));

// Functions

QString getMagicName(const MagicID magicId);

}

#endif // KF2_MAGIC_H
