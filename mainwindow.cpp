#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "protocol.h"

#include <QMenu>
#include <QAction>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sStatusBar = statusBar();//创建状态栏

    lSerialStatus = new QLabel("SerialClose", this);
    lWorkStatusWatch = new QLabel("工况监控关闭", this);
    lwellCtrlBoxWatch = new QLabel("井口控制箱监控关闭", this);

    sStatusBar->addPermanentWidget(lSerialStatus);
    sStatusBar->addPermanentWidget(lWorkStatusWatch);
    sStatusBar->addPermanentWidget(lwellCtrlBoxWatch);

//    lable->setText("Normal text file");
//    sStatusBar->addWidget(lable);//状态栏添加组件
//    sBar->addWidget(new QLabel("2",this));//addWidget:从左往右添加

    serial = new QSerialPort;
    //信号与槽函数关联
    connect (serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//发送数据槽函数
void MainWindow::sendSerialData()
{
/*
    serial->write (ui->textEdit_send->toPlainText().toLatin1());
*/
}

void MainWindow::readSerialData()
{

    QByteArray buf;
    qDebug() << "readData: " << endl;
    buf = serial->readAll();

    if (!buf.isEmpty()) {
        qDebug() << "   " << buf;

/*        QString str = ui->textEdit_recv->toPlainText();
        str += tr (buf);
        ui->textEdit_recv->clear();
        ui->textEdit_recv->append (str);
*/    }

}

void MainWindow::on_muSerialConf_triggered()
{
    uiSerialConfig = new SerialConfigDialog(this, serial);
    uiSerialConfig->show();
    connect(uiSerialConfig,SIGNAL(sendSerialStatus(QString)),this,SLOT(RcvSerialStatus(QString)));
}

void MainWindow::on_actionPlatformConf_triggered()
{
    uiPlatformConfig = new PlatformConfigDialog(this);
    uiPlatformConfig -> show();
}

void MainWindow::on_actionOpenRealtimePlot_triggered()
{
    uiRealtimePlot = new RealtimePlotDialog(this);
    uiRealtimePlot -> show();
}

void MainWindow::on_actionOpenHistoryPlot_triggered()
{
    uiHistoryPlot = new HistoryPlotDialog(this);
    uiHistoryPlot -> show();
}

void MainWindow::RcvSerialStatus(QString stat)
{
    this->show();
    lSerialStatus->setText(stat);
}

void MainWindow::on_workStatus_toggled(bool arg1)
{
     if(arg1)
    {
        lWorkStatusWatch->setText("工况监控打开");
    }
    else
    {
        lWorkStatusWatch->setText("工况监控关闭");
    }
}

void MainWindow::on_wellCtrlBox_toggled(bool arg1)
{
    if(arg1)
    {
        lwellCtrlBoxWatch->setText("井口控制箱监控打开");
    }
    else
    {
        lwellCtrlBoxWatch->setText("井口控制箱监控关闭");
    }
}

void MainWindow::on_platformTableWidget_cellDoubleClicked(int row, int column)
{
    QMenu *platTableMenu = new QMenu;
    // 弹出可操作选项 记录数据 / 保存数据 / 图表显示
    QAction *recordAction = new QAction(QStringLiteral("记录数据"));
    QAction *saveAction = new QAction(QStringLiteral("保存数据"));
    QAction *animateAction = new QAction(QStringLiteral("图表显示"));

    platTableMenu -> addAction(recordAction);
    platTableMenu -> addAction(saveAction);
    platTableMenu -> addAction(animateAction);

    connect(recordAction, SIGNAL(triggered(bool)), this, SLOT(on_actionOpenRealtimePlot_triggered()));
    connect(saveAction, SIGNAL(triggered(bool)), this, SLOT(on_actionOpenRealtimePlot_triggered()));
    connect(animateAction, SIGNAL(triggered(bool)), this, SLOT(on_actionOpenRealtimePlot_triggered()));

    platTableMenu->exec(QCursor::pos());
}

void MainWindow::on_platformTableWidget_customContextMenuRequested(const QPoint &pos)
{

}
