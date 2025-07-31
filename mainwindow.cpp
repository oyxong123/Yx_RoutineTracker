#include "mainwindow.h"
#include "routinedialog.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QDate>
#include <QFile>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString dbFile = "yx_routinetracker.db";
    QString schemaFile = "schema.sql";
    QString sqlitePath = "sqlite3.exe";

    // create yx_routinetracker.db and create schema if it does not exist.
    if (!QFile::exists(dbFile)) {
        if (!QFile::exists(schemaFile)) {
            QMessageBox::critical(this, "Db Error", "Error: schema.sql not found while creating database.");
            return;
        }

        QStringList arguments;
        arguments << dbFile << QString(".read %1").arg(schemaFile);
        QProcess process;
        process.start(sqlitePath, arguments);
        process.waitForFinished();

        qDebug() << "=== SQLite3 Database Creation ===";
        qDebug() << "Attempted to create database:" << dbFile;
        qDebug() << "Using schema file:" << schemaFile;
        qDebug() << "SQLite3 process path:" << sqlitePath;
        qDebug() << "--- Process Output ---";
        qDebug() << "stdout:" << process.readAllStandardOutput();
        qDebug() << "stderr:" << process.readAllStandardError();
        qDebug() << "===============================";
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");  // Use as default connection.
    db.setDatabaseName(dbFile);
    bool ok = db.open();
    if (!ok) {
        QMessageBox::critical(this, "Db Error", db.lastError().text());
        return;
    }

    QObject::connect(ui->btnHistory, &QPushButton::clicked, this, &MainWindow::test);
    QObject::connect(ui->btnNextDate, &QPushButton::clicked, this, &MainWindow::btnNextDate_clicked);
    QObject::connect(ui->btnPreviousDate, &QPushButton::clicked, this, &MainWindow::btnPreviousDate_clicked);
    QObject::connect(ui->btnRoutine, &QPushButton::clicked, this, &MainWindow::btnRoutine_clicked);

    initialize();

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

void MainWindow::initialize()
{
    displayDate = QDate::currentDate();
    ui->lblDate->setText(displayDate.toString(Qt::RFC2822Date));
    ui->lblDay->setText(getDayFromInt(displayDate.dayOfWeek()));
    qDebug() << "Main Window initialized";
}

QString MainWindow::getDayFromInt(int n)
{
    if (n == 1) return "Mon";
    else if (n == 2) return "Tues";
    else if (n == 3) return "Wed";
    else if (n == 4) return "Thurs";
    else if (n == 5) return "Fri";
    else if (n == 6) return "Sat";
    else if (n == 7) return "Sun";
    else {
        QMessageBox::critical(this, "Computation Error", "Day Conversion Error. n = " + QString::number(n) + ".");
        return "?";
    }
}

void MainWindow::btnNextDate_clicked()
{
    displayDate = displayDate.addDays(1);
    ui->lblDate->setText(displayDate.toString(Qt::RFC2822Date));
    ui->lblDay->setText(getDayFromInt(displayDate.dayOfWeek()));
}

void MainWindow::btnPreviousDate_clicked()
{
    displayDate = displayDate.addDays(-1);
    ui->lblDate->setText(displayDate.toString(Qt::RFC2822Date));
    ui->lblDay->setText(getDayFromInt(displayDate.dayOfWeek()));
}

void MainWindow::btnRoutine_clicked()
{
    hide();
    RoutineDialog *rd = new RoutineDialog();
    rd->exec();
    initialize();
    show();
}

QStringList MainWindow::getRoutineOfDate(QDate date)
{
    QStringList routineList;
    {
        QSqlQuery query;
        query.setForwardOnly(true);
        bool ok = query.exec(
            "SELECT id, name, type, type_param, priority "
            "FROM routine "
            "WHERE is_active = 1;"
            );
        if (!ok) {
            QMessageBox::critical(this, "Db Error", query.lastError().text());
            return routineList;
        }
        while (query.next()) {
            qint64 id = query.value(0).toLongLong();
            QString name = query.value(1).toString();
            qDebug() << "id: " << id << "  name: " << name;
        }
    }
    {
        QSqlQuery query;
        query.setForwardOnly(true);
        bool ok = query.exec(
            "SELECT id, name, type, type_param, priority "
            "FROM routine "
            "WHERE date = :date;"
            );
        if (!ok) {
            QMessageBox::critical(this, "Db Error", query.lastError().text());
            return routineList;
        }
        while (query.next()) {
            qint64 id = query.value(0).toLongLong();
            QString name = query.value(1).toString();
            qDebug() << "id: " << id << "  name: " << name;
        }
    }
    return routineList;
}

void MainWindow::test()
{
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
