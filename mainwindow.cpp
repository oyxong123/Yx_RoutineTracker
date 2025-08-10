#include "mainwindow.h"
#include "routinedialog.h"
#include "ui_mainwindow.h"
#include "routinestruct.h"
#include "historydialog_search.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QDate>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTimer>
#include <QCheckBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->btnHistory, &QPushButton::clicked, this, &MainWindow::btnHistory_clicked);
    QObject::connect(ui->btnNextDate, &QPushButton::clicked, this, &MainWindow::btnNextDate_clicked);
    QObject::connect(ui->btnPreviousDate, &QPushButton::clicked, this, &MainWindow::btnPreviousDate_clicked);
    QObject::connect(ui->btnRoutine, &QPushButton::clicked, this, &MainWindow::btnRoutine_clicked);

    // Initialize system tray icon.
    exitAction = trayMenu.addAction("Exit");
    QObject::connect(exitAction, &QAction::triggered, this, &MainWindow::trayExitAction_clicked);
    tray.setIcon(QIcon(":/system/resources/Hackmon16.png"));
    tray.setToolTip("Yx-RoutineTracker");
    tray.setContextMenu(&trayMenu);
    tray.show();

    displayDate = QDate::currentDate();
    ui->lblDate->setText(displayDate.toString(Qt::RFC2822Date));
    ui->lblDay->setText(getDayFromInt(displayDate.dayOfWeek()));

    QTimer::singleShot(0, this, &MainWindow::initialize);  // Need to be defered, otherwise would return 'qt.sql.qsqlquery: QSqlQuery::exec: database not open'.
    show();
}

MainWindow::~MainWindow()
{
    // Close and remove the db connection (structured this way to ensure safe removal of db connection).
    QString connName = QSqlDatabase::database().connectionName();
    {
        QSqlDatabase::database().close();
    }
    QSqlDatabase::removeDatabase(connName);
    delete ui;
    qDebug() << "Application existed successfully";
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (!forceExit)
    {
        hide();
        event->ignore();  // Ignore quit app event.
    }
}

void MainWindow::initialize()
{
    // Reset window to initial height.
    int defaultContHeight = 40;
    int defaultWindowHeight = 110;

    // Reset UI (automatically disconnect signals from deleted widgets).
    QList<QWidget*> children = ui->contRoutines->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (const QWidget* child : children) delete child;

    QList<RoutineGroup*> routineGrps = getRoutineOfDate(displayDate);
    int routineGrpX = 0;
    int routineX = 90;
    int firstY = 0;
    int spacing = 0;
    int grpsSize = routineGrps.size();
    for (int i = 0; i < grpsSize; ++i) {
        const RoutineGroup* routineGrp = routineGrps[i];
        QLabel* lbl = new QLabel(routineGrp->name + " :", ui->contRoutines);
        lbl->move(routineGrpX, firstY + spacing);
        lbl->show();

        int grpSize = routineGrp->content.size();
        for (int j = 0; j < grpSize; ++j) {
            const Routine* routine = routineGrp->content[j];
            QCheckBox *chk = new QCheckBox(routine->name, ui->contRoutines);
            chk->setProperty("routine_id", routine->id);  // Store the routine's id for other usage.
            QObject::connect(chk, &QCheckBox::checkStateChanged, this, &MainWindow::chkRoutine_checked);
            chk->move(routineX, firstY + spacing);
            chk->show();

            if (j < grpSize - 1) spacing += 22;  // Add spacing if not the last routine.
        }

        if (i < grpsSize - 1) spacing += 32;  // Add spacing if not the last routine.
    }

    ui->contRoutines->setFixedHeight(defaultContHeight + spacing);
    this->setFixedHeight(defaultWindowHeight + spacing);
    centerWindow();
    qDebug() << "Main Window initialized";
}

void MainWindow::initializeFromOutside()
{
    initialize();
}

