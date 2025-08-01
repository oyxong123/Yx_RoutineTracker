#include "mainwindow.h"
#include "routinedialog.h"
#include "routinedialog_add.h"
#include "ui_routinedialog.h"

#include <QPushButton>
#include <QSqlQuery>
#include <QDate>
#include <QMessageBox>
#include <QSqlError>

RoutineDialog::RoutineDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RoutineDialog)
{
    ui->setupUi(this);
    mw = qobject_cast<MainWindow *>(parent);
    if (!mw) {
        QMessageBox::critical(this, "App Error", "Parent is not a MainWindow!");
    }

    QObject::connect(ui->btnHome, &QPushButton::clicked, this, &RoutineDialog::btnHome_clicked);
    QObject::connect(ui->btnAdd, &QPushButton::clicked, this, &RoutineDialog::btnAdd_clicked);
    QObject::connect(ui->btnDel, &QPushButton::clicked, this, &RoutineDialog::btnDel_clicked);
    QObject::connect(ui->btnAdd, &QPushButton::clicked, this, &RoutineDialog::test);

    initialize();

    qDebug() << "Routine Dialog initialized";
}

RoutineDialog::~RoutineDialog()
{
    delete ui;
}

void RoutineDialog::closeEvent(QCloseEvent *event)
{
    mw->initializeFromOutside();
    mw->show();

    QDialog::closeEvent(event);
}

void RoutineDialog::initialize()
{

}

void RoutineDialog::initializeFromOutside()
{
    initialize();
}

void RoutineDialog::btnHome_clicked()
{
    close();
    mw->show();
    mw->initializeFromOutside();
}

void RoutineDialog::btnAdd_clicked()
{
    hide();
    RoutineDialog_Add *rda = new RoutineDialog_Add(this);
    rda->show();
}

void RoutineDialog::btnDel_clicked()
{

}

void RoutineDialog::test()
{
    {
        QSqlQuery query;  // Queries default database when unspecified.
        query.setForwardOnly(true);  // Used during cases where only next() and seek() (forward browsing) is utilized, to improve query speed.
        bool ok = query.exec(
            "SELECT id, name, type, type_param, is_active, priority "
            "FROM routine "
            );
        if (!ok) {
            QMessageBox::critical(this, "Db Error", query.lastError().text());
            return;
        }
        while (query.next()) {
            qint64 id = query.value(0).toLongLong();
            QString name = query.value(1).toString();
            QString type = query.value(2).toString();
            QString type_param = query.value(3).toString();
            bool is_active = query.value(4).toBool();
            int priority = query.value(5).toInt();
        }
    }
}
