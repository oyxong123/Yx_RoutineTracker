#include "historydialog.h"
#include "ui_historydialog.h"
#include "routinestruct.h"

#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QOperatingSystemVersion>

HistoryDialog::HistoryDialog(const QDate &startDate, const QDate &endDate, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HistoryDialog)
    , startDate(startDate)
    , endDate(endDate)
{
    ui->setupUi(this);
    mw = qobject_cast<MainWindow *>(parent);
    if (!mw) {
        QMessageBox::critical(this, "App Error", "Parent is not a MainWindow!");
    }

    QObject::connect(ui->btnHome, &QPushButton::clicked, this, &HistoryDialog::btnHome_clicked);

    initialize();

    qDebug() << "History Dialog Initialized.";
}

HistoryDialog::~HistoryDialog()
{
    delete ui;
}

void HistoryDialog::closeEvent(QCloseEvent *event)
{
    mw->show();

    QDialog::closeEvent(event);
}

void HistoryDialog::initialize()
{
    insertRoutines();
    insertRecords();

    // Required for adding borders to QTreeWidget (Only on Windows 10).
    if(QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows10 &&
        QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows11){
        setStyleSheet(
            "QHeaderView::section{"  // CSS of headers.
            "font-size: 8pt;"
            "border-top:0px solid #000000;"
            "border-left:0px solid #000000;"
            "border-right:1px solid #000000;"
            "border-bottom:1px solid #000000;"
            "background-color:white;"
            "padding:4px;"
            "}"
            "QTableCornerButton::section{"  // CSS of top left cell.
            "border-top:0px solid #000000;"
            "border-left:0px solid #000000;"
            "border-right:1px solid #000000;"
            "border-bottom:1px solid #000000;"
            "background-color:white;"
            "}"
            "QTableView{"  // CSS of content cells.
            "font-size: 8pt;"
            "}"
        );
    }
}

