#ifndef HISTORYPLOTDIALOG_H
#define HISTORYPLOTDIALOG_H

#include <QDialog>

namespace Ui {
class HistoryPlotDialog;
}

class HistoryPlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryPlotDialog(QWidget *parent = nullptr);
    ~HistoryPlotDialog();

private:
    Ui::HistoryPlotDialog *ui;
};

#endif // HISTORYPLOTDIALOG_H
