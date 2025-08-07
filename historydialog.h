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
    explicit HistoryDialog(const QDate &startDate, const QDate &endDate, QWidget *parent = nullptr);
    ~HistoryDialog();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void btnHome_clicked();

private:
    Ui::HistoryDialog *ui;
    MainWindow *mw;
    QDate startDate;
    QDate endDate;
    QMap<qint64, int> routineIdToRowIdx;
    void initialize();
    void insertRoutines();
    void insertRecords();
};

#endif // HISTORYDIALOG_H
