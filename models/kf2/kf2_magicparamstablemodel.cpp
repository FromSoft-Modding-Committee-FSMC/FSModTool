#include "kf2_magicparamstablemodel.h"

QVariant KF2::Models::MagicParamsTableModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid()) return {};
    
    const auto& magic = stat(index.row());

    switch (index.column())
    {
        case 0: return QString::number(magic.acquired);
        case 1: return QString::number(magic.focusRecharge);
        case 2: return QString::number(magic.unknown0x02);
        case 3: return QString::number(magic.unknown0x03);
        case 4: return QString::number(magic.statusInflicted);
        case 5: return QString::number(magic.unknown0x05);
        case 6: return QString::number(magic.slashOff);
        case 7: return QString::number(magic.chopOff);
        case 8: return QString::number(magic.stabOff);
        case 9: return QString::number(magic.holyOff);
        case 10: return QString::number(magic.fireOff);
        case 11: return QString::number(magic.earthOff);
        case 12: return QString::number(magic.windOff);
        case 13: return QString::number(magic.waterOff);
        case 14: return QString::number(magic.mpCost);
        case 15: return QString::number(magic.unknown0x18);
        case 16: return QString::number(magic.unknown0x19);
        default: return {};
    }
}

QVariant KF2::Models::MagicParamsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return {};
    if (orientation == Qt::Vertical) return KF2::getMagicName(static_cast<KF2::MagicID>(section));

    //if (orientation == Qt::Horizontal):
    switch (section)
    {
        case 0: return QStringLiteral("Acquired");
        case 1: return QStringLiteral("Stamina Cost");
        case 2: return QStringLiteral("Unknown 02");
        case 3: return QStringLiteral("Unknown 03");
        case 4: return QStringLiteral("Status Inflicted");
        case 5: return QStringLiteral("Unknown 05");
        case 6: return QStringLiteral("Slash Off.");
        case 7: return QStringLiteral("Chop Off.");
        case 8: return QStringLiteral("Stab Off.");
        case 9: return QStringLiteral("Holy Off.");
        case 10: return QStringLiteral("Fire Off.");
        case 11: return QStringLiteral("Earth Off.");
        case 12: return QStringLiteral("Wind Off.");
        case 13: return QStringLiteral("Water Off.");
        case 14: return QStringLiteral("MP Cost");
        case 15: return QStringLiteral("Unknown 18");
        case 16: return QStringLiteral("Unknown 19");
        default: return {};
    }
}

bool KF2::Models::MagicParamsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;
    
    auto& magic = stat(index.row());

    if (!value.isValid() || value.toString().isEmpty() || role != Qt::EditRole || !index.isValid())
        return false;
    
    auto u8Value = Utilities::clampToByte(value.toUInt());
    auto u16Value = Utilities::clampToUShort(value.toUInt());

    switch (index.row())
    {
        case 0: magic.acquired = std::clamp(u8Value, (uint8_t) 0, (uint8_t) 1); break;
        case 1: magic.focusRecharge = u8Value; break;
        case 2: magic.unknown0x02 = u8Value; break;
        case 3: magic.unknown0x03 = u8Value; break;
        case 4: magic.statusInflicted = u8Value; break;
        case 5: magic.unknown0x05 = u8Value; break;
        case 6: magic.slashOff = u16Value; break;
        case 7: magic.chopOff = u16Value; break;
        case 8: magic.stabOff = u16Value; break;
        case 9: magic.holyOff = u16Value; break;
        case 10: magic.fireOff = u16Value; break;
        case 11: magic.earthOff = u16Value; break;
        case 12: magic.windOff = u16Value; break;
        case 13: magic.waterOff = u16Value; break;
        case 14: magic.mpCost = u16Value; break;
        case 15: magic.unknown0x18 = u8Value; break;
        case 16: magic.unknown0x19 = u8Value; break;
    }
    
    emit dataChanged(index, index);
    return result;
}
