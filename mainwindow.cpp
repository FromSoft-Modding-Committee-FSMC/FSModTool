#include "mainwindow.h"
#include "core/icons.h"
#include "editors/exeeditor.h"
#include "editors/gamedbeditwidget.h"
#include "editors/mapeditwidget.h"
#include "editors/modelviewerwidget.h"
#include "editors/texturedbviewer.h"
#include "kf1/editors/gamedbeditor.h"
#include <QFileDialog>
#include <iostream>
#include <memory>

void MainWindow::on_actionLoad_files_triggered()
{
    auto directory = QFileDialog::getExistingDirectory(
        this, QStringLiteral("Select the root folder of your game's CD."), QDir::homePath());

    if (directory.isEmpty()) return;

    // Close all tabs
    for (int tab = ui->editorTabs->count() - 1; tab >= 0; tab--) ui->editorTabs->removeTab(tab);

    core.loadFrom(directory);

    dynamic_cast<FileListModel*>(ui->filesTree->model())->update();
}

void MainWindow::on_actionSave_changes_triggered()
{
    auto dirPath = QFileDialog::getExistingDirectory(this,
                                                     QStringLiteral(
                                                         "Select where to save the changed files"),
                                                     QDir::homePath());

    if (dirPath.isEmpty()) return;

    if (QDir(dirPath) == core.getSrcDir())
    {
        auto answer = QMessageBox::question(this,
                                            QStringLiteral("You're about to overwrite your files!"),
                                            QStringLiteral(
                                                "You just chose the same directory as your source "
                                                "files. Are you sure you want to overwrite them?"));

        if (answer != QMessageBox::Yes) return;
    }

    core.saveTo(dirPath);

    QMessageBox::information(this,
                             QStringLiteral("Changes saved successfully!"),
                             QStringLiteral("Your changes have been saved!"));
}

void MainWindow::on_filesTree_doubleClicked(const QModelIndex& index)
{
    auto* file = reinterpret_cast<KFMTFile*>(index.internalPointer());

    if (file->dataType() == KFMTFile::DataType::Container) return;
    if (openTabs.find(file) != openTabs.end())
    {
        ui->editorTabs->setCurrentWidget(openTabs.find(file)->second);
        return;
    }

    KFMTEditor* editor = nullptr;
    QIcon* icon = &Icons::unknown;
    auto tabTitle = index.data().value<QString>();

    switch (file->dataType())
    {
        case KFMTFile::DataType::GameDB:
            switch (core.currentGame())
            {
                case KFMTCore::SimpleGame::KF1:
                    editor = new KF1::GameDBEditor(*file, ui->editorTabs);
                    break;
                case KFMTCore::SimpleGame::KF2:
                    editor = new GameDBEditWidget(*file, ui->editorTabs);
                    break;
                default:
                    break;
            }
            icon = &Icons::gameDb;
            break;
        case KFMTFile::DataType::GameEXE:
            editor = new EXEEditor(*file, ui->editorTabs);
            icon = &Icons::gameExe;
            break;
        case KFMTFile::DataType::MapTilemap:
            [[fallthrough]];
        case KFMTFile::DataType::MapDB:
            [[fallthrough]];
        case KFMTFile::DataType::MapScript: {
            // FIXME: This way of getting the DB/script files is *still* terrible. There must be a better way.
            const auto fileIndex = file->indexInContainer();
            const auto containerName = file->fileName().left(file->fileName().size()
                                                             - QString::number(fileIndex).size());
            size_t tilemapIndex;
            size_t dbIndex;
            size_t scriptIndex;
            switch (file->dataType())
            {
                case KFMTFile::DataType::MapTilemap:
                    tilemapIndex = fileIndex;
                    dbIndex = fileIndex + 1;
                    scriptIndex = fileIndex + 2;
                    break;
                case KFMTFile::DataType::MapDB:
                    tilemapIndex = fileIndex - 1;
                    dbIndex = fileIndex;
                    scriptIndex = fileIndex + 1;
                    break;
                case KFMTFile::DataType::MapScript:
                    tilemapIndex = fileIndex - 2;
                    dbIndex = fileIndex - 1;
                    scriptIndex = fileIndex;
                    break;
                default:
                    KFMTError::fatalError(
                        "MainWindow::on_filesTree_doubleClicked: This should be unreachable...");
            }

            editor = new MapEditWidget(*core.getFile(containerName, tilemapIndex),
                                       *core.getFile(containerName, dbIndex),
                                       *core.getFile(containerName, scriptIndex),
                                       ui->editorTabs);
            icon = &Icons::map;
            tabTitle = tabTitle.left(tabTitle.lastIndexOf(' '));
            // The following needs to be done since there's a space in 'Map DB'.
            // If we don't do this, we end up creating a tab name like "Western Shore Map".
            if (file->dataType() == KFMTFile::DataType::MapDB)
                tabTitle = tabTitle.left(tabTitle.lastIndexOf(' '));

            // This is done so that we don't create multiple tabs for the same map
            // Since the tabs are organized by file pointer, we use the tilemap file as our canonical
            // "index".
            file = core.getFile(containerName, tilemapIndex);
            break;
        }
        case KFMTFile::DataType::Model:
            editor = new ModelViewerWidget(*file, ui->editorTabs);
            icon = &Icons::model;
            break;
        case KFMTFile::DataType::TextureDB:
            editor = new TextureDBViewer(*file, ui->editorTabs);
            icon = &Icons::textureDb;
        default:
            break;
    }

    if (editor == nullptr)
        return;

    openTabs.emplace(file, editor);
    ui->editorTabs->addTab(editor, *icon, tabTitle);
    ui->editorTabs->setCurrentWidget(editor);
}

void MainWindow::on_filesTree_clicked(const QModelIndex& index)
{
    auto* file = reinterpret_cast<KFMTFile*>(index.internalPointer());

    std::cerr << "CLICK!!!\n";
    std::cerr << 'r' << index.row() << " c" << index.column() << '\n';
    std::cerr << "parent? " << (index.parent().isValid() ? 'y' : 'n') << '\n';
    if (index.parent().isValid())
        std::cerr << "        r" << index.parent().row() << " c" << index.parent().column() << '\n';
    std::cerr << "FILE INFO:\n";
    std::cerr << "m_path: " << file->fileName().toString().toStdString() << '\n';
}
