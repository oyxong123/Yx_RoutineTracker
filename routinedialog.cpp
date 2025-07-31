#include "routinedialog.h"
#include "ui_routinedialog.h"
#include <QPushButton>

RoutineDialog::RoutineDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RoutineDialog)
{
    ui->setupUi(this);

    QObject::connect(ui->btnHome, &QPushButton::clicked, this, &RoutineDialog::btnHome_clicked);

    qDebug() << "Routine Dialog initialized.";
}

RoutineDialog::~RoutineDialog()
{
    delete ui;
}

void RoutineDialog::btnHome_clicked()
{
    close();
}
