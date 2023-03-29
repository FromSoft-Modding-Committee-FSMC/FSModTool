#include "kf2_levelcurvetablemodel.h"
#include "utilities.h"

QVariant KF2::Models::LevelCurveTableModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid()) return {};
    
    const auto& level = stat(index.row());
    
    auto prefix = QStringLiteral(u"");
    if (index.row() > 0)
        prefix = QStringLiteral(u"+");

    switch (index.column())
    {
        case 0: return QString::number(level.maxHP);
        case 1: return QString::number(level.maxMP);
        case 2: return prefix + QString::number(level.physicalIncrement);
        case 3: return prefix + QString::number(level.magicalIncrement);
        case 4: return QString::number(level.expForNext);
        default: return {};
    }
}

QVariant KF2::Models::LevelCurveTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return {};
    
    if (orientation == Qt::Vertical) return QStringLiteral("Level %1").arg(section + 1);

    //if (orientation == Qt::Horizontal):
    switch (section)
    {
        case 0: return QStringLiteral("HP");
        case 1: return QStringLiteral("MP");
        case 2: return QStringLiteral("Str. Increase");
        case 3: return QStringLiteral("Mag. Increase");
        case 4: return QStringLiteral("Exp. to level up");
        default: return {};
    }
}

bool KF2::Models::LevelCurveTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto& level = stat(index.row());

    if (!value.isValid() || value.toString().isEmpty() || role != Qt::EditRole || !index.isValid())
        return false;
    
    uint16_t u16Value = Utilities::clampToUShort(value.toUInt());
    uint32_t u32Value = value.toUInt();

    switch (index.column())
    {
        case 0: level.maxHP = u16Value; break;
        case 1: level.maxMP = u16Value; break;
        case 2: level.physicalIncrement = u16Value; break;
        case 3: level.magicalIncrement = u16Value; break;
        case 4: level.expForNext = u32Value; break;
        default: return false;
    }

    emit dataChanged(index, index);
    return true;
}
