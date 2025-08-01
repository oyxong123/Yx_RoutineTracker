#include "historydialog_search.h"
#include "ui_historydialog_search.h"

HistoryDialog_Search::HistoryDialog_Search(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HistoryDialog_Search)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

HistoryDialog_Search::~HistoryDialog_Search()
{
    delete ui;
}
