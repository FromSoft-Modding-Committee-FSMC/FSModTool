#ifndef KF2_ARMOR_H
#define KF2_ARMOR_H

#include <cstdint>

namespace KF2
{

// Structs

/*!
 * \brief Stats structure for armor.
 */
struct ArmourParams
{
    uint8_t unknown0x00;
    uint8_t unknown0x01;
    uint16_t slashDef;
    uint16_t chopDef;
    uint16_t stabDef;
    uint16_t poisonDef;
    uint16_t darkDef;
    uint16_t fireDef;
    uint16_t earthDef;
    uint16_t windDef;
    uint16_t waterDef;
    uint16_t hpRestoreTick;
    uint16_t hpReduceTick;
    uint8_t unknown0x18;
    uint8_t unknown0x19;
    uint8_t unknown0x1A;
    uint8_t unknown0x1B;
    uint8_t unknown0x1C;
    uint8_t unknown0x1D;
    uint8_t unknown0x1E;
    uint8_t unknown0x1F;
} __attribute__((packed, aligned(1)));
}

#endif // KF2_ARMOR_H