void HistoryDialog::insertRoutines()
{
    // Reset QTreeWidget.
    ui->tabHistory->clear();
    ui->tabHistory->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tabHistory->setFocusPolicy(Qt::NoFocus);

    // Create groups for each routine type.
    RoutineGroup* grpDaily = new RoutineGroup("Daily");
    RoutineGroup* grpWeekly = new RoutineGroup("Weekly");
    RoutineGroup* grpBiweekly = new RoutineGroup("Biweekly");
    RoutineGroup* grpMonthly = new RoutineGroup("Monthly");
    RoutineGroup* grpQuarterly = new RoutineGroup("Quarterly");
    RoutineGroup* grpSemiannually = new RoutineGroup("Semiannually");
    RoutineGroup* grpAnnually = new RoutineGroup("Annually");
    RoutineGroup* grpDay = new RoutineGroup("Day");
    RoutineGroup* grpInterval = new RoutineGroup("Interval");
    QList<RoutineGroup*> routineGrps = { grpDaily, grpWeekly, grpBiweekly, grpMonthly, grpQuarterly, grpSemiannually, grpAnnually, grpDay, grpInterval };

    // Retrieve routine records from db.
    QSqlQuery query;
    query.setForwardOnly(true);
    bool ok = query.exec(
        "SELECT id, name, type, priority "
        "FROM routine "
        );
    qDebug() << "Executed Query: " << query.executedQuery();
    if (!ok) {
        QMessageBox::critical(this, "Db Error", query.lastError().text());
        return;
    }
    qDebug() << "Result Set: ";
    while (query.next()) {
        qint64 id = query.value(0).toLongLong();
        QString name = query.value(1).toString();
        QString type = query.value(2).toString();
        int priority = query.value(3).toInt();
        qDebug() << "id:" << id << " name:" << name << " type:" << type << " priority:" << priority;

        Routine* item = new Routine();
        item->id = id;
        item->name = name;
        item->priority = priority;

        if (type == "Daily") grpDaily->content.append(item);
        else if (type == "Weekly") grpWeekly->content.append(item);
        else if (type == "Biweekly") grpBiweekly->content.append(item);
        else if (type == "Monthly") grpMonthly->content.append(item);
        else if (type == "Quarterly") grpQuarterly->content.append(item);
        else if (type == "Semiannually") grpSemiannually->content.append(item);
        else if (type == "Annually") grpAnnually->content.append(item);
        else if (type == "Day") grpDay->content.append(item);
        else if (type == "Interval") grpInterval->content.append(item);
        else {
            QMessageBox::critical(this, "Db Error", "Unidentified type found");
            return;
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

    // Set routine types and routines to vertical header.
    int currentRowIdx = 0;
    QStringList vHeaders;
    for (int i=0; i<routineGrps.size(); i++) {
        RoutineGroup* routineGrp = routineGrps[i];
        QString headerSpace = "";
        vHeaders.append(headerSpace);
        ++currentRowIdx;
        QString headerType = routineGrp->name;
        vHeaders.append("~ " + headerType + " ~");
        ++currentRowIdx;

        for (int j=0; j<routineGrp->content.size(); j++) {
            Routine* routine = routineGrp->content[j];
            QString headerRoutine = routine->name;
            QString no = QString::number(j + 1);
            vHeaders.append(no + ": " + headerRoutine);
            routineIdToRowIdx[routine->id] = currentRowIdx;
            ++currentRowIdx;
        }
    }
    int rowHeight = 20;
    ui->tabHistory->verticalHeader()->setDefaultSectionSize(rowHeight);  // Height set is limited by the height of header text.
    ui->tabHistory->setRowCount(vHeaders.count());
    ui->tabHistory->setVerticalHeaderLabels(vHeaders);
}

void HistoryDialog::insertRecords()
{
    // Set dates from selected range to horizontal headers.
    QStringList hHeaders;
    int totalDate = startDate.daysTo(endDate) + 1;
    for (int i=0; i<totalDate; i++) {
        QDate date = startDate.addDays(i);
        QString headerDate = date.toString("d/M");
        hHeaders.append(headerDate);
    }
    int columnWidth = 20;
    ui->tabHistory->horizontalHeader()->setDefaultSectionSize(columnWidth);  // Width set is limited by the width of header text.
    ui->tabHistory->setColumnCount(totalDate);
    ui->tabHistory->setHorizontalHeaderLabels(hHeaders);

    // Place X in corresponding cells based on records from db with dates that are in the selected date range.
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare("SELECT date, routine_id "
                  "FROM records "
                  "WHERE date >= :startDate "
                  "AND date <= :endDate");
    query.bindValue(":startDate", startDate);
    query.bindValue(":endDate", endDate);
    bool ok = query.exec();
    qDebug() << "Executed Query: " << query.executedQuery();
    if (!ok) {
        QMessageBox::critical(this, "Db Error", query.lastError().text());
        return;
    }
    qDebug() << "Result Set: ";
    while (query.next()) {
        QString date = query.value(0).toString();
        qint64 routine_id = query.value(1).toLongLong();
        qDebug() << "date:" << date << " routine_id:" << routine_id;

        if (!routineIdToRowIdx.contains(routine_id)) continue;  // Skip record if its corresponding routine header can't be found.

        QDate dateNow = QDate::fromString(date, Qt::ISODate);
        int columnIdx = startDate.daysTo(dateNow);
        int rowIdx = routineIdToRowIdx[routine_id];  // Get the row of "the routine that holds the id that this record's routine_id corresponds with".
        if (columnIdx < 0 || columnIdx >= ui->tabHistory->columnCount()) {
            QMessageBox::critical(this, "Computation Error", "Invalid index found");
            qDebug() << "Invalid column index:" << columnIdx;
            return;
        }
        if (rowIdx < 0 || rowIdx >= ui->tabHistory->rowCount()) {
            QMessageBox::critical(this, "Computation Error", "Invalid index found");
            qDebug() << "Invalid row index:" << rowIdx;
            return;
        }

        QTableWidgetItem* item = new QTableWidgetItem("X");
        item->setTextAlignment(Qt::AlignCenter);
        ui->tabHistory->setItem(rowIdx, columnIdx, item);
    }
}

void HistoryDialog::btnHome_clicked()
{
    close();
    mw->show();
}
