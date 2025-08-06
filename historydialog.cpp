#include "historydialog.h"
#include "ui_historydialog.h"

#include <QPushButton>
#include <QMessageBox>

HistoryDialog::HistoryDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HistoryDialog)
{
    ui->setupUi(this);


    QObject::connect(ui->btnHome, &QPushButton::clicked, this, &HistoryDialog::btnHome_clicked);
}

HistoryDialog::~HistoryDialog()
{
    delete ui;
}

void HistoryDialog::closeEvent(QCloseEvent *event)
{
    mw->initializeFromOutside();
    mw->show();

    QDialog::closeEvent(event);
}

void HistoryDialog::initialize()
{

}

void HistoryDialog::btnHome_clicked()
{
    close();
    mw->show();
    mw->initializeFromOutside();
}
