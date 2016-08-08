#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*/--------------------变量初始化部分----------------------\*/
    //公共部分
    coredir = "../core/";        //根证书及根证书私钥相对路径
    reqdir = "../reqfiles/";     //存储请求文件相对路径
    signdir = "../signedfiles/"; //存储签发文件相对路径

    //文件接收部分
    totalbytes = 0;
    bytesrecved = 0;
    filenamesize = 0;
    /*\--------------------变量初始化部分----------------------/*/
    connect(&tcpserver,SIGNAL(newConnection()),this,SLOT(acceptConnection()));

    getLocalIpAddr(); //

}

MainWindow::~MainWindow()
{
    delete ui;
}

////
/// \brief MainWindow::selectFile
///
void MainWindow::selectFile()
{
    QFileInfo fileinfo;
    //获取文件绝对路径
    QString absurl = QFileDialog::getOpenFileName(this,"select file",reqdir,"*.csr");
    if (absurl.isNull())
    {
        //QMessageBox::warning(NULL,"error","Select file failed!\n");
        ui->textBrowser->append(getTime() + "选择文件失败");
    }
    else
    {
        QString filename;
        fileinfo = QFileInfo(absurl);
        //获取文件名
        filename = fileinfo.fileName();
        //ui->lineEdit_10->setText(filename);
        //除去后缀名
        int index = filename.lastIndexOf(".");
        filename.truncate(index);
        reqfilename = filename;
        ui->textBrowser->append(getTime() + "选择文件 '" + reqfilename + "' 成功");
    }
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

//点击[根证书签名]按钮事件
void MainWindow::on_pushButton_5_clicked()
{
    SignCertFile();
}

//点击[选择文件]按钮事件
void MainWindow::on_pushButton_4_clicked()
{
    selectFile();
}
