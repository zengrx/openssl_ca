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
    reqfindir = "../reqfin/";    //存储完成签发的请求文件相对路径
    signdir = "../signedfiles/"; //存储签发文件相对路径

    //文件接收部分
    totalbytes = 0;
    bytesrecved = 0;
    filenamesize = 0;
    /*\--------------------变量初始化部分----------------------/*/
    connect(&tcpserver,SIGNAL(newConnection()),this,SLOT(acceptConnection()));

    /*---------------------函数功能初始化----------------------*/
    getLocalIpAddr(); //
    initCrlList();    //

}

MainWindow::~MainWindow()
{
    delete ui;
}

////
/// \brief MainWindow::selectFile
/// 选择文件函数
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
        QString filename; //局部变量 储存无后缀文件名
        fileinfo = QFileInfo(absurl);
        //获取文件名
        filename = fileinfo.fileName();
        //ui->lineEdit_10->setText(filename);
        //除去后缀名
        int index = filename.lastIndexOf(".");
        filename.truncate(index);
        reqfilename = filename;
        ui->textBrowser->append(getTime() + "选择文件 '" + reqfilename + "' 成功");
        ui->pushButton_5->setEnabled(true);
    }
}

////
/// \brief MainWindow::loadRootCA
/// \return true or false
/// 载入根证书信息函数
/// 调用loadCert及loadKey为certop结构体赋值
///
bool MainWindow::loadRootCA()
{
    //声明变量及传值
    X509 *rootcert = NULL;
    EVP_PKEY *pkey = NULL;
    rootcert = loadCert();
    pkey = loadKey();
    certop.rootcert = rootcert;
    certop.pkey = pkey;
    if (pkey == NULL || rootcert == NULL)
    {
        ui->textBrowser->append(getTime() + "加载根证书或密钥失败，请重试");
        return false;
    }
    else
    {
        ui->textBrowser->append(getTime() + "加载根证书及密钥成功");
        return true;
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
    ui->pushButton_5->setEnabled(false);
}

//点击[选择文件]按钮事件
void MainWindow::on_pushButton_4_clicked()
{
    selectFile();
}

//点击[撤销证书]按钮事件
void MainWindow::on_pushButton_7_clicked()
{
    certop.ser = ui->lineEdit_2->text();
    if(revokeCert())
    {
        qDebug() << "seccess";
        showCrlInfo();
    }
}

//点击[生成撤销链]按钮事件
void MainWindow::on_pushButton_9_clicked()
{
    createCrl();
}
