#include "historydialog_search.h"
#include "ui_historydialog_search.h"
#include "mainwindow.h"
#include "historydialog.h"

#include <QMessageBox>
#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>

HistoryDialog_Search::HistoryDialog_Search(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HistoryDialog_Search)
{
    ui->setupUi(this);
    mw = qobject_cast<MainWindow *>(parent);
    if (!mw) {
        QMessageBox::critical(this, "App Error", "Parent is not a MainWindow!");
    }

    // Need to be set before slots are assigned to &QDateTimeEdit::dateChanged signal.
    QDate currentDate = QDate::currentDate();
    ui->deStartDate->setDate(currentDate);
    ui->deEndDate->setDate(currentDate);
    ui->deStartDate->setMaximumDate(currentDate);
    ui->deEndDate->setMaximumDate(currentDate);

    QObject::connect(ui->btnSearch, &QPushButton::clicked, this, &HistoryDialog_Search::btnSearch_clicked);
    QObject::connect(ui->chkRange, &QCheckBox::checkStateChanged, this, &HistoryDialog_Search::chkRange_clicked);
    QObject::connect(ui->deStartDate, &QDateTimeEdit::dateChanged, this, &HistoryDialog_Search::de_dateChanged);
    QObject::connect(ui->deEndDate, &QDateTimeEdit::dateChanged, this, &HistoryDialog_Search::de_dateChanged);
    QObject::connect(ui->cbPreset, &QComboBox::currentIndexChanged, this, &HistoryDialog_Search::cbPreset_changed);

    qDebug() << "History Dialog (Search) initialized";
}

HistoryDialog_Search::~HistoryDialog_Search()
{
    delete ui;
}

void HistoryDialog_Search::closeEvent(QCloseEvent *event)
{
    if (isDeleteFromCloseButton) {
        mw->initializeFromOutside();
        mw->show();
    }

    QDialog::closeEvent(event);
}

void HistoryDialog_Search::btnSearch_clicked()
{
    QDate startDate = ui->deStartDate->date();
    QDate endDate = ui->deEndDate->date();

    isDeleteFromCloseButton = false;
    close();
    HistoryDialog *hd = new HistoryDialog(startDate, endDate, mw);
    hd->show();
}

void HistoryDialog_Search::chkRange_clicked()
{
    Qt::CheckState state = ui->chkRange->checkState();
    if (state != Qt::Unchecked) {
        ui->deEndDate->setEnabled(true);
    } else {
        ui->deEndDate->setDate(QDate::currentDate());
        ui->deEndDate->setEnabled(false);
    }
}

void HistoryDialog_Search::de_dateChanged()
{
    int indexCustom = ui->cbPreset->findText("Custom");
    if (indexCustom == -1) {
        QString newItem = "Custom";
        ui->cbPreset->addItem(newItem);
        ui->cbPreset->setCurrentText(newItem);  // This line has huge risk of triggering mutual recursion, which causes serious failure in program behavior.
        isCustom = true;
    }
}

