#include "mainwindow.h"
#include "core/icons.h"
#include "editors/simpletableeditor.h"
#include "editors/mapeditwidget.h"
#include "editors/modelviewerwidget.h"
#include "editors/texturedbviewer.h"
#include "models/kf2/kf2_armourparamstablemodel.h"
#include "models/kf2/kf2_levelcurvetablemodel.h"
#include "models/kf2/kf2_magicparamstablemodel.h"
#include "models/kf2/kf2_objectclassestablemodel.h"
#include "models/kf2/kf2_weaponparamstablemodel.h"
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
        // TODO: Add KF1 DB stuff here
            
        // KF2 specific
        //case KFMTFile::DataType::GameEXE:
        //    editor = new EXEEditor(*file, ui->editorTabs);
        //    icon = &Icons::gameExe;
        //    break;
        
        case KFMTFile::DataType::KF2_ArmourParams:
            editor = new SimpleTableEditor(*file,
                                           new KF2::Models::ArmourParamsTableModel(*file),
                                           this);
            icon = &Icons::armour;
            break;
        case KFMTFile::DataType::KF2_LevelCurve:
            editor = new SimpleTableEditor(*file,
                                           new KF2::Models::LevelCurveTableModel(*file),
                                           this);
            icon = &Icons::levelCurve;
            break;
        case KFMTFile::DataType::KF2_MagicParams:
            editor = new SimpleTableEditor(*file,
                                           new KF2::Models::MagicParamsTableModel(*file),
                                           this);
            icon = &Icons::magic;
            break;
        case KFMTFile::DataType::KF2_ObjectClasses:
            editor = new SimpleTableEditor(*file,
                                           new KF2::Models::ObjectClassesTableModel(*file),
                                           this);
            icon = &Icons::objClass;
            break;
        case KFMTFile::DataType::KF2_WeaponParams:
            editor = new SimpleTableEditor(*file,
                                           new KF2::Models::WeaponParamsTableModel(*file),
                                           this);
            icon = &Icons::weapon;
            break;
        case KFMTFile::DataType::MapTilemap:
            [[fallthrough]];
        case KFMTFile::DataType::MapDB:
            [[fallthrough]];
        case KFMTFile::DataType::MapScript: {
            KFMTError::fatalError(QStringLiteral(u"Oopsies poopsies I made a deletey!"));
            break;
        }
        case KFMTFile::DataType::Model:
            editor = new ModelViewerWidget(*file, ui->editorTabs);
            icon = &Icons::model;
            break;
        case KFMTFile::DataType::TextureDB:
            editor = new TextureDBViewer(*file, ui->editorTabs);
            icon = &Icons::textureDb;
            break;
        default:
            break;
    }

    if (editor == nullptr)
        return;

    openTabs.emplace(file, editor);
    ui->editorTabs->addTab(editor, *icon, tabTitle);
    ui->editorTabs->setCurrentWidget(editor);
}
