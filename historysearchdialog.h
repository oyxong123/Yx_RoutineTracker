#ifndef HISTORYSEARCHDIALOG_H
#define HISTORYSEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class HistorySearchDialog;
}

class HistorySearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistorySearchDialog(QWidget *parent = nullptr);
    ~HistorySearchDialog();

private:
    Ui::HistorySearchDialog *ui;
};

#endif // HISTORYSEARCHDIALOG_H
