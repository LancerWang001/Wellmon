#ifndef REALTIMEPLOTDIALOG_H
#define REALTIMEPLOTDIALOG_H

#include <QDialog>

namespace Ui {
class RealtimePlotDialog;
}

class RealtimePlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RealtimePlotDialog(QWidget *parent = nullptr);
    ~RealtimePlotDialog();

private:
    Ui::RealtimePlotDialog *ui;
};

#endif // REALTIMEPLOTDIALOG_H
