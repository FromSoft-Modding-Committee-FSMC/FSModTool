#include "models/texturelistmodel.h"
#include "QFileDialog"
#include "texturedbviewer.h"

TextureDBViewer::TextureDBViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextureDBViewer)
{
    ui->setupUi(this);
    ui->clutFbPosLabel->setText(clutFbPosLabelPrefix);
    ui->pixelFbPosLabel->setText(pixelFbPosLabelPrefix);
    ui->pixelModeLabel->setText(pixelModePrefix);
    ui->pixelSizeLabel->setText(pixelSizePrefix);
    ui->exportAllBtn->setVisible(false);
}

void TextureDBViewer::setTextureDB(std::shared_ptr<TextureDB> textureDB)
{
    curTextureDB = textureDB;
    ui->texList->setModel(new TextureListModel(ui->texList, textureDB));
    ui->exportAllBtn->setVisible(curTextureDB->getTextureCount() > 1);
    updateTextureViewer();
}

void TextureDBViewer::on_exportBtn_clicked()
{
    QFileDialog exportDlg(this, "Export texture", QDir::currentPath(),
                          "PNG (*.png);;BMP (*.bmp);;PPM (*.ppm);;XBM (*.xbm);;XPM (*.xpm)");
    
    exportDlg.setAcceptMode(QFileDialog::AcceptSave);
    exportDlg.setFileMode(QFileDialog::ExistingFile);
    
    if (exportDlg.exec() != QDialog::Accepted)
        return;
    
    const QString fileName = exportDlg.selectedFiles().first();
    const std::string format = exportDlg.selectedNameFilter().left(3).toStdString();
    
    curTextureDB->getTexture(curTexture).image.save(fileName, format.c_str(), 0);
}

void TextureDBViewer::on_exportAllBtn_clicked()
{
    auto dir = QFileDialog::getExistingDirectory(this, "Export all textures", QDir::currentPath());
    if (dir.isEmpty()) return;

    for (size_t i = 0; i < curTextureDB->getTextureCount(); i++)
        curTextureDB->getTexture(i).image.save(dir + QDir::separator() + "Texture"
                                                   + QString::number(i) + ".png",
                                               "png");
}

void TextureDBViewer::on_texList_activated(const QModelIndex &index)
{
    curTexture = index.row();
    updateTextureViewer();
}

void TextureDBViewer::on_replaceNBtn_clicked()
{
    replaceTexture(Qt::FastTransformation);
}

void TextureDBViewer::on_replaceSBtn_clicked()
{
    replaceTexture(Qt::SmoothTransformation);
}

void TextureDBViewer::replaceTexture(Qt::TransformationMode mode)
{
    const auto *filter = "Images (*.bmp *.gif *.jpg *.jpeg *.png *.ppm *.xbm *.xpm)";
    auto fileName = QFileDialog::getOpenFileName(this, "Import image", QDir::currentPath(), filter);
    
    if (fileName.isEmpty())
        return;
    
    QImage replacement(fileName);
    curTextureDB->replaceTexture(replacement, curTexture, mode);
    updateTextureViewer();
}

void TextureDBViewer::updateTextureLabel()
{
    if (!curTexPixmap.isNull())
        ui->texWidget->setPixmap(curTexPixmap.scaled(ui->texWidget->size(), Qt::KeepAspectRatio,
                                                     Qt::FastTransformation));
}

void TextureDBViewer::updateTextureViewer()
{
    auto &texture = curTextureDB->getTexture(curTexture);
    curTexPixmap = QPixmap::fromImage(texture.image);
    updateTextureLabel();
    
    ui->clutFbPosLabel->setText(clutFbPosLabelPrefix + QString::asprintf("{%d, %d}",
                                                                         texture.clutVramX,
                                                                         texture.clutVramY));
    ui->pixelFbPosLabel->setText(pixelFbPosLabelPrefix + QString::asprintf("{%d, %d}",
                                                                           texture.pxVramX,
                                                                           texture.pxVramY));
    ui->pixelSizeLabel->setText(pixelSizePrefix + QString::asprintf("%dx%d",
                                                                    texture.pxWidth,
                                                                    texture.pxHeight));
    QString pixelModeString;
    
    switch (texture.pMode)
    {
    case TextureDB::PixelMode::CLUT4Bit:
        pixelModeString = "CLUT 4bpp";
        break;
    case TextureDB::PixelMode::CLUT8Bit:
        pixelModeString = "CLUT 8bpp";
        break;
    case TextureDB::PixelMode::Direct15Bit:
        pixelModeString = "Direct 15bpp";
        break;
    case TextureDB::PixelMode::Direct24Bit:
        pixelModeString = "Direct 24bpp";
        break;
    case TextureDB::PixelMode::Mixed:
        pixelModeString = "Mixed";
        break;
    }
    
    ui->pixelModeLabel->setText(pixelModePrefix + pixelModeString);
}