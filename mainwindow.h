#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <QDebug>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QCheckBox>
#include <QGraphicsRectItem>
#include <QGraphicsView>


#include "serialconfigdialog.h"
#include "platformconfigdialog.h"
#include "realtimeplotdialog.h"
#include "historyplotdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SerialConfigDialog *uiSerialConfig;
    PlatformConfigDialog *uiPlatformConfig;
    RealtimePlotDialog *uiRealtimePlot;
    HistoryPlotDialog *uiHistoryPlot;
    QSerialPort *serial;//串口实例
    QStatusBar * sStatusBar;
    QLabel *lSerialStatus;
    QLabel *lWorkStatusWatch;
    QLabel *lwellCtrlBoxWatch;
    QGraphicsView *wellGraphicView;

private slots://必须加slots
    void sendSerialData();
    void readSerialData();
    void on_muSerialConf_triggered();
    void on_actionPlatformConf_triggered();
    void on_actionOpenRealtimePlot_triggered();
    void on_actionOpenHistoryPlot_triggered();
    void  RcvSerialStatus(QString stat);
    void on_workStatus_toggled(bool arg1);
    void on_wellCtrlBox_toggled(bool arg1);
    void on_platformTableWidget_customContextMenuRequested(const QPoint &pos);
    void on_platformTableWidget_cellDoubleClicked(int row, int column);
};
#endif // MAINWINDOW_H
