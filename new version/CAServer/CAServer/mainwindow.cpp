#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //初始化变量
    totalbytes = 0;
    bytesrecved = 0;
    filenamesize = 0;
    connect(&tcpserver,SIGNAL(newConnection()),this,SLOT(acceptConnection()));

    getLocalIpAddr(); //

}

MainWindow::~MainWindow()
{
    delete ui;
}

//点击[接收文件]按钮事件
void MainWindow::on_pushButton_clicked()
{
    start();
}

//点击[刷新IP]按钮事件
void MainWindow::on_pushButton_6_clicked()
{
    ui->comboBox->clear(); //清除下拉列表内容
    getLocalIpAddr();
}
