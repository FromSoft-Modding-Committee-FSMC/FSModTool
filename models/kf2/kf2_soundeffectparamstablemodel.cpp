#include "kf2_soundeffectparamstablemodel.h"
#include "utilities.h"

QVariant KF2::Models::SoundEffectParamsTableModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid()) return {};
    
    const auto& params = stat(index.row());
    
    switch (index.column())
    {
        case 0: return QString::number(params.vabID);
        case 1: return QString::number(params.programIndex);
        case 2: return QString::number(params.toneIndex);
        case 3: return QString::number(params.pitch);
        case 4: return QString::number(params.unknown0x08);
        default: return {};
    }
}

QVariant KF2::Models::SoundEffectParamsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return {};
    
    if (orientation == Qt::Vertical)
        return valueAndName(section, KF2::getSoundName(static_cast<KF2::SoundID>(section)));

    //if (orientation == Qt::Horizontal):
    switch (section)
    {
        case 0: return QStringLiteral("VAB ID");
        case 1: return QStringLiteral("Program");
        case 2: return QStringLiteral("Tone");
        case 3: return QStringLiteral("Pitch");
        case 4: return QStringLiteral("Unknown 08");
        default: return {};
    }
}

bool KF2::Models::SoundEffectParamsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto& params = stat(index.row());

    if (!value.isValid() || value.toString().isEmpty() || role != Qt::EditRole || !index.isValid())
        return false;
    
    int16_t s16Value = Utilities::clampToShort(value.toInt());

    switch (index.column())
    {
        case 0: params.vabID = s16Value; break;
        case 1: params.programIndex = s16Value; break;
        case 2: params.toneIndex = s16Value; break;
        case 3: params.pitch = s16Value; break;
        case 4: params.unknown0x08 = s16Value; break;
        default: return false;
    }

    emit dataChanged(index, index);
    return true;
}
