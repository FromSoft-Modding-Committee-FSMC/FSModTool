#include "gamedbeditor.h"
#include "ui_gamedbeditor.h"

KF1::GameDBEditor::GameDBEditor(KFMTFile& file_, QWidget* parent)
    : KFMTEditor(file_, parent), ui(new Ui::GameDBEditor)
{
    ui->setupUi(this);
}
