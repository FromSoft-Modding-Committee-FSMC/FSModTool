#ifndef KF1_LEVELCURVETABLEMODEL_H
#define KF1_LEVELCURVETABLEMODEL_H

#include "types/kf1/levelcurveentry.h"
#include <QAbstractTableModel>

namespace KF1 {

class LevelCurveTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    LevelCurveTableModel(LevelCurveEntry *levelCurve,
                         size_t levelCurveSize,
                         QObject *parent = nullptr)
        : QAbstractTableModel(parent), m_levelCurve(levelCurve), m_levelCurveSize(levelCurveSize)
    {}

    int columnCount(const QModelIndex &parent) const override
    {
        Q_UNUSED(parent)
        return 5;
    }

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if (!index.isValid())
            return Qt::NoItemFlags;
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent)
        return createIndex(row, column);
    }

    int rowCount(const QModelIndex &parent) const override
    {
        Q_UNUSED(parent)
        return static_cast<int>(m_levelCurveSize);
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override
    {
        Q_UNUSED(section) Q_UNUSED(value) Q_UNUSED(role) Q_UNUSED(orientation)
        return false;
    }

private:
    LevelCurveEntry *m_levelCurve;
    size_t m_levelCurveSize;
};

} // namespace KF1

#endif // KF1_LEVELCURVETABLEMODEL_H
