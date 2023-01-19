#include "filelistmodel.h"
#include "core/icons.h"
#include <QAbstractItemView>
#include <QIcon>
#include <iostream>

QVariant FileListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) return QStringLiteral("Files");
    return {};
}

QModelIndex FileListModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) return {};

    if (!parent.isValid()) return createIndex(row, column, &core.files[static_cast<size_t>(row)]);

    return createIndex(row,
                       column,
                       &reinterpret_cast<KFMTFile*>(parent.internalPointer())
                            ->m_subFiles[static_cast<size_t>(row)]);
}

QModelIndex FileListModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) return {};

    auto* indexFile = reinterpret_cast<KFMTFile*>(index.internalPointer());

    size_t fileNo = 0;

    for (auto& f : core.files)
    {
        // If the file is not a subfile of anything, there is no parent
        if (f.filePath() == indexFile->filePath()) return {};
        // But if it is a subfile, we return the proper parent.
        if (indexFile->filePath().startsWith(QString(f.filePath().data()) + ':'))
            return createIndex(fileNo, 0, &f);
        fileNo++;
    }

    return {};
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) return core.files.size();

    return reinterpret_cast<KFMTFile*>(parent.internalPointer())->m_subFiles.size();
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

    if (role == Qt::DisplayRole) return QString(file->fileName().data());

    if (role != Qt::DecorationRole) return {};

    switch (file->fileType())
    {
        case KFMTFile::FileType::Folder:
            return Icons::folder;
        case KFMTFile::FileType::MIX:
            return Icons::container;
        case KFMTFile::FileType::Raw:
            switch (file->dataType())
            {
                case KFMTFile::DataType::GameDB:
                    return Icons::gameDb;
                case KFMTFile::DataType::GameEXE:
                    return Icons::gameExe;
                case KFMTFile::DataType::MapTilemap:
                    [[fallthrough]];
                case KFMTFile::DataType::MapDB:
                    [[fallthrough]];
                case KFMTFile::DataType::MapScript:
                    return Icons::map;
                case KFMTFile::DataType::Model:
                    return Icons::model;
                case KFMTFile::DataType::SoundBankBody:
                    return Icons::soundbankBody;
                case KFMTFile::DataType::SoundBankHeader:
                    return Icons::soundbankHeader;
                case KFMTFile::DataType::TextureDB:
                    return Icons::textureDb;
                default:
                    return Icons::unknown;
            }
        case KFMTFile::FileType::T:
            return Icons::container;
        default:
            __builtin_unreachable();
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
