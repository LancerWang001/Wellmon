#include "platformconfigdialog.h"
#include "ui_platformconfigdialog.h"

PlatformConfigDialog::PlatformConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlatformConfigDialog)
{
    ui->setupUi(this);
}

PlatformConfigDialog::~PlatformConfigDialog()
{
    delete ui;
}
