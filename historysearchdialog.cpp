#include "historysearchdialog.h"
#include "ui_historysearchdialog.h"

HistorySearchDialog::HistorySearchDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HistorySearchDialog)
{
    ui->setupUi(this);
}

HistorySearchDialog::~HistorySearchDialog()
{
    delete ui;
}
