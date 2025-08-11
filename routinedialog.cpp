#include "routinedialog.h"
#include "ui_routinedialog.h"
#include "routinedialog_add.h"
#include "mainwindow.h"

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
    QObject::connect(ui->btnDnd, &QPushButton::clicked, this, &RoutineDialog::btnDnd_clicked);
    QObject::connect(ui->treRoutine, &QTreeWidget::itemChanged, this, &RoutineDialog::treRoutine_activeChecked);

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
    // Reset QTreeWidget.
    ui->treRoutine->clear();
    ui->treRoutine->setSelectionMode(QAbstractItemView::NoSelection);
    ui->treRoutine->setFocusPolicy(Qt::NoFocus);

    // Columns of QTreeWidget use zero-based indexing.
    idxColName = 0;
    idxColTypeParam = 1;
    idxColActive = 2;
    idxColPriority = 3;
    QString headerName = "Routine";
    QString headerTypeParam = "Details";
    QString headerActive = "Active";
    QString headerPriority = "Priority";

    // Create columns ('Priority' column is hidden).
    QStringList headers = {headerName, headerTypeParam, headerActive, headerPriority};
    ui->treRoutine->setHeaderLabels(headers);
    QHeaderView *headerView = ui->treRoutine->header();
    headerView->setSectionResizeMode(idxColName, QHeaderView::Fixed);
    ui->treRoutine->setColumnWidth(idxColName, 200);
    headerView->setSectionResizeMode(idxColTypeParam, QHeaderView::Fixed);
    ui->treRoutine->setColumnWidth(idxColTypeParam, 100);
    headerView->setSectionResizeMode(idxColActive, QHeaderView::Fixed);
    ui->treRoutine->setColumnWidth(idxColActive, 40);
    ui->treRoutine->hideColumn(idxColPriority);

    // Add groups during this step to preserve list order.
    QList<QTreeWidgetItem *> grps;
    QTreeWidgetItem *grpDaily = new QTreeWidgetItem();
    grpDaily->setText(0, "Daily");
    grps.append(grpDaily);
    QTreeWidgetItem *grpWeekly = new QTreeWidgetItem();
    grpWeekly->setText(0, "Weekly");
    grps.append(grpWeekly);
    QTreeWidgetItem *grpBiweekly = new QTreeWidgetItem();
    grpBiweekly->setText(0, "Biweekly");
    grps.append(grpBiweekly);
    QTreeWidgetItem *grpMonthly = new QTreeWidgetItem();
    grpMonthly->setText(0, "Monthly");
    grps.append(grpMonthly);
    QTreeWidgetItem *grpQuarterly= new QTreeWidgetItem();
    grpQuarterly->setText(0, "Quarterly");
    grps.append(grpQuarterly);
    QTreeWidgetItem *grpSemiannually = new QTreeWidgetItem();
    grpSemiannually->setText(0, "Semiannually");
    grps.append(grpSemiannually);
    QTreeWidgetItem *grpAnnually = new QTreeWidgetItem();
    grpAnnually->setText(0, "Annually");
    grps.append(grpAnnually);
    QTreeWidgetItem *grpDay = new QTreeWidgetItem();
    grpDay->setText(0, "Day");
    grps.append(grpDay);
    QTreeWidgetItem *grpInterval = new QTreeWidgetItem();
    grpInterval->setText(0, "Interval");
    grps.append(grpInterval);

    // Retrieve routine records from db, then add them to their respective groups.
    QSqlQuery query;  // Queries default database when unspecified.
    query.setForwardOnly(true);  // Used during cases where only next() and seek() (forward browsing) is utilized, to improve query speed.
    bool ok = query.exec(
        "SELECT id, name, type, type_param, is_active, priority "
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
        QString type_param = query.value(3).toString();
        bool is_active = query.value(4).toBool();
        int priority = query.value(5).toInt();
        qDebug() << "id:" << id << " name:" << name << " type:" << type << " type_param:" << type_param << " is_active:" << is_active << " priority:" << priority;

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setData(0, RoleId, id);
        item->setText(idxColName, name);
        item->setToolTip(idxColName, name);
        item->setText(idxColTypeParam, type_param);
        item->setToolTip(idxColTypeParam, type_param);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);  // Explicitly set for better portability.
        if (is_active) item->setCheckState(idxColActive, Qt::Checked);
        else item->setCheckState(idxColActive, Qt::Unchecked);

        // Qt uses lexicographic order sort on the QStrings. Using this method pads each priority value with 0s, allowing the QStrings to be sorted correctly.
        if (priority > 99999){  // Make assertion to prevent this method from failing. (Max allowed value due to priority number padded to 5 digits is 99999)
            QMessageBox::critical(this, "Computation Error", "Priority exceeded maximum value of 99999");
            return;
        }
        QString lexSortKey =  QString("%1").arg(priority, 5, 10, QChar('0'));  // Pad the priority number to 5 digits with leading zeros (e.g., "00007", "00010", "00100").
        item->setText(idxColPriority, lexSortKey);

        if (type == "Daily") grpDaily->addChild(item);
        else if (type == "Weekly") grpWeekly->addChild(item);
        else if (type == "Biweekly") grpBiweekly->addChild(item);
        else if (type == "Monthly") grpMonthly->addChild(item);
        else if (type == "Quarterly") grpQuarterly->addChild(item);
        else if (type == "Semiannually") grpSemiannually->addChild(item);
        else if (type == "Annually") grpAnnually->addChild(item);
        else if (type == "Day") grpDay->addChild(item);
        else if (type == "Interval") grpInterval->addChild(item);
        else {
            QMessageBox::critical(this, "Db Error", "Unidentified type found");
            return;
        }
    }

    // Remove empty groups during this step to preserve list order.
    if (grpDaily->childCount() == 0) grps.removeAll(grpDaily);
    if (grpWeekly->childCount() == 0) grps.removeAll(grpWeekly);
    if (grpBiweekly->childCount() == 0) grps.removeAll(grpBiweekly);
    if (grpMonthly->childCount() == 0) grps.removeAll(grpMonthly);
    if (grpQuarterly->childCount() == 0) grps.removeAll(grpQuarterly);
    if (grpSemiannually->childCount() == 0) grps.removeAll(grpSemiannually);
    if (grpAnnually->childCount() == 0) grps.removeAll(grpAnnually);
    if (grpDay->childCount() == 0) grps.removeAll(grpDay);
    if (grpInterval->childCount() == 0) grps.removeAll(grpInterval);

    // Insert data into QTreeWidget.
    ui->treRoutine->insertTopLevelItems(0, grps);
    ui->treRoutine->expandAll();

    // Sort routines in each group based on their hidden column 'priority' that was modified for lexicographic order sort (lower number = greater priority = higher in list) (Column at index 3 = Priority).
    grpDaily->sortChildren(idxColPriority, Qt::AscendingOrder);
    grpWeekly->sortChildren(idxColPriority, Qt::AscendingOrder);
    grpBiweekly->sortChildren(idxColPriority, Qt::AscendingOrder);
    grpMonthly->sortChildren(idxColPriority, Qt::AscendingOrder);
    grpQuarterly->sortChildren(idxColPriority, Qt::AscendingOrder);
    grpSemiannually->sortChildren(idxColPriority, Qt::AscendingOrder);
    grpAnnually->sortChildren(idxColPriority, Qt::AscendingOrder);
    grpDay->sortChildren(idxColPriority, Qt::AscendingOrder);
    grpInterval->sortChildren(idxColPriority, Qt::AscendingOrder);
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
    bool isChecked = ui->btnDel->isChecked();
    if (isChecked) {
        ui->btnDnd->setChecked(false);
        ui->treRoutine->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->treRoutine->clearSelection();
    } else {
        ui->btnDel->setChecked(true);
        QList<QTreeWidgetItem*> selectedItems = ui->treRoutine->selectedItems();
        if (selectedItems.size() > 1) {
            QMessageBox::critical(this, "GUI Error", "Invalid selected items count found");
            return;
        } else if (selectedItems.size() == 1) {
            QTreeWidgetItem* item = selectedItems.first();
            int id = item->data(0, RoleId).toInt();
            QMessageBox::StandardButton rc = QMessageBox::question(this,
                                                                       "Confirm",
                                                                       "Are you sure you want to delete this routine?");
            if (rc == QMessageBox::Yes) {
                QSqlQuery query;
                query.prepare("DELETE FROM routine "
                              "WHERE id=:id");
                query.bindValue(":id", id);
                bool ok = query.exec();
                qDebug() << "Executed Query: " << query.executedQuery();
                qDebug() << "Bound Values: " << query.boundValues();
                if (!ok) {
                    QMessageBox::critical(this, "Db Error", query.lastError().text());
                    return;
                }
            } else {
                return;
            }
        }
        ui->treRoutine->clearSelection();
        ui->treRoutine->setSelectionMode(QAbstractItemView::NoSelection);
        ui->btnDel->setChecked(false);
        initialize();
    }
}

void RoutineDialog::btnDnd_clicked()  // not yet implemeted.
{
    bool isChecked = ui->btnDnd->isChecked();
    if (isChecked) {
        ui->btnDel->setChecked(false);
        ui->treRoutine->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->treRoutine->clearSelection();

    } else {
        ui->treRoutine->clearSelection();
        ui->treRoutine->setSelectionMode(QAbstractItemView::NoSelection);
    }
}

void RoutineDialog::treRoutine_activeChecked(QTreeWidgetItem *item, int col)
{
    if (col == idxColActive) {
        int id = item->data(0, RoleId).toInt();
        bool is_active;
        Qt::CheckState state = item->checkState(col);
        if (state == Qt::Unchecked) is_active = 0;
        else is_active = 1;

        QSqlQuery query;
        query.prepare("UPDATE routine "
                      "SET is_active=:is_active "
                      "WHERE id=:id");
        query.bindValue(":is_active", is_active);
        query.bindValue(":id", id);
        bool ok = query.exec();
        qDebug() << "Executed Query: " << query.executedQuery();
        qDebug() << "Bound Values: " << query.boundValues();
        if (!ok) {
            QMessageBox::critical(this, "Db Error", query.lastError().text());
            return;
        }
    }
}
