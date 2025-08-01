#include "routinedialog_add.h"
#include "ui_routinedialog_add.h"

#include <QPushButton>
#include <QMessageBox>
#include <QComboBox>
#include <QSqlQuery>
#include <QSqlError>

RoutineDialog_Add::RoutineDialog_Add(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RoutineDialog_Add)
{
    ui->setupUi(this);
    rd = qobject_cast<RoutineDialog *>(parent);
    if (!rd) {
        QMessageBox::critical(this, "App Error", "Parent is not a RoutineDialog!");
    }
    ui->lblTypeParam->hide();
    ui->contTypeParam->hide();

    QObject::connect(ui->cbType, &QComboBox::currentIndexChanged, this, &RoutineDialog_Add::cbType_currentIndexChanged);
    QObject::connect(ui->btnAdd, &QPushButton::clicked, this, &RoutineDialog_Add::btnAdd_clicked);

    qDebug() << "Routine Dialog (Add) initialized";
}

RoutineDialog_Add::~RoutineDialog_Add()
{
    delete ui;
}

void RoutineDialog_Add::closeEvent(QCloseEvent *event)
{
    rd->initializeFromOutside();
    rd->show();

    QDialog::closeEvent(event);
}

void RoutineDialog_Add::cbType_currentIndexChanged(int /*index*/)
{
    QString currentText = ui->cbType->currentText();
    if (currentText == "Day"){
        setFixedHeight(235);
        ui->lblTypeParam->setText("Selected Days :");
        ui->contTypeParam->setCurrentIndex(0);
        ui->lblTypeParam->show();
        ui->contTypeParam->show();
    } else if (currentText == "Biweekly") {
        setFixedHeight(270);
        ui->lblTypeParam->setText("Start Date :");
        ui->contTypeParam->setCurrentIndex(1);
        ui->lblTypeParam->show();
        ui->contTypeParam->show();
    } else if (currentText == "Interval"){
        setFixedHeight(125);
        ui->lblTypeParam->setText("Every N Days :");
        ui->contTypeParam->setCurrentIndex(2);
        ui->lblTypeParam->show();
        ui->contTypeParam->show();
    } else {
        setFixedHeight(95);
        ui->lblTypeParam->hide();
        ui->contTypeParam->hide();
    }
}

void RoutineDialog_Add::btnAdd_clicked()
{
    QString name = ui->inpName->text();
    QString type = ui->cbType->currentText();
    int is_active = 1;

    QString type_param = nullptr;
    if (type == "Day"){
        QString selectedDays;
        if (ui->chkMon->checkState() != Qt::Unchecked) selectedDays += "Mon, ";
        if (ui->chkTues->checkState() != Qt::Unchecked) selectedDays += "Tues, ";
        if (ui->chkWed->checkState() != Qt::Unchecked) selectedDays += "Wed, ";
        if (ui->chkThurs->checkState() != Qt::Unchecked) selectedDays += "Thurs, ";
        if (ui->chkFri->checkState() != Qt::Unchecked) selectedDays += "Fri, ";
        if (ui->chkSat->checkState() != Qt::Unchecked) selectedDays += "Sat, ";
        if (ui->chkSun->checkState() != Qt::Unchecked) selectedDays += "Sun, ";
        if (!selectedDays.isEmpty()) selectedDays.chop(2);
        type_param = selectedDays;
        qDebug() << selectedDays;
        qDebug() << type_param;
    } else if (type == "Biweekly") {
        QString selectedDate = ui->calBiweekly->selectedDate().toString();
        type_param = selectedDate;
    } else if (type == "Interval"){
        QString enteredInterval = ui->sbInterval->text();
        type_param = enteredInterval;
    }

    int priority = 0;
    {
        QSqlQuery query;
        query.prepare(
            "SELECT COUNT(*) "
            "FROM routine "
            "WHERE type = :type;"
            );
        query.bindValue(":type", type);
        query.setForwardOnly(true);
        bool ok = query.exec();
        qDebug() << "Executed Query: " << query.executedQuery();
        qDebug() << "Bound Values: " << query.boundValues();
        if (!ok) {
            QMessageBox::critical(this, "Db Error", query.lastError().text());
            return;
        }
        qDebug() << "Result Set: ";
        while (query.next()) {
            int count = query.value(0).toInt();
            qDebug() << "count:" << count;
            priority = count + 1;
        }
        if (priority == 0) {
            QMessageBox::critical(this, "Computation Error", "Issuing priority will cause problems");
            return;
        }
    }

    {
        QSqlQuery query;
        query.prepare("INSERT INTO routine (name, type, type_param, is_active, priority) "
                      "VALUES (:name, :type, :type_param, :is_active, :priority)");
        query.bindValue(":name", name);
        query.bindValue(":type", type);
        query.bindValue(":type_param", type_param);
        query.bindValue(":is_active", is_active);
        query.bindValue(":priority", priority);
        bool ok = query.exec();
        qDebug() << "Executed Query: " << query.executedQuery();
        qDebug() << "Bound Values: " << query.boundValues();
        if (!ok) {
            QMessageBox::critical(this, "Db Error", query.lastError().text());
            return;
        }
    }

    QMessageBox::information(this, "Success", "Created routine: " + name + "!");
    close();
}
