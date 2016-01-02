#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    bits=512;
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

//导入待验证证书


//验证证书

void MainWindow::on_pushButton_7_clicked()
{
    verify.userCerUrl = QFileDialog::getOpenFileName(this,"select file","./",NULL);
    Load_Cer();
}

void MainWindow::on_pushButton_8_clicked()
{
    if (verify.userCerUrl==NULL)
        QMessageBox::warning(this,"警告","请选择证书！","确定");
    else
    {
        if(CheckCertWithRoot())
            qDebug()<<"Ok";
        else
            qDebug()<<"False";
//        X509_Pem_Verify();
    }
}
