#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include "mainwindow.h"

#include <QDialog>

namespace Ui {
class HistoryDialog;
}

class HistoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryDialog(QWidget *parent = nullptr);
    ~HistoryDialog();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void btnHome_clicked();

private:
    Ui::HistoryDialog *ui;
    MainWindow *mw;
    void initialize();
};

#endif // HISTORYDIALOG_H
