#include "magic.h"
#include "core/kfmtcore.h"

QString KF2::getMagicName(const KF2::MagicID magicId)
{
    switch (magicId)
    {
        case MagicID::ArcherArrow: return QStringLiteral(u"Archer Arrow");
        case MagicID::ArcherParalyzeArrow: return QStringLiteral(u"Archer Paralyze Arrow");
        case MagicID::Breath: return QStringLiteral(u"Breath");
        case MagicID::DarkSlayerMagicA: return QStringLiteral(u"Dark Slayer Magic A");
        case MagicID::DarkSlayerMagicB: return QStringLiteral(u"Dark Slayer Magic B");
        case MagicID::Dispoison: return QStringLiteral(u"Dispoison");
        case MagicID::DoubleFireball: return QStringLiteral(u"Double Fireball");
        case MagicID::EarthHeal: return QStringLiteral(u"Earth Heal");
        case MagicID::EarthWave: return QStringLiteral(u"Earth Wave");
        case MagicID::EnemyEarthMagic: return QStringLiteral(u"Enemy Earth Magic");
        case MagicID::EnemyHomingEarthMagic: return QStringLiteral(u"Enemy Homing Earth Magic");
        case MagicID::EnemyLightningVolt: return QStringLiteral(u"Enemy Lightning Volt");
        case MagicID::FireBall: return QStringLiteral(u"Fire Ball");
        case MagicID::FireStorm: return QStringLiteral(u"Fire Storm");
        case MagicID::FireWall: return QStringLiteral(u"Fire Wall");
        case MagicID::Flame: return QStringLiteral(u"Flame");
        case MagicID::FlameSwordMagicA: return QStringLiteral(u"Flame Sword Magic A");
        case MagicID::FlameSwordMagicB: return QStringLiteral(u"Flame Sword Magic B");
        case MagicID::Flash: return QStringLiteral(u"Flash");
        case MagicID::Freeze: return QStringLiteral(u"Freeze");
        case MagicID::IceBladeMagicA: return QStringLiteral(u"Ice Blade Magic A");
        case MagicID::IceBladeMagicB: return QStringLiteral(u"Ice Blade Magic B");
        case MagicID::IceStorm: return QStringLiteral(u"Ice Storm");
        case MagicID::Light: return QStringLiteral(u"Light");
        case MagicID::LightningVolt: return QStringLiteral(u"Lightning Volt");
        case MagicID::Meteor: return QStringLiteral(u"Meteor");
        case MagicID::MissileShield: return QStringLiteral(u"Missile Shield");
        case MagicID::MoonlightSwordMagicA: return QStringLiteral(u"Moonlight Sword Magic A");
        case MagicID::MoonlightSwordMagicB: return QStringLiteral(u"Moonlight Sword Magic B");
        case MagicID::None: return QStringLiteral(u"None");
        case MagicID::PsytheCurseOrb: return QStringLiteral(u"Psythe Curse Orb");
        case MagicID::ResistFire: return QStringLiteral(u"Resist Fire");
        case MagicID::SKnightPoisonBreath: return QStringLiteral(u"S-Knight Poison Breath");
        case MagicID::Seath: return QStringLiteral(u"Seath");
        case MagicID::SeathsSwordMagicA: return QStringLiteral(u"Seath's Sword Magic A");
        case MagicID::ShidenMagicA: return QStringLiteral(u"Shiden Magic A");
        case MagicID::ShidenMagicB: return QStringLiteral(u"Shiden Magic B");
        case MagicID::SpiderSwordMagicA: return QStringLiteral(u"Spider Sword Magic A");
        case MagicID::SpiderSwordMagicB: return QStringLiteral(u"Spider Sword Magic B");
        case MagicID::Stone: return QStringLiteral(u"Stone");
        case MagicID::TarnWindMagic: return QStringLiteral(u"Tarn Wind Magic");
        case MagicID::WaterFall: return QStringLiteral(u"Water Fall");
        case MagicID::WindCutter: return QStringLiteral(u"Wind Cutter");
        default: return unknownString;
    }
}
