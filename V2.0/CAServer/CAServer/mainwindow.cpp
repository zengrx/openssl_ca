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

    indexptr = -1;               //ListWidget索引

    //文件接收部分
    totalbytes = 0;              //接收文件全部大小=文件真实大小+文件名等信息大小
    bytesrecved = 0;             //已接收的文件大小
    filenamesize = 0;            //文件名等信息大小
    /*\-----------------------------------------------------/*/


    /*/--------------------函数功能初始化---------------------\*/
    getLocalIpAddr();   //获取本机IP地址
    initCrlList();      //初始化撤销链信息
    updateListWidget(); //初始化证书签发列表
    /*\-----------------------------------------------------/*/

    connect(&tcpserver,SIGNAL(newConnection()),this,SLOT(acceptConnection()));



}

MainWindow::~MainWindow()
{
    delete ui;
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
        ui->textBrowser->append(getTime() + "根证书及密钥加载成功...");
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

//点击[选择请求文件]按钮事件
void MainWindow::on_pushButton_4_clicked()
{
    selectReqFile();
}

//点击[撤销证书]按钮事件
void MainWindow::on_pushButton_7_clicked()
{
    certop.ser = ui->lineEdit_2->text();
    if(revokeCert())
    {
        ui->textBrowser->append(getTime() + "撤销证书成功，该证书已不具备效用");
        showCrlInfo();
    }
}

//点击[生成撤销链]按钮事件
void MainWindow::on_pushButton_9_clicked()
{
    if(createCrl())
    {
        ui->textBrowser->append(getTime() + "已生成新的撤销链");
    }
}

//点击[恢复证书]按钮事件
void MainWindow::on_pushButton_8_clicked()
{
    if(!restoreCert())
    {
        ui->textBrowser->append(getTime() + "证书恢复失败，请重试");
        return;
    }
    ui->pushButton_8->setEnabled(false);
}

//ListWidget行点击事件
void MainWindow::on_listWidget_2_currentRowChanged(int currentRow)
{
    ui->pushButton_8->setEnabled(true); //激活按键
    indexptr = currentRow-1; //获取当前位置索引值
    qDebug() << indexptr;
}

//点击[选择证书文件]按钮事件
void MainWindow::on_pushButton_2_clicked()
{
    selectCertFile();
}

//点击[验证证书]按钮事件
void MainWindow::on_pushButton_10_clicked()
{
    rootCaVerify();
}
