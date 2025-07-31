#ifndef ROUTINEDIALOG_H
#define ROUTINEDIALOG_H

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

private slots:
    void btnHome_clicked();

private:
    Ui::RoutineDialog *ui;
};

#endif // ROUTINEDIALOG_H
