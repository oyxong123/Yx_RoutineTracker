#ifndef HISTORYDIALOG_SEARCH_H
#define HISTORYDIALOG_SEARCH_H

#include "mainwindow.h"

#include <QDialog>

namespace Ui {
class HistoryDialog_Search;
}

class HistoryDialog_Search : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryDialog_Search(QWidget *parent = nullptr);
    ~HistoryDialog_Search();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void btnSearch_clicked();
    void chkRange_clicked();
    void de_dateChanged();
    void cbPreset_changed();

private:
    Ui::HistoryDialog_Search *ui;
    MainWindow *mw;
    bool isCustom = false;
    bool isDeleteFromCloseButton = true;
};

#endif // HISTORYDIALOG_SEARCH_H