void MainWindow::centerWindow()
{
    QRect screenGeo = QGuiApplication::primaryScreen()->availableGeometry();
    int x = (screenGeo.width() - this->width()) / 2;
    int y = (screenGeo.height() - this->height()) / 2 - 20;  // The additional 20px deducted is to account for the taskbar height at the bottom.
    this->move(x, y);
}

QString MainWindow::getDayFromInt(int n)
{
    if (n == 1) return "Mon";
    else if (n == 2) return "Tue";
    else if (n == 3) return "Wed";
    else if (n == 4) return "Thu";
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
    initialize();
}

void MainWindow::btnPreviousDate_clicked()
{
    displayDate = displayDate.addDays(-1);
    ui->lblDate->setText(displayDate.toString(Qt::RFC2822Date));
    ui->lblDay->setText(getDayFromInt(displayDate.dayOfWeek()));
    initialize();
}

void MainWindow::btnRoutine_clicked()
{
    hide();
    RoutineDialog *rd = new RoutineDialog(this);
    rd->show();
}

void MainWindow::btnHistory_clicked()
{
    hide();
    HistoryDialog_Search *hds = new HistoryDialog_Search(this);
    hds->show();
}

QList<RoutineGroup*> MainWindow::getRoutineOfDate(QDate date)
{
    RoutineGroup *grpDaily = new RoutineGroup("Daily");
    RoutineGroup *grpWeekly = new RoutineGroup("Weekly");
    RoutineGroup *grpBiweekly = new RoutineGroup("Biweekly");
    RoutineGroup *grpMonthly = new RoutineGroup("Monthly");
    RoutineGroup *grpQuarterly = new RoutineGroup("Quarterly");
    RoutineGroup *grpSemiannually = new RoutineGroup("Semiannually");
    RoutineGroup *grpAnnually = new RoutineGroup("Annually");
    RoutineGroup *grpDay = new RoutineGroup("Day");
    RoutineGroup *grpInterval = new RoutineGroup("Interval");
    QList<RoutineGroup*> routineGrps = { grpDaily, grpWeekly, grpBiweekly, grpMonthly, grpQuarterly, grpSemiannually, grpAnnually, grpDay, grpInterval };

    QSqlQuery query;
    query.setForwardOnly(true);
    bool ok = query.exec(
        "SELECT id, name, type, type_param, priority "
        "FROM routine "
        "WHERE is_active = 1;"
        );
    qDebug() << "Executed Query: " << query.executedQuery();
    if (!ok) {
        QMessageBox::critical(this, "Db Error", query.lastError().text());
        return routineGrps;
    }
    qDebug() << "Result Set: ";
    while (query.next()) {
        qint64 id = query.value(0).toLongLong();
        QString name = query.value(1).toString();
        QString type = query.value(2).toString();
        QString type_param = query.value(3).toString();
        int priority = query.value(4).toInt();
        qDebug() << "id:" << id << "  name:" << name << " type:" << type << " type_param:" << type_param << " priority:" << priority;

        Routine *routine = new Routine();
        routine->id = id;
        routine->name = name;
        routine->priority = priority;

        // QDate::dayOfWeek() returns (1 = Mon to 7 = Sun), which means first day is '1'.
        // Algorithms are tweaked to fit concept where first day is '7', which is Sun.
        if (type == "Daily") {
            bool found = hasRecordInRange(date, date);
            if (!found) grpDaily->content.append(routine);
        } else if (type == "Weekly") {
            date = date.addDays(-3);
            int dayOfWeek = date.dayOfWeek();
            int offset = dayOfWeek % 7;  // When dayOfWeek is Mon to Sat (1-6), the resultant value will also be that value, and be used to subtract the routineStartDate. When the dayOfWeek is Sun (7), the resultant value will instead be 0, which supports the logic where the routineStartDate doesn't need to be subtracted if it's already on Sun (the first date of the week).
            QDate sunDate = date.addDays(-offset);  // Offset the supplied date (from argument) to get the first date of the week (Sun).
            QDate satDate = sunDate.addDays(6);  // The last date of the week (Sat) is always 6 days away from the first day of the week (Sun).
            bool found = hasRecordInRange(sunDate, satDate);
            if (!found) grpWeekly->content.append(routine);
        } else if (type == "Biweekly") {
            QDate routineStartDate = QDate::fromString(type_param, "dd MMM yyyy");
            if (!routineStartDate.isValid()) {
                QMessageBox::critical(this, "Computation Error", "Invalid month found");
                return routineGrps;
            }
            int dayOfWeek = routineStartDate.dayOfWeek();
            int offset = dayOfWeek % 7;  // When dayOfWeek is Mon to Sat (1-6), the resultant value will also be that value, and be used to subtract the routineStartDate. When the dayOfWeek is Sun (7), the resultant value will instead be 0, which supports the logic where the routineStartDate doesn't need to be subtracted if it's already on Sun (the first date of the biweek).
            QDate startDateOfBiweeks = routineStartDate.addDays(-offset);  // startDateOfBiweeks = First day of biweek of routine start date (Sun).
            qint64 daysToDate = startDateOfBiweeks.daysTo(date);
            qint64 weeksToStartDate = daysToDate / 14;  // 1 biweek = 14 days. Since the current biweek hasn't reached 14 days, it will not be counted into the biweek interval count between the start date of the first biweek and the start date of the current biweek.
            qint64 daysToStartDate = weeksToStartDate * 14;
            QDate firstDateOfBiweek = startDateOfBiweeks.addDays(daysToStartDate);
            QDate lastDateOfBiweek = startDateOfBiweeks.addDays(daysToStartDate + 13);  // Excluding the start date of biweek of supplied date (from argument), there are only 13 days left to reach the last date of the biweek of the supplied date.
            bool found = hasRecordInRange(firstDateOfBiweek, lastDateOfBiweek);
            if (!found) grpBiweekly->content.append(routine);
        } else if (type == "Monthly") {
            int year = date.year();
            int month = date.month();
            int dayCount = date.daysInMonth();
            QDate firstDateOfMonth = QDate(year, month, 1);  // First day of month is always 1.
            QDate lastDateOfMonth = QDate(year, month, dayCount);  // Last day of month is equivalent to the day count of month.
            bool found = hasRecordInRange(firstDateOfMonth, lastDateOfMonth);
            if (!found) grpMonthly->content.append(routine);
        } else if (type == "Quarterly") {
            int year = date.year();
            int month = date.month();
            QDate firstDateOfQuarter;
            QDate lastDateOfQuarter;
            if (month >= 1 && month <= 3) {
                firstDateOfQuarter = QDate(year, 1, 1);  // First day of first quarter of year is always 1st Jan (1).
                lastDateOfQuarter = QDate(year, 3, 31);  // Last day of first quarter of year is always 31st Mar (3).
            } else if (month >= 4 && month <= 6) {
                firstDateOfQuarter = QDate(year, 4, 1);  // First day of second quarter of year is always 1st Apr (4).
                lastDateOfQuarter = QDate(year, 6, 30);  // Last day of second quarter of year is always 30th Jun (6).
            } else if (month >= 7 && month <= 9) {
                firstDateOfQuarter = QDate(year, 7, 1);  // First day of third quarter of year is always 1st Jul (7).
                lastDateOfQuarter = QDate(year, 9, 30);  // Last day of third quarter of year is always 30th Sep (9).
            } else if (month >= 10 && month <= 12) {
                firstDateOfQuarter = QDate(year, 10, 1);  // First day of fourth quarter of year is always 1st Oct (10).
                lastDateOfQuarter = QDate(year, 12, 31);  // Last day of fourth quarter of year is always 31st Dec (12).
            } else {
                QMessageBox::critical(this, "Computation Error", "Invalid month found");
                return routineGrps;
            }
            if (!firstDateOfQuarter.isValid() || !lastDateOfQuarter.isValid()) {
                QMessageBox::critical(this, "Computation Error", "Null date found");
                return routineGrps;
            }
            bool found = hasRecordInRange(firstDateOfQuarter, lastDateOfQuarter);
            if (!found) grpQuarterly->content.append(routine);
        } else if (type == "Semiannually") {
            int year = date.year();
            int month = date.month();
            QDate firstDateOfSemiannual;
            QDate lastDateOfSemiannual;
            if (month >= 1 && month <= 6) {
                firstDateOfSemiannual = QDate(year, 1, 1);  // First day of first half of year is always 1st Jan (1).
                lastDateOfSemiannual = QDate(year, 6, 30);  // Last day of first half of year is always 30th Jun (6).
            } else if (month >= 7 && month <= 12) {
                firstDateOfSemiannual = QDate(year, 7, 1);  // First day of second half of year is always 1st Jul (7).
                lastDateOfSemiannual = QDate(year, 12, 31);  // Last day of second half of year is always 31st Dec (12).
            } else {
                QMessageBox::critical(this, "Computation Error", "Invalid month found");
                return routineGrps;
            }
            if (!firstDateOfSemiannual.isValid() || !lastDateOfSemiannual.isValid()) {
                QMessageBox::critical(this, "Computation Error", "Null date found");
                return routineGrps;
            }
            bool found = hasRecordInRange(firstDateOfSemiannual, lastDateOfSemiannual);
            if (!found) grpSemiannually->content.append(routine);
        } else if (type == "Annually") {
            int year = date.year();
            QDate firstDateOfYear = QDate(year, 1, 1);  // First day of year is always 1st Jan (1).
            QDate lastDateOfYear = QDate(year, 12, 31);  // Last day of year is always 31st Dec (12).
            bool found = hasRecordInRange(firstDateOfYear, lastDateOfYear);
            if (!found) grpAnnually->content.append(routine);
        } else if (type == "Day") {
            QString month = date.toString("ddd");
            QStringList days = type_param.split(", ");
            if (!days.contains(month)) continue;
            bool found = hasRecordInRange(date, date);
            if (!found) grpDay->content.append(routine);
        } else if (type == "Interval") {
            QStringList temp = type_param.split(", ");
            if (temp.count() != 2) {
                QMessageBox::critical(this, "Computation Error", "Incorrect parameter found");
                return routineGrps;
            }
            int interval = temp[0].toInt();
            if (interval == 0) {
                QMessageBox::critical(this, "Computation Error", "Invalid parameter found");
                return routineGrps;
            }
            QDate routineStartDate = QDate::fromString(temp[1], Qt::RFC2822Date);
            if (!routineStartDate.isValid()) {
                QMessageBox::critical(this, "Computation Error", "Invalid parameter found");
                return routineGrps;
            }
            int delta = routineStartDate.daysTo(date);
            int result = delta % interval;
            if (result != 0) continue;
            bool found = hasRecordInRange(date, date);
            if (!found) grpInterval->content.append(routine);
        } else {
            QMessageBox::critical(this, "Db Error", "Unidentified type found");
            return routineGrps;
        }
    }

    // Remove groups with no routines and sort routines in each routine group based on their priority.
    for (int i=0; i<routineGrps.size();) {
        RoutineGroup* grp = routineGrps[i];
        QList<Routine*>& routines = grp->content;  // '&' must be used to sort the actual data insteda of the copy of the content.
        if (routines.isEmpty()) {
            delete grp;
            routineGrps.removeAt(i);
        } else {
            std::sort(routines.begin(), routines.end(), [](const Routine* a, const Routine* b) {
                return a->priority < b->priority;
            });
            ++i;
        }
    }

    return routineGrps;
}

