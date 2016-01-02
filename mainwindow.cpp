#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(722,481);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//证书请求文件按钮事件
void MainWindow::on_pushButton_clicked()
{
    //执行请求文件
    careq();
    //写入message
    showMessage();
}

//借来查看证书内容
// (～￣▽￣)→))*￣▽￣*)o主要用来签名
void MainWindow::on_pushButton_2_clicked()
{
    int day;
    //detail();
    char name1[100];
    char name2[100];
    char name3[100];
    strcpy(name1,(ui->lineEdit->text()+".csr").toStdString().c_str());
    strcpy(name2,(ui->lineEdit->text()+".crt").toStdString().c_str());
    strcpy(name3,(ui->lineEdit->text()+".key").toStdString().c_str());
    day = ui->lineEdit_8->text().toInt();
    if(CreateCertFromRequestFile(8,day,name1,name2,name3,3))
        printf("ss\n");
    else
        printf("gg\n");
    //测试输出
    detail();
}
