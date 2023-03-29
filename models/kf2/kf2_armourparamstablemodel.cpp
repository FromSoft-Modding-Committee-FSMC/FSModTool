#include "kf2_armourparamstablemodel.h"
#include "types/kf2/object.h"
#include "utilities.h"

QVariant KF2::Models::ArmourParamsTableModel::data(const QModelIndex & index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid()) return {};
    
    const auto& armorStat = stat(index.row());

    switch (index.column())
    {
        case 0: return QString::number(armorStat.unknown0x00);
        case 1: return QString::number(armorStat.unknown0x01);
        case 2: return QString::number(armorStat.slashDef);
        case 3: return QString::number(armorStat.chopDef);
        case 4: return QString::number(armorStat.stabDef);
        case 5: return QString::number(armorStat.poisonDef);
        case 6: return QString::number(armorStat.darkDef);
        case 7: return QString::number(armorStat.fireDef);
        case 8: return QString::number(armorStat.earthDef);
        case 9: return QString::number(armorStat.windDef);
        case 10: return QString::number(armorStat.waterDef);
        case 11: return QString::number(armorStat.hpRestoreTick);
        case 12: return QString::number(armorStat.hpReduceTick);
        case 13: return QString::number(armorStat.unknown0x18);
        case 14: return QString::number(armorStat.unknown0x19);
        case 15: return QString::number(armorStat.unknown0x1A);
        case 16: return QString::number(armorStat.unknown0x1B);
        case 17: return QString::number(armorStat.unknown0x1C);
        case 18: return QString::number(armorStat.unknown0x1D);
        case 19: return QString::number(armorStat.unknown0x1E);
        case 20: return QString::number(armorStat.unknown0x1F);
        default: return {};
    }
}

QVariant KF2::Models::ArmourParamsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return {};
    
    // TODO: Need to make this show the weapon's name.
    if (orientation == Qt::Vertical)
        return valueAndName(section, KF2::getObjectName(static_cast<KF2::ObjectID>(0x15 + section)));

    //if (orientation == Qt::Horizontal):
    switch (section)
    {
        case 0: return QStringLiteral(u"Unknown 00");
        case 1: return QStringLiteral(u"Unknown 01");
        case 2: return QStringLiteral(u"Slash Def.");
        case 3: return QStringLiteral(u"Chop Def.");
        case 4: return QStringLiteral(u"Stab Def.");
        case 5: return QStringLiteral(u"Poison Def.");
        case 6: return QStringLiteral(u"Dark Def.");
        case 7: return QStringLiteral(u"Fire Def.");
        case 8: return QStringLiteral(u"Earth Def.");
        case 9: return QStringLiteral(u"Wind Def.");
        case 10: return QStringLiteral(u"Water Def.");
        case 11: return QStringLiteral(u"Heal timer");
        case 12: return QStringLiteral(u"Hurt timer");
        case 13: return QStringLiteral(u"Unknown 18");
        case 14: return QStringLiteral(u"Unknown 19");
        case 15: return QStringLiteral(u"Unknown 1A");
        case 16: return QStringLiteral(u"Unknown 1B");
        case 17: return QStringLiteral(u"Unknown 1C");
        case 18: return QStringLiteral(u"Unknown 1D");
        case 19: return QStringLiteral(u"Unknown 1E");
        case 20: return QStringLiteral(u"Unknown 1F");
        default: return {};
    }
}

bool KF2::Models::ArmourParamsTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (!value.isValid() || value.toString().isEmpty() || role != Qt::EditRole || !index.isValid())
        return false;
    auto& armorStat = stat(index.row());
    
    auto u8Value = Utilities::clampToByte(value.toUInt());
    auto u16Value = Utilities::clampToUShort(value.toUInt());

    switch (index.column())
    {
        case 0: armorStat.unknown0x00 = u8Value; break;
        case 1: armorStat.unknown0x01 = u8Value; break;
        case 2: armorStat.slashDef = u16Value; break;
        case 3: armorStat.chopDef = u16Value; break;
        case 4: armorStat.stabDef = u16Value; break;
        case 5: armorStat.poisonDef = u16Value; break;
        case 6: armorStat.darkDef = u16Value; break;
        case 7: armorStat.fireDef = u16Value; break;
        case 8: armorStat.earthDef = u16Value; break;
        case 9: armorStat.windDef = u16Value; break;
        case 10: armorStat.waterDef = u16Value; break;
        case 11: armorStat.hpRestoreTick = u16Value; break;
        case 12: armorStat.hpReduceTick = u16Value; break;
        case 13: armorStat.unknown0x18 = u8Value; break;
        case 14: armorStat.unknown0x19 = u8Value; break;
        case 15: armorStat.unknown0x1A = u8Value; break;
        case 16: armorStat.unknown0x1B = u8Value; break;
        case 17: armorStat.unknown0x1C = u8Value; break;
        case 18: armorStat.unknown0x1D = u8Value; break;
        case 19: armorStat.unknown0x1E = u8Value; break;
        case 20: armorStat.unknown0x1F = u8Value; break;
        default: return false;
    }

    emit dataChanged(index, index);
    return true;
}
