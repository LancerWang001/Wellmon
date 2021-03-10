#ifndef PLATFORMCONFIGDIALOG_H
#define PLATFORMCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class PlatformConfigDialog;
}

class PlatformConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlatformConfigDialog(QWidget *parent = nullptr);
    ~PlatformConfigDialog();

private:
    Ui::PlatformConfigDialog *ui;
};

#endif // PLATFORMCONFIGDIALOG_H