bool MainWindow::hasRecordInRange(QDate startDate, QDate endDate)
{
    QString startDateStr = startDate.toString(Qt::ISODate);
    QString endDateStr = endDate.toString(Qt::ISODate);
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare(
        "SELECT EXISTS("
        "SELECT 1 "
        "FROM records "
        "WHERE date >= :startDate "
        "AND date <= :endDate);"
        );
    query.bindValue(":startDate", startDateStr);
    query.bindValue(":endDate", endDateStr);
    bool ok = query.exec();
    qDebug() << "Executed Query: " << query.executedQuery();
    qDebug() << "Bound Values: " << query.boundValues();
    if (!ok) {
        QMessageBox::critical(this, "Db Error", query.lastError().text());
    }
    qDebug() << "Result Set: ";
    while (query.next()) {
        int found = query.value(0).toInt();
        qDebug() << found;
    }
    return 0;
}

void MainWindow::tray_clicked(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {  // The icon is clicked.
        show();
        raise();
        activateWindow();
    }
}

void MainWindow::trayExitAction_clicked(bool /*checked*/)
{
    forceExit = true;
    qApp -> quit();
}

void MainWindow::chkRoutine_checked(Qt::CheckState state)
{
    QCheckBox* cb = qobject_cast<QCheckBox*>(sender());  // sender() returns the pointer to the checkbox that emitted the signal.
    if (!cb) {
        QMessageBox::critical(this, "GUI Error", "Checkbox not found");
        return;
    }
    QString date = displayDate.toString(Qt::ISODate);
    int routine_id = cb->property("routine_id").toInt();
    if (routine_id == 0) {  // Either the property is invalid or the converstion to int has failed.
        QMessageBox::critical(this, "Computation Error", "Property invalid or routine_id not found");
        return;
    }

    // Verify whether the checkstate displayed on the GUI has the same state stored in db.
    {
        QSqlQuery query;
        query.setForwardOnly(true);
        query.prepare(
            "SELECT EXISTS("
            "SELECT 1 "
            "FROM records "
            "WHERE date=:date "
            "AND routine_id=:routine_id);"
            );
        query.bindValue(":date", date);
        query.bindValue(":routine_id", routine_id);
        bool ok = query.exec();
        qDebug() << "Executed Query: " << query.executedQuery();
        qDebug() << "Bound Values: " << query.boundValues();
        if (!ok) {
            QMessageBox::critical(this, "Db Error", query.lastError().text());
        }
        qDebug() << "Result Set: ";
        while (query.next()) {
            int found = query.value(0).toInt();
            qDebug() << found;

            if (state == Qt::Unchecked && !found) {  // Invalid if initial checkstate is 'checked', but there's no record in db.
                QMessageBox::critical(this, "Db Sync Error", "Invalid db/checkbox state found");
                return;
            } else if (state != Qt::Unchecked && found) {  // Invalid if initial checkstate is 'unchecked', but there's record in db.
                QMessageBox::critical(this, "Db Sync Error", "Invalid db/checkbox state found");
                return;
            }
        }
    }

    if (state == Qt::Unchecked) {
        QSqlQuery query;
        query.prepare("DELETE from records "
                      "WHERE date=:date "
                      "AND routine_id=:routine_id;");
        query.bindValue(":date", date);
        query.bindValue(":routine_id", routine_id);
        bool ok = query.exec();
        qDebug() << "Executed Query: " << query.executedQuery();
        qDebug() << "Bound Values: " << query.boundValues();
        if (!ok) {
            QMessageBox::critical(this, "Db Error", query.lastError().text());
            return;
        }
    } else {
        QSqlQuery query;
        query.prepare("INSERT INTO records (date, routine_id) "
                      "VALUES (:date, :routine_id)");
        query.bindValue(":date", date);
        query.bindValue(":routine_id", routine_id);
        bool ok = query.exec();
        qDebug() << "Executed Query: " << query.executedQuery();
        qDebug() << "Bound Values: " << query.boundValues();
        if (!ok) {
            QMessageBox::critical(this, "Db Error", query.lastError().text());
            return;
        }
    }
}
