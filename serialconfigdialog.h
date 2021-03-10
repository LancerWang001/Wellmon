#ifndef SERIALCONFIGDIALOG_H
#define SERIALCONFIGDIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace Ui {
class SerialConfigDialog;
}

class SerialConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialConfigDialog(QWidget *parent = nullptr);
    explicit SerialConfigDialog(QWidget *parent = nullptr, QSerialPort *serial = nullptr);
    ~SerialConfigDialog();

private slots:
    void on_SerialOpenClose_clicked();

private:
    Ui::SerialConfigDialog *ui;
    QSerialPort *serialconf;//串口实例
    void insertSerialEnum();

signals:
    void sendSerialStatus(QString stat);
};

#endif // SERIALCONFIGDIALOG_H