void HistoryDialog_Search::cbPreset_changed()
{
    // Prevent this function from accidentally calling other functions (through released signals) when setting values.
    ui->deStartDate->blockSignals(true);
    ui->deEndDate->blockSignals(true);

    QString currentText = ui->cbPreset->currentText();
    QDate currentDate = QDate::currentDate();
    if (currentText == "Today") {
        ui->chkRange->setCheckState(Qt::Unchecked);
        ui->deStartDate->setDate(currentDate);
    } else if (currentText == "Past Week") {
        ui->chkRange->setCheckState(Qt::Checked);
        QDate startDate = currentDate.addDays(-6);
        ui->deEndDate->setDate(currentDate);
        ui->deStartDate->setDate(startDate);
    } else if (currentText == "Past 2 Weeks") {
        ui->chkRange->setCheckState(Qt::Checked);
        QDate startDate = currentDate.addDays(-13);
        ui->deStartDate->setDate(startDate);
        ui->deEndDate->setDate(currentDate);
    } else if (currentText == "This Week") {
        ui->chkRange->setCheckState(Qt::Checked);
        int dayOfWeek = currentDate.dayOfWeek();
        int offset = dayOfWeek % 7;  // When dayOfWeek is Mon to Sat (1-6), the resultant value will also be that value, and be used to subtract the currentDate. When the dayOfWeek is Sun (7), the resultant value will instead be 0, which supports the logic where the routineStartDate doesn't need to be subtracted if it's already on Sun (the first date of the week).
        QDate sunDate = currentDate.addDays(-offset);
        ui->deStartDate->setDate(sunDate);
        ui->deEndDate->setDate(currentDate);
    } else if (currentText == "This Month") {
        ui->chkRange->setCheckState(Qt::Checked);
        int year = currentDate.year();
        int month = currentDate.month();
        QDate firstDateOfMonth = QDate(year, month, 1);  // First day of month is always 1.
        ui->deStartDate->setDate(firstDateOfMonth);
        ui->deEndDate->setDate(currentDate);
    } else if (currentText == "This Quarter") {
        int year = currentDate.year();
        int month = currentDate.month();
        QDate firstDateOfQuarter;
        if (month >= 1 && month <= 3) {
            firstDateOfQuarter = QDate(year, 1, 1);  // First day of first quarter of year is always 1st Jan (1).
        } else if (month >= 4 && month <= 6) {
            firstDateOfQuarter = QDate(year, 4, 1);  // First day of second quarter of year is always 1st Apr (4).
        } else if (month >= 7 && month <= 9) {
            firstDateOfQuarter = QDate(year, 7, 1);  // First day of third quarter of year is always 1st Jul (7).
        } else if (month >= 10 && month <= 12) {
            firstDateOfQuarter = QDate(year, 10, 1);  // First day of fourth quarter of year is always 1st Oct (10).
        } else {
            QMessageBox::critical(this, "Computation Error", "Invalid month found");
        }
        if (!firstDateOfQuarter.isValid()) {
            QMessageBox::critical(this, "Computation Error", "Null date found");
        }
        ui->deStartDate->setDate(firstDateOfQuarter);
        ui->deEndDate->setDate(currentDate);
    } else if (currentText == "This Half-Year") {
        ui->chkRange->setCheckState(Qt::Checked);
        int year = currentDate.year();
        int month = currentDate.month();
        QDate firstDateOfSemiannual;
        if (month >= 1 && month <= 6) {
            firstDateOfSemiannual = QDate(year, 1, 1);  // First day of first half of year is always 1st Jan (1).
        } else if (month >= 7 && month <= 12) {
            firstDateOfSemiannual = QDate(year, 7, 1);  // First day of second half of year is always 1st Jul (7).
        } else {
            QMessageBox::critical(this, "Computation Error", "Invalid month found");
        }
        if (!firstDateOfSemiannual.isValid()) {
            QMessageBox::critical(this, "Computation Error", "Null date found");
        }
        ui->deStartDate->setDate(firstDateOfSemiannual);
        ui->deEndDate->setDate(currentDate);
    } else if (currentText == "This Year") {
        ui->chkRange->setCheckState(Qt::Checked);
        int year = currentDate.year();
        QDate firstDateOfYear = QDate(year, 1, 1);  // First day of year is always 1st Jan (1).
        ui->deStartDate->setDate(firstDateOfYear);
        ui->deEndDate->setDate(currentDate);
    }

    // Remove 'Custom' option when one of the pre-populated preset is selected.
    if (isCustom){
        int indexCustom = ui->cbPreset->findText("Custom");
        ui->cbPreset->removeItem(indexCustom);
        isCustom = false;
    }

    // Release the blocked signals.
    ui->deStartDate->blockSignals(false);
    ui->deEndDate->blockSignals(false);
}
