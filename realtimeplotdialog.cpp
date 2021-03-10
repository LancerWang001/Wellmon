#include "realtimeplotdialog.h"
#include "ui_realtimeplotdialog.h"

RealtimePlotDialog::RealtimePlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RealtimePlotDialog)
{
    ui->setupUi(this);
}

RealtimePlotDialog::~RealtimePlotDialog()
{
    delete ui;
}
