#include "mainwindow.h"
#include "windowtracker.h"

#include <QApplication>
#include <QFile>
#include <QProcess>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>

class WindowEventFilter : public QObject {
protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        QWidget *w = qobject_cast<QWidget*>(obj);
        if (w && w->isWindow()) {
            if (event->type() == QEvent::Show || event->type() == QEvent::WindowActivate) {
                WindowTracker::setCurrent(w);
            }
        }
        return QObject::eventFilter(obj, event);
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mw;

    WindowEventFilter *filter = new WindowEventFilter;
    a.installEventFilter(filter);

    QString dbFile = "yx_routinetracker.db";
    QString schemaFile = "schema.sql";
    QString sqlitePath = "sqlite3.exe";

    // create yx_routinetracker.db and create schema if it does not exist.
    if (!QFile::exists(dbFile)) {
        if (!QFile::exists(schemaFile)) {
            QMessageBox::critical(&mw, "Db Error", "Error: schema.sql not found while creating database.");
            return 1;
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
        QMessageBox::critical(&mw, "Db Error", db.lastError().text());
        return 1;
    }

    return a.exec();
}
