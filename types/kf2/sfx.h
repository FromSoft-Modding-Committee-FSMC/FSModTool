#ifndef SFX_H
#define SFX_H

#include <cstdint>
#include <QString>

namespace KF2
{

// Enum containing the IDs of common game sounds.
enum class SoundID : uint8_t
{
    Sword1=0,
    Sword2=1,
    Sword3=2,
    BowPullback=3,
    ArrowShot=5,
    HarvinesFlute=8,
    GroundBell=9,
    PlayerDeath=11,
    PlayerLanding=12,
    UsingItem=13,
    PlacingItem=14,
    MenuMove=16,
    MenuSelect=17,
    MenuReturn=18,
    GuyraCube=20,
    MagicSound01=21,
    MagicSound02=22,
    MagicSound03=23,
    MagicSound04=24,
    MagicSound05=26,
    MagicSound06=27,
    MagicSound07=28,
    MagicSound08=29,
    MagicSound09=30,
    MagicSound10=31,
    BowLoose=32,
    MagicSound11=33,
    MagicSound12=34,
    MagicSound13=35,
    MagicSound14=36,
    MagicSound15=37,
    MagicSound16=38,
    MagicSound17=39,
    MagicSound18=40,
    MagicSound19=41,
    MagicSound20=42,
    EarthHeal=43,
    DoorOpen01=68,
    DoorOpen02=69,
    DoorOpen03=70,
    DoorOpen04=73,
    DoorOpen05=74,
    DoorOpen06=75,
    DoorOpen07=76,
    LeverActivation=77,
    WallSpikes=78,
    SwingingScythes=224,
    SwingingSpikeBall=225,
    WaterWell=238,
    ActualWaterFall=239,
    None=255
};

QString getSoundName(const SoundID soundId);

} // namespace KF2

#endif // SFX_H
