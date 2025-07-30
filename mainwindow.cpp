#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");  // Use as default connection.
    // create yx_routinetracker.db and create schema if it never existed.
    db.setDatabaseName("yx_routinetracker.db");
    bool ok = db.open();
    if (!ok) {
        QMessageBox::critical(this, "Db Error", db.lastError().text());
        return;
    }

    QObject::connect(ui->btnSettings, &QPushButton::clicked, this, &MainWindow::test);

    show();
}

MainWindow::~MainWindow()
{
    // Close and remove the db connection (structured this way to ensure safe removal of db connection).
    QString connName = QSqlDatabase::database().connectionName();
    {
        QSqlDatabase db = QSqlDatabase::database();
        db.close();
    }
    QSqlDatabase::removeDatabase(connName);
    delete ui;
}

void MainWindow::test() {
    {
        QSqlQuery query;  // Queries default database when unspecified.
        query.setForwardOnly(true);  // Used during cases where only next() and seek() (forward browsing) is utilized, to improve query speed.
        bool ok = query.exec(
            "SELECT id, name "
            "FROM routine "
            "WHERE is_active = 1;"
            );
        if (!ok) {
            QMessageBox::critical(this, "Db Error", query.lastError().text());
            return;
        }
        while (query.next()) {
            qint64 id = query.value(0).toLongLong();
            QString name = query.value(1).toString();
            qDebug() << "id: " << id << "  name: " << name;
        }
    }
    {
        QString startDate = QDate::currentDate().addDays(-7).toString(Qt::ISODate);
        QString endDate = QDate::currentDate().toString(Qt::ISODate);
        QSqlQuery query;
        query.setForwardOnly(true);
        query.prepare(
            "SELECT date, routine_id "
            "FROM records "
            "WHERE date BETWEEN :start_date AND :end_date "
            "ORDER BY date ASC");
        query.bindValue(":start_date", startDate);
        query.bindValue(":end_date", endDate);
        bool ok = query.exec();
        if (!ok) {
            QMessageBox::critical(this, "Db Error", query.lastError().text());
            return;
        }
        while (query.next()) {
            QDate date = QDate::fromString(query.value(0).toString(), Qt::ISODate);
            int routine_id = query.value(1).toLongLong();
            qDebug() << "date: " << date << "  routine_id: " << routine_id;
        }
    }
}
