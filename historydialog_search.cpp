#include "historydialog_search.h"
#include "ui_historydialog_search.h"
#include "mainwindow.h"

#include <QMessageBox>

HistoryDialog_Search::HistoryDialog_Search(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HistoryDialog_Search)
{
    ui->setupUi(this);
    mw = qobject_cast<MainWindow *>(parent);
    if (!mw) {
        QMessageBox::critical(this, "App Error", "Parent is not a MainWindow!");
    }

    initialize();

    qDebug() << "History Dialog (Search) initialized";
}

HistoryDialog_Search::~HistoryDialog_Search()
{
    delete ui;
}

void HistoryDialog_Search::closeEvent(QCloseEvent *event)
{
    mw->initializeFromOutside();
    mw->show();

    QDialog::closeEvent(event);
}

void HistoryDialog_Search::initialize()
{

}
