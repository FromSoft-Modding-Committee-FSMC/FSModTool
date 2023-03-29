#ifndef SIMPLETABLEEDITOR_H
#define SIMPLETABLEEDITOR_H

#include "editors/kfmteditor.h"
#include "ui_simpletableeditor.h"
#include <QAbstractTableModel>
#include <QWidget>

class SimpleTableEditor : public KFMTEditor
{
    Q_OBJECT

public:
    explicit SimpleTableEditor(KFMTFile& file_, QAbstractTableModel* model, QWidget* parent = nullptr)
        : KFMTEditor(file_, parent), m_model(model), ui(new Ui::SimpleTableEditor)
    {
        ui->setupUi(this);
        ui->tableView->setModel(m_model.get());
    }
    ~SimpleTableEditor() {delete ui;}
    
private:
    std::unique_ptr<QAbstractTableModel> m_model;
    Ui::SimpleTableEditor *ui;
};

#endif // SIMPLETABLEEDITOR_H
