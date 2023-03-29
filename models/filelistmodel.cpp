#include "filelistmodel.h"
#include "core/icons.h"
#include "core/kfmtcore.h"
#include <QAbstractItemView>
#include <QIcon>
#include <iostream>

QVariant FileListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) return QStringLiteral(u"Files");
    return {};
}

QModelIndex FileListModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) return {};

    if (!parent.isValid()) return createIndex(row, column, core.files[static_cast<size_t>(row)]);

    auto parentFile = reinterpret_cast<KFMTFile*>(parent.internalPointer());

    fsmt_assert(parentFile != nullptr, "FileListModel::index: Parent file not found!");

    return createIndex(row, column, (*parentFile)[static_cast<size_t>(row)]);
}

QModelIndex FileListModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) return {};

    auto file = reinterpret_cast<KFMTFile*>(index.internalPointer());
    fsmt_assert(file != nullptr, "FileListModel::parent: Tried to get parent for null file!");
    fsmt_assert(file != &core.files, "FileListModel::parent: Tried to get parent for root file!");
    
    KFMTFile* parent = file->parent();
    fsmt_assert(parent != nullptr, "FileListModel::parent: File has null parent!");
    // If this file is in the root, there's no parent.
    if (parent == &core.files) return {};
     
    // Otherwise, get grandparent to find row and build index
    KFMTFile* grandparent = parent->parent();
    fsmt_assert(grandparent != nullptr, "FileListModel::parent: File has null grandparent!");
    for (uint32_t c = 0; c < grandparent->childCount(); c++)
        if (parent == (*grandparent)[c]) return createIndex(c, 0, parent);
    
    return {};
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) return core.files.childCount();

    return reinterpret_cast<KFMTFile*>(parent.internalPointer())->childCount();
}

int FileListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    auto* file = reinterpret_cast<KFMTFile*>(index.internalPointer());

    if (!index.isValid()) return {};

    if (role == Qt::DisplayRole)
        return file->prettyName();

    if (role != Qt::DecorationRole) return {};
    
    switch (file->dataType())
    {
        case KFMTFile::DataType::Container:
            switch (file->format())
            {
                case KFMTFile::FileFormat::Folder:
                    return Icons::folder;
                case KFMTFile::FileFormat::MIX:
                    return Icons::container;
                case KFMTFile::FileFormat::Raw:
                    __builtin_unreachable();
                case KFMTFile::FileFormat::T:
                    return Icons::container;
                default:
                    __builtin_unreachable();
            }
        case KFMTFile::DataType::GameDB:
            return Icons::gameDb;
        case KFMTFile::DataType::KF1_ArmourParams: [[fallthrough]];
        case KFMTFile::DataType::KF2_ArmourParams:
            return Icons::armour;
        case KFMTFile::DataType::KF1_LevelCurve: [[fallthrough]];
        case KFMTFile::DataType::KF2_LevelCurve:
            return Icons::levelCurve;
        case KFMTFile::DataType::KF1_MagicParams: [[fallthrough]];
        case KFMTFile::DataType::KF2_MagicParams:
            return Icons::magic;
        case KFMTFile::DataType::KF2_ObjectClasses:
            return Icons::objClass;
        case KFMTFile::DataType::KF1_WeaponParams: [[fallthrough]];
        case KFMTFile::DataType::KF2_WeaponParams:
            return Icons::weapon;
        case KFMTFile::DataType::KF2_GameExec: return Icons::gameExe;
        case KFMTFile::DataType::MapTilemap: [[fallthrough]];
        case KFMTFile::DataType::MapDB: [[fallthrough]];
        case KFMTFile::DataType::MapScript:
            return Icons::map;
        case KFMTFile::DataType::Model:
            return Icons::model;
        case KFMTFile::DataType::SoundBankBody:
            return Icons::soundbankBody;
        case KFMTFile::DataType::KF2_SoundEffectParams: [[fallthrough]];
        case KFMTFile::DataType::SoundBankHeader:
            return Icons::soundbankHeader;
        case KFMTFile::DataType::TextureDB:
            return Icons::textureDb;
        default:
            return Icons::unknown;
    }
    
}

void FileListModel::contextMenu(const QPoint& pos)
{
    // We reset this in case stuff goes wrong.
    contextMenuFile = nullptr;

    auto* view = dynamic_cast<QAbstractItemView*>(QObject::parent());
    auto index = view->indexAt(pos);
    if (!index.isValid()) return;

    contextMenuFile = reinterpret_cast<KFMTFile*>(index.internalPointer());

    if (contextMenuFile->dataType() != KFMTFile::DataType::Container)
        return;

    containerContextMenu->exec(view->viewport()->mapToGlobal(pos));
}
