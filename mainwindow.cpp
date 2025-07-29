#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");  // Use as default connection.
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
    QSqlDatabase db = QSqlDatabase::database();
}
