#ifndef KF2_SOUNDEFFECTPARAMSTABLEMODEL_H
#define KF2_SOUNDEFFECTPARAMSTABLEMODEL_H

#include "core/kfmtfile.h"
#include "types/kf2/sfx.h"
#include "utilities.h"
#include <QAbstractTableModel>

namespace KF2::Models {

class SoundEffectParamsTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    SoundEffectParamsTableModel(KFMTFile& file, QObject* parent = nullptr)
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
        return 256;
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override
    {
        Q_UNUSED(section) Q_UNUSED(value) Q_UNUSED(role) Q_UNUSED(orientation)
        return false;
    }

private:
    inline const KF2::SoundEffectParams& stat(uint8_t index) const
    {
        return Utilities::as<KF2::SoundEffectParams>(m_file.m_data, index * sizeof(KF2::SoundEffectParams));
    }
    inline KF2::SoundEffectParams& stat(uint8_t index)
    {
        return Utilities::as<KF2::SoundEffectParams>(m_file.m_data, index * sizeof(KF2::SoundEffectParams));
    }
    KFMTFile& m_file;
};

} // namespace KF2::Models

#endif // KF2_LEVELCURVETABLEMODEL_H
