#ifndef KF2_WEAPON_H
#define KF2_WEAPON_H

#include "magic.h"
#include "types/ps1/libgte.h"
#include <cstdint>

namespace KF2
{

// Structs

/*!
 * \brief Stats structure for weapons.
 */
struct WeaponParams
{
    uint8_t soundEffect;
    uint8_t staminaRecharge;
    uint8_t unknown0x2;
    MagicID magicA;
    MagicID magicB;
    uint8_t magicAShots;
    uint16_t slashOff;
    uint16_t chopOff;
    uint16_t stabOff;
    uint16_t holyOff;
    uint16_t fireOff;
    uint16_t earthOff;
    uint16_t windOff;
    uint16_t waterOff;
    uint16_t hpRestoreTick;
    uint16_t mpRestoreTick;
    int16_t swingRange;
    uint16_t swingSpeed;
    uint16_t swingDamageTime;
    uint16_t magicACastTimeMin;
    uint16_t magicACastTimeMax;
    int16_t magicBCastSpeed;
    uint16_t unknown0x26;
    uint16_t Spell2DamageStartTime;
    uint16_t unknown0x2a;
    uint16_t unknown0x2c;
    uint16_t unknown0x2e;
    uint16_t unknown0x30;
    int16_t unknown0x32;
    PS1::SVECTOR position;
    PS1::SVECTOR rotation;
} __attribute__((packed, aligned(1)));

// Functions

QString getWeaponName(const uint8_t weaponIndex);

}

#endif // KF2_WEAPON_H
