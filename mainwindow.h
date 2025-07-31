#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>

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

private slots:
    void btnNextDate_clicked();
    void btnPreviousDate_clicked();
    void btnRoutine_clicked();
    void test();

private:
    Ui::MainWindow *ui;
    QDate displayDate;
    QString getDayFromInt(int n);
    QStringList getRoutineOfDate(QDate date);
    void initialize();
};
#endif // MAINWINDOW_H
