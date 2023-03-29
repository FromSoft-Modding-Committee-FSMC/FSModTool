#include "kf2_weaponparamstablemodel.h"
#include "types/kf2/object.h"
#include "utilities.h"

QVariant KF2::Models::WeaponParamsTableModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid()) return {};
    
    const auto& weaponStat = stat(index.row());

    switch (index.column())
    {
        case 0: return QString::number(weaponStat.soundEffect);
        case 1: return QString::number(weaponStat.staminaRecharge);
        case 2: return QString::number(weaponStat.unknown0x2);
        case 3: return valueAndName(weaponStat.magicA, KF2::getMagicName(weaponStat.magicA));
        case 4: return valueAndName(weaponStat.magicB, KF2::getMagicName(weaponStat.magicB));
        case 5: return QString::number(weaponStat.magicAShots);
        case 6: return QString::number(weaponStat.slashOff);
        case 7: return QString::number(weaponStat.chopOff);
        case 8: return QString::number(weaponStat.stabOff);
        case 9: return QString::number(weaponStat.holyOff);
        case 10: return QString::number(weaponStat.fireOff);
        case 11: return QString::number(weaponStat.earthOff);
        case 12: return QString::number(weaponStat.windOff);
        case 13: return QString::number(weaponStat.waterOff);
        case 14: return QString::number(weaponStat.hpRestoreTick);
        case 15: return QString::number(weaponStat.mpRestoreTick);
        case 16: return QString::number(weaponStat.swingRange);
        case 17: return QString::number(weaponStat.swingSpeed);
        case 18: return QString::number(weaponStat.swingDamageTime);
        case 19: return QString::number(weaponStat.magicACastTimeMin);
        case 20: return QString::number(weaponStat.magicACastTimeMax);
        case 21: return QString::number(weaponStat.magicBCastSpeed);
        case 22: return QString::number(weaponStat.unknown0x26);
        case 23: return QString::number(weaponStat.Spell2DamageStartTime);
        case 24: return QString::number(weaponStat.unknown0x2a);
        case 25: return QString::number(weaponStat.unknown0x2c);
        case 26: return QString::number(weaponStat.unknown0x2e);
        case 27: return QString::number(weaponStat.unknown0x30);
        case 28: return QString::number(weaponStat.unknown0x32);
        case 29: return QString::number(weaponStat.position.vx);
        case 30: return QString::number(weaponStat.position.vy);
        case 31: return QString::number(weaponStat.position.vz);
        case 32: return QString::number(weaponStat.position.pad);
        case 33: return QString::number(weaponStat.rotation.vx);
        case 34: return QString::number(weaponStat.rotation.vy);
        case 35: return QString::number(weaponStat.rotation.vz);
        case 36: return QString::number(weaponStat.rotation.pad);
        default: return {};
    }
}

QVariant KF2::Models::WeaponParamsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return {};
    
    if (orientation == Qt::Vertical) return KF2::getObjectName(static_cast<KF2::ObjectID>(section));

    //if (orientation == Qt::Horizontal):
    switch (section)
    {
        case 0: return QStringLiteral("Sound Effect");
        case 1: return QStringLiteral("Stamina Cost");
        case 2: return QStringLiteral("Unknown 02");
        case 3: return QStringLiteral("Magic A");
        case 4: return QStringLiteral("Magic B");
        case 5: return QStringLiteral("Magic A Shots");
        case 6: return QStringLiteral("Slash Off.");
        case 7: return QStringLiteral("Chop Off.");
        case 8: return QStringLiteral("Stab Off.");
        case 9: return QStringLiteral("Holy Off.");
        case 10: return QStringLiteral("Fire Off.");
        case 11: return QStringLiteral("Earth Off.");
        case 12: return QStringLiteral("Wind Off.");
        case 13: return QStringLiteral("Water Off.");
        case 14: return QStringLiteral("HP Restore Timer");
        case 15: return QStringLiteral("MP Restore Timer");
        case 16: return QStringLiteral("Attack Range");
        case 17: return QStringLiteral("Swing Speed");
        case 18: return QStringLiteral("Swing Damage Time");
        case 19: return QStringLiteral("Magic A Cast Start Time");
        case 20: return QStringLiteral("Magic A Cast End Time");
        case 21: return QStringLiteral("Magic B Animation Speed");
        case 22: return QStringLiteral("Unknown 26");
        case 23: return QStringLiteral("Magic B Damage Time?");
        case 24: return QStringLiteral("Unknown 2A");
        case 25: return QStringLiteral("Unknown 2C");
        case 26: return QStringLiteral("Unknown 2E");
        case 27: return QStringLiteral("Unknown 30");
        case 28: return QStringLiteral("Unknown 32");
        case 29: return QStringLiteral("View Pos. x");
        case 30: return QStringLiteral("View Pos. y");
        case 31: return QStringLiteral("View Pos. z");
        case 32: return QStringLiteral("View Pos. pad");
        case 33: return QStringLiteral("View Rot. x");
        case 34: return QStringLiteral("View Rot. y");
        case 35: return QStringLiteral("View Rot. z");
        case 36: return QStringLiteral("View Rot. pad");
        default: return {};
    }
}

