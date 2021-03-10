#include "serialconfigdialog.h"
#include "ui_serialconfigdialog.h"

static const char blankString[] = QT_TRANSLATE_NOOP ("SettingsDialog", "N/A");

SerialConfigDialog::SerialConfigDialog (QWidget *parent) :
    QDialog (parent),
    ui (new Ui::SerialConfigDialog)
{
    ui->setupUi (this);
}

SerialConfigDialog::SerialConfigDialog (QWidget *parent, QSerialPort *serial) :
    QDialog (parent),
    ui (new Ui::SerialConfigDialog)
{
    ui->setupUi (this);
    serialconf = serial;
    insertSerialEnum();
}

SerialConfigDialog::~SerialConfigDialog()
{
    delete ui;
}

void SerialConfigDialog::insertSerialEnum()
{
    //将所有可以使用的串口设备添加到ComboBox中
    QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
    QString description;
    QString manufacturer;
    QString serialNumber;

    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
        QStringList list;
        description = serialPortInfo.description();
        manufacturer = serialPortInfo.manufacturer();
        serialNumber = serialPortInfo.serialNumber();
        list << serialPortInfo.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << serialPortInfo.systemLocation()
             << (serialPortInfo.vendorIdentifier() ? QString::number (serialPortInfo.vendorIdentifier(), 16) : blankString)
             << (serialPortInfo.productIdentifier() ? QString::number (serialPortInfo.productIdentifier(), 16) : blankString);
        ui->cbSerialPort->addItem (list.first(), list);
    }
}


void SerialConfigDialog::on_SerialOpenClose_clicked()
{
    QString strSerialStat = "";
    if (ui->SerialOpenClose->text() == "打开串口") {
        ui->SerialOpenClose->setText ("关闭串口");
        //设置串口名字
        serialconf->setPortName (ui->cbSerialPort->currentText());
        //设置波特率
        serialconf->setBaudRate (ui->cbBaud->currentText().toInt());

        //设置数据位
        switch (ui->cbDataBit->currentText().toInt()) {
            case 5:
                serialconf->setDataBits (QSerialPort::Data5);
                break;

            case 6:
                serialconf->setDataBits (QSerialPort::Data6);
                break;

            case 7:
                serialconf->setDataBits (QSerialPort::Data7);
                break;

            case 8:
                serialconf->setDataBits (QSerialPort::Data8);
                break;

            default:
                serialconf->setDataBits (QSerialPort::UnknownDataBits);
                break;
        }

        //设置停止位
        if (ui->cbStopBit->currentText() == "1") {
            serialconf->setStopBits (QSerialPort::OneStop);
        } else if (ui->cbStopBit->currentText() == "1.5") {
            serialconf->setStopBits (QSerialPort::OneAndHalfStop);
        } else if (ui->cbStopBit->currentText() == "2") {
            serialconf->setStopBits (QSerialPort::TwoStop);
        } else {
            serialconf->setStopBits (QSerialPort::UnknownStopBits);
        }

        //设置奇偶校验位
        if (ui->cbParity->currentText() == "None") {
            serialconf->setParity (QSerialPort::NoParity);
        } else if (ui->cbParity->currentText() == "Odd") {
            serialconf->setParity (QSerialPort::OddParity);
        } else if (ui->cbParity->currentText() == "Even") {
            serialconf->setParity (QSerialPort::EvenParity);
        } else if (ui->cbParity->currentText() == "Mark") {
            serialconf->setParity (QSerialPort::MarkParity);
        } else if (ui->cbParity->currentText() == "Space") {
            serialconf->setParity (QSerialPort::SpaceParity);
        } else {
            serialconf->setStopBits (QSerialPort::UnknownStopBits);
        }

        //设置流控
        if (ui->cbParity->currentText() == "None") {
            serialconf->setFlowControl (QSerialPort::NoFlowControl);
        } else if (ui->cbParity->currentText() == "Hardware") {
            serialconf->setFlowControl (QSerialPort::HardwareControl);
        } else if (ui->cbParity->currentText() == "Software") {
            serialconf->setFlowControl (QSerialPort::SoftwareControl);
        } else {
            serialconf->setStopBits (QSerialPort::UnknownStopBits);
        }

        //打开串口
        if (serialconf->open (QIODevice::ReadWrite)) {
            ui->cbSerialPort->setEnabled (false);
            ui->cbBaud  ->setEnabled (false);
            ui->cbDataBit ->setEnabled (false);
            ui->cbStopBit ->setEnabled (false);
            ui->cbParity ->setEnabled (false);
            ui->cBFlowCtrl ->setEnabled (false);
            strSerialStat = ui->cbSerialPort->currentText() + ' ' +
                    ui->cbBaud->currentText() + ',' +
                    ui->cbDataBit->currentText() + ',' +
                    ui->cbStopBit->currentText() + ',' +
                    ui->cbParity->currentText() + ',' +
                    ui->cBFlowCtrl->currentText();
        }
    } else if (ui->SerialOpenClose->text() == "关闭串口") {
        ui->SerialOpenClose->setText ("打开串口");
        serialconf->close();
        //serial->deleteLater();
        //恢复设置功能
        ui->cbSerialPort->setEnabled (true);
        ui->cbBaud->setEnabled (true);
        ui->cbDataBit->setEnabled (true);
        ui->cbStopBit->setEnabled (true);
        ui->cbParity->setEnabled (true);
        ui->cBFlowCtrl->setEnabled (true);
        strSerialStat = "SerialClose";
    }
    emit sendSerialStatus(strSerialStat);
}

