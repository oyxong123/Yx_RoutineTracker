#include "routinedialog_add.h"
#include "ui_routinedialog_add.h"

#include <QPushButton>
#include <QMessageBox>

RoutineDialog_Add::RoutineDialog_Add(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RoutineDialog_Add)
{
    ui->setupUi(this);
    rd = qobject_cast<RoutineDialog *>(parent);
    if (!rd) {
        QMessageBox::critical(this, "App Error", "Parent is not a RoutineDialog!");
    }

    QObject::connect(ui->btnTest, &QPushButton::clicked, this, &RoutineDialog_Add::btnTest_clicked);

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

void RoutineDialog_Add::btnTest_clicked()
{
    close();
}
