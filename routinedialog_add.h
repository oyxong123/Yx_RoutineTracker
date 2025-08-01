#ifndef ROUTINEDIALOG_ADD_H
#define ROUTINEDIALOG_ADD_H

#include "routinedialog.h"

#include <QDialog>

namespace Ui {
class RoutineDialog_Add;
}

class RoutineDialog_Add : public QDialog
{
    Q_OBJECT

public:
    explicit RoutineDialog_Add(QWidget *parent = nullptr);
    ~RoutineDialog_Add();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void cbType_currentIndexChanged(int index);
    void btnAdd_clicked();

private:
    Ui::RoutineDialog_Add *ui;
    RoutineDialog *rd;
};

#endif // ROUTINEDIALOG_ADD_H
