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

private slots:

private:
    Ui::HistoryDialog_Search *ui;
    MainWindow *mw;
    void initialize();
};

#endif // HISTORYDIALOG_SEARCH_H
