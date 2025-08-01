#ifndef ROUTINEDIALOG_H
#define ROUTINEDIALOG_H

#include "mainwindow.h"

#include <QDialog>

namespace Ui {
class RoutineDialog;
}

class RoutineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoutineDialog(QWidget *parent = nullptr);
    ~RoutineDialog();
    void initializeFromOutside();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void btnHome_clicked();
    void btnAdd_clicked();
    void btnDel_clicked();

private:
    Ui::RoutineDialog *ui;
    MainWindow *mw;
    void initialize();
    void test();
};

#endif // ROUTINEDIALOG_H
