#ifndef ROUTINEDIALOG_H
#define ROUTINEDIALOG_H

#include "mainwindow.h"

#include <QDialog>
#include <QTreeWidgetItem>

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
    void btnDnd_clicked();
    void treRoutine_activeChecked(QTreeWidgetItem *item, int col);

private:
    Ui::RoutineDialog *ui;
    MainWindow *mw;
    void initialize();
    int idxColName;
    int idxColTypeParam;
    int idxColActive;
    int idxColPriority;
    int RoleId = Qt::UserRole;
};

#endif // ROUTINEDIALOG_H
