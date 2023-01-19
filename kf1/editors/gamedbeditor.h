#ifndef KF1_GAMEDBEDITOR_H
#define KF1_GAMEDBEDITOR_H

#include "editors/kfmteditor.h"

namespace Ui {
class GameDBEditor;
}

namespace KF1 {
class GameDBEditor : public KFMTEditor
{
    Q_OBJECT

public:
    explicit GameDBEditor(KFMTFile& file_, QWidget* parent = nullptr);
    ~GameDBEditor() final { delete ui; }

private:
    Ui::GameDBEditor *ui;
};

} // namespace KF1

#endif // KF1_GAMEDBEDITOR_H
