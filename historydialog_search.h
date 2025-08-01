#ifndef HISTORYDIALOG_SEARCH_H
#define HISTORYDIALOG_SEARCH_H

#include <QDialog>

namespace Ui {
class HistoryDialog_Search;
}

class HistoryDialog_Search : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryDialog_Search(QWidget *parent = nullptr);
    ~HistoryDialog_Search();

private:
    Ui::HistoryDialog_Search *ui;
};

#endif // HISTORYDIALOG_SEARCH_H
