#include "kf2_objectclassestablemodel.h"
#include "utilities.h"

QVariant KF2::Models::ObjectClassesTableModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid()) return {};
    
    const auto& objClass = stat(index.row());

    switch (index.column())
    {
        case 0:
            return valueAndName(objClass.type, KF2::getObjectClassTypeName(objClass.type));
        case 1: return QString::number(objClass.transformType);
        case 2: return QString::number(objClass.unknown0x02);
        case 3: return QString::number(objClass.unknown0x03);
        case 4: return QString::number(objClass.collisionCylinderPrimaryRadius);
        case 5: return QString::number(objClass.collisionCylinderSecondaryRadius);
        case 6: return QString::number(objClass.collisionCylinderHeight);
        case 7: return QString::number(objClass.unknown0x0a);
        case 8: return QString::number(objClass.unknown0x0c);
        case 9: return QString::number(objClass.unknown0x0d);
        case 10: return QString::number(objClass.unknown0x0e);
        case 11: return valueAndName(objClass.soundEffect0, KF2::getSoundName(objClass.soundEffect0));
        case 12: return QString::number(objClass.unknown0x10);
        case 13: return QString::number(objClass.unknown0x11);
        case 14: return QString::number(objClass.unknown0x12);
        case 15: return QString::number(objClass.unknown0x13);
        case 16: return QString::number(objClass.unknown0x14);
        case 17: return QString::number(objClass.unknown0x15);
        case 18: return valueAndName(objClass.soundEffect1, KF2::getSoundName(objClass.soundEffect1));
        case 19: return QString::number(objClass.unknown0x17);
        default: return {};
    }
}

QVariant KF2::Models::ObjectClassesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return {};
    
    if (orientation == Qt::Vertical) return KF2::getObjectName(static_cast<KF2::ObjectID>(section));

    //if (orientation == Qt::Horizontal):
    switch (section)
    {
        case 0: return QStringLiteral("Class Type");
        case 1: return QStringLiteral("Transform Type");
        case 2: return QStringLiteral("Unknown 02");
        case 3: return QStringLiteral("Unknown 03");
        case 4: return QStringLiteral("Coll. Cyl. 1º Radius");
        case 5: return QStringLiteral("Coll. Cyl. 2º Radius");
        case 6: return QStringLiteral("Coll. Cyl. Height");
        case 7: return QStringLiteral("Unknown 0A");
        case 8: return QStringLiteral("Unknown 0C");
        case 9: return QStringLiteral("Unknown 0D");
        case 10: return QStringLiteral("Unknown 0E");
        case 11: return QStringLiteral("Sound Effect A");
        case 12: return QStringLiteral("Unknown 10");
        case 13: return QStringLiteral("Unknown 11");
        case 14: return QStringLiteral("Unknown 12");
        case 15: return QStringLiteral("Unknown 13");
        case 16: return QStringLiteral("Unknown 14");
        case 17: return QStringLiteral("Unknown 15");
        case 18: return QStringLiteral("Sound Effect B");
        case 19: return QStringLiteral("Unknown 17");
        default: return {};
    }
}

bool KF2::Models::ObjectClassesTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!value.isValid() || value.toString().isEmpty() || role != Qt::EditRole || !index.isValid())
        return false;

    auto& objClass = stat(index.row());
    auto u8Value = Utilities::clampToByte(value.toUInt());
    auto u16Value = Utilities::clampToUShort(value.toUInt());

    switch (index.column())
    {
        case 0: objClass.type = static_cast<KF2::ObjectClassType>(u8Value); break;
        case 1: objClass.transformType = u8Value; break;
        case 2: objClass.unknown0x02 = u8Value; break;
        case 3: objClass.unknown0x03 = u8Value; break;
        case 4: objClass.collisionCylinderPrimaryRadius = u16Value; break;
        case 5: objClass.collisionCylinderSecondaryRadius = u16Value; break;
        case 6: objClass.collisionCylinderHeight = u16Value; break;
        case 7: objClass.unknown0x0a = u16Value; break;
        case 8: objClass.unknown0x0c = u8Value; break;
        case 9: objClass.unknown0x0d = u8Value; break;
        case 10: objClass.unknown0x0e = u8Value; break;
        case 11: objClass.soundEffect0 = static_cast<KF2::SoundID>(u8Value); break;
        case 12: objClass.unknown0x10 = u8Value; break;
        case 13: objClass.unknown0x11 = u8Value; break;
        case 14: objClass.unknown0x12 = u8Value; break;
        case 15: objClass.unknown0x13 = u8Value; break;
        case 16: objClass.unknown0x14 = u8Value; break;
        case 17: objClass.unknown0x15 = u8Value; break;
        case 18: objClass.soundEffect1 = static_cast<KF2::SoundID>(u8Value); break;
        case 19: objClass.unknown0x17 = u8Value; break;
        default: return false;
    }

    emit dataChanged(index, index);
    return true;
}
