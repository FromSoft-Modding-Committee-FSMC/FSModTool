#include "sfx.h"
#include "core/kfmtcore.h"

QString KF2::getSoundName(const KF2::SoundID soundId)
{
    switch (soundId)
    {
        case SoundID::Sword1: return QStringLiteral(u"Sword 1");
        case SoundID::Sword2: return QStringLiteral(u"Sword 2");
        case SoundID::Sword3: return QStringLiteral(u"Sword 3");
        case SoundID::BowPullback: return QStringLiteral(u"Bow Pullback");
        case SoundID::ArrowShot: return QStringLiteral(u"Arrow Shot");
        case SoundID::HarvinesFlute: return QStringLiteral(u"Harvine's Flute");
        case SoundID::GroundBell: return QStringLiteral(u"Ground Bell");
        case SoundID::PlayerDeath: return QStringLiteral(u"Player Death");
        case SoundID::PlayerLanding: return QStringLiteral(u"Player Landing");
        case SoundID::UsingItem: return QStringLiteral(u"Using Item");
        case SoundID::PlacingItem: return QStringLiteral(u"Placing Item");
        case SoundID::MenuMove: return QStringLiteral(u"Menu Move");
        case SoundID::MenuSelect: return QStringLiteral(u"Menu Select");
        case SoundID::MenuReturn: return QStringLiteral(u"Menu Return");
        case SoundID::GuyraCube: return QStringLiteral(u"Guyra Cube");
        case SoundID::MagicSound01: return QStringLiteral(u"Magic Sound 01");
        case SoundID::MagicSound02: return QStringLiteral(u"Magic Sound 02");
        case SoundID::MagicSound03: return QStringLiteral(u"Magic Sound 03");
        case SoundID::MagicSound04: return QStringLiteral(u"Magic Sound 04");
        case SoundID::MagicSound05: return QStringLiteral(u"Magic Sound 05");
        case SoundID::MagicSound06: return QStringLiteral(u"Magic Sound 06");
        case SoundID::MagicSound07: return QStringLiteral(u"Magic Sound 07");
        case SoundID::MagicSound08: return QStringLiteral(u"Magic Sound 08");
        case SoundID::MagicSound09: return QStringLiteral(u"Magic Sound 09");
        case SoundID::MagicSound10: return QStringLiteral(u"Magic Sound 10");
        case SoundID::BowLoose: return QStringLiteral(u"Bow Loose");
        case SoundID::MagicSound11: return QStringLiteral(u"Magic Sound 11");
        case SoundID::MagicSound12: return QStringLiteral(u"Magic Sound 12");
        case SoundID::MagicSound13: return QStringLiteral(u"Magic Sound 13");
        case SoundID::MagicSound14: return QStringLiteral(u"Magic Sound 14");
        case SoundID::MagicSound15: return QStringLiteral(u"Magic Sound 15");
        case SoundID::MagicSound16: return QStringLiteral(u"Magic Sound 16");
        case SoundID::MagicSound17: return QStringLiteral(u"Magic Sound 17");
        case SoundID::MagicSound18: return QStringLiteral(u"Magic Sound 18");
        case SoundID::MagicSound19: return QStringLiteral(u"Magic Sound 19");
        case SoundID::MagicSound20: return QStringLiteral(u"Magic Sound 20");
        case SoundID::EarthHeal: return QStringLiteral(u"Earth Heal");
        case SoundID::DoorOpen01: return QStringLiteral(u"Door Open 01");
        case SoundID::DoorOpen02: return QStringLiteral(u"Door Open 02");
        case SoundID::DoorOpen03: return QStringLiteral(u"Door Open 03");
        case SoundID::DoorOpen04: return QStringLiteral(u"Door Open 04");
        case SoundID::DoorOpen05: return QStringLiteral(u"Door Open 05");
        case SoundID::DoorOpen06: return QStringLiteral(u"Door Open 06");
        case SoundID::DoorOpen07: return QStringLiteral(u"Door Open 07");
        case SoundID::LeverActivation: return QStringLiteral(u"Lever Activation");
        case SoundID::WallSpikes: return QStringLiteral(u"Wall Spikes");
        case SoundID::SwingingScythes: return QStringLiteral(u"Swinging Scythes");
        case SoundID::SwingingSpikeBall: return QStringLiteral(u"Swinging Spike Ball");
        case SoundID::WaterWell: return QStringLiteral(u"Water Well");
        case SoundID::ActualWaterFall: return QStringLiteral(u"Actual Waterfall");
        case SoundID::None: return QStringLiteral(u"None");
        default: return unknownString;
    }
}
