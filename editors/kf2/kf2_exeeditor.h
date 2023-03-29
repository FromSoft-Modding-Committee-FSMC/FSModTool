#ifndef KF2_EXEEDITOR_H
#define KF2_EXEEDITOR_H

#include "editors/kfmteditor.h"
#include <QWidget>

namespace Ui
{
class EXEEditor;
}

class EXEEditor : public KFMTEditor
{
    Q_OBJECT
    
public:
    explicit EXEEditor(KFMTFile& file_, QWidget* parent = nullptr);
    ~EXEEditor();

private slots:
    void on_shopCombo_currentIndexChanged(int index);

private:
    Ui::EXEEditor *ui;
};

#endif // KF2_EXEEDITOR_H