bool KF2::Models::WeaponParamsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!value.isValid() || value.toString().isEmpty() || role != Qt::EditRole || !index.isValid())
        return false;

    auto& weaponStat = stat(index.row());
    
    auto s16Value = Utilities::clampToShort(value.toInt());
    auto u8Value = Utilities::clampToByte(value.toUInt());
    auto u16Value = Utilities::clampToUShort(value.toUInt());

    switch (index.column())
    {
        case 0: weaponStat.soundEffect = u8Value; break;
        case 1: weaponStat.staminaRecharge = u8Value; break;
        case 2: weaponStat.unknown0x2 = u8Value; break;
        case 3:
            weaponStat.magicA = static_cast<KF2::MagicID>(u8Value);
            break;
        case 4:
            weaponStat.magicB = static_cast<KF2::MagicID>(u8Value);
            break;
        case 5: weaponStat.magicAShots = u8Value; break;
        case 6: weaponStat.slashOff = u16Value; break;
        case 7: weaponStat.chopOff = u16Value; break;
        case 8: weaponStat.stabOff = u16Value; break;
        case 9: weaponStat.holyOff = u16Value; break;
        case 10: weaponStat.fireOff = u16Value; break;
        case 11: weaponStat.earthOff = u16Value; break;
        case 12: weaponStat.windOff = u16Value; break;
        case 13: weaponStat.waterOff = u16Value; break;
        case 14: weaponStat.hpRestoreTick = u16Value; break;
        case 15: weaponStat.mpRestoreTick = u16Value; break;
        case 16: weaponStat.swingRange = s16Value; break;
        case 17: weaponStat.swingSpeed = u16Value; break;
        case 18: weaponStat.swingDamageTime = u16Value; break;
        case 19: weaponStat.magicACastTimeMin = u16Value; break;
        case 20: weaponStat.magicACastTimeMax = u16Value; break;
        case 21: weaponStat.magicBCastSpeed = s16Value; break;
        case 22: weaponStat.unknown0x26 = u16Value; break;
        case 23: weaponStat.Spell2DamageStartTime = u16Value; break;
        case 24: weaponStat.unknown0x2a = u16Value; break;
        case 25: weaponStat.unknown0x2c = u16Value; break;
        case 26: weaponStat.unknown0x2e = u16Value; break;
        case 27: weaponStat.unknown0x30 = u16Value; break;
        case 28: weaponStat.unknown0x32 = s16Value; break;
        case 29: weaponStat.position.vx = s16Value; break;
        case 30: weaponStat.position.vy = s16Value; break;
        case 31: weaponStat.position.vz = s16Value; break;
        case 32: weaponStat.position.pad = s16Value; break;
        case 33: weaponStat.rotation.vx = s16Value; break;
        case 34: weaponStat.rotation.vy = s16Value; break;
        case 35: weaponStat.rotation.vz = s16Value; break;
        case 36: weaponStat.rotation.pad = s16Value; break;
        default: return false;
    }

    emit dataChanged(index, index);
    return true;
}
