#include "levelcurvetablemodel.h"
#include "utilities.h"

QVariant KF1::LevelCurveTableModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid()) return {};

    switch (index.column()) {
    case 0:
        return QString::number(m_levelCurve[index.row()].m_hpAtLevel);
    case 1:
        return QString::number(m_levelCurve[index.row()].m_mpAtLevel);
    case 2:
        return QString::number(m_levelCurve[index.row()].m_physicalGain);
    case 3:
        return QString::number(m_levelCurve[index.row()].m_magicalGain);
    case 4:
        return QString::number(m_levelCurve[index.row()].m_experienceToNext);
    default:
        return {};
    }
}

QVariant KF1::LevelCurveTableModel::headerData(int section,
                                               Qt::Orientation orientation,
                                               int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Vertical)
        return QStringLiteral("Level ") + QString::number(section);

    switch (section) {
    case 0:
        return QStringLiteral("HP");
    case 1:
        return QStringLiteral("MP");
    case 2:
        return QStringLiteral("Strength Power Increase");
    case 3:
        return QStringLiteral("Magic Power Increase");
    case 4:
        return QStringLiteral("Exp. for the next level");
    default:
        return {};
    }
}

bool KF1::LevelCurveTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!value.isValid() || value.toString().isEmpty() || role != Qt::EditRole || !index.isValid())
        return false;

    switch (index.column()) {
    case 0:
        m_levelCurve[index.row()].m_hpAtLevel = Utilities::clampToUShort(value.toUInt());
        break;
    case 1:
        m_levelCurve[index.row()].m_mpAtLevel = Utilities::clampToUShort(value.toUInt());
        break;
    case 2:
        m_levelCurve[index.row()].m_physicalGain = Utilities::clampToUShort(value.toUInt());
        break;
    case 3:
        m_levelCurve[index.row()].m_magicalGain = Utilities::clampToUShort(value.toUInt());
        break;
    case 4:
        m_levelCurve[index.row()].m_experienceToNext = std::min(65535u, value.toUInt());
        break;
    default:
        return false;
    }

    emit dataChanged(index, index);
    return true;
}
