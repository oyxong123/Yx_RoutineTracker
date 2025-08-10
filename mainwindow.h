#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "routinestruct.h"

#include <QMainWindow>
#include <QDate>
#include <QSystemTrayIcon>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSystemTrayIcon tray;
    QMenu trayMenu;
    QAction *exitAction = new QAction;
    void initializeFromOutside();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void btnNextDate_clicked();
    void btnPreviousDate_clicked();
    void btnRoutine_clicked();
    void btnHistory_clicked();
    void tray_clicked(QSystemTrayIcon::ActivationReason reason);
    void trayExitAction_clicked(bool checked);
    void chkRoutine_checked(Qt::CheckState state);

private:
    Ui::MainWindow *ui;
    QDate displayDate;
    QString getDayFromInt(int n);
    QList<RoutineGroup*> getRoutineOfDate(QDate date);
    void initialize();
    void centerWindow();
    bool forceExit = false;
    bool hasRecordInRange(QDate startDate, QDate endDate);
};
#endif // MAINWINDOW_H
