#ifndef KF2_LEVELCURVETABLEMODEL_H
#define KF2_LEVELCURVETABLEMODEL_H

#include "core/kfmtfile.h"
#include "types/kf2/levelcurveentry.h"
#include "utilities.h"
#include <QAbstractTableModel>

namespace KF2::Models {

class LevelCurveTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    LevelCurveTableModel(KFMTFile& file, QObject* parent = nullptr)
        : QAbstractTableModel(parent), m_file(file)
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
        return 100;
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override
    {
        Q_UNUSED(section) Q_UNUSED(value) Q_UNUSED(role) Q_UNUSED(orientation)
        return false;
    }

private:
    inline const KF2::LevelCurveEntry& stat(uint8_t index) const
    {
        return Utilities::as<KF2::LevelCurveEntry>(m_file.m_data, index * sizeof(KF2::LevelCurveEntry));
    }
    inline KF2::LevelCurveEntry& stat(uint8_t index)
    {
        return Utilities::as<KF2::LevelCurveEntry>(m_file.m_data, index * sizeof(KF2::LevelCurveEntry));
    }
    KFMTFile& m_file;
};

} // namespace KF2::Models

#endif // KF2_LEVELCURVETABLEMODEL_H
