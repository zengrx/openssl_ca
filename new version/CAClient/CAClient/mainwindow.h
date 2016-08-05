#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QFile>
#include <QDateTime>
#include <qstring.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>

namespace Ui {
class MainWindow;
}

//证书相关信息结构体
struct certInfo
{
    QString common;       //用户称呼
    QString country;      //国家
    QString locality;     //地区
    QString province;     //省或州
    QString organization; //组织
    QString ogUnit;       //部门
    QString emailAddr;    //邮箱地址
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    /*************************************************************/
    //公用变量
    QString dir; //定义的操作目录
    /*************************************************************/

    /*-----------------------------------------------------------*/
    //文件传输中用到的变量
    QTcpSocket *tcpClient;         //tcp连接
    QFile *localFile;              //本地需要发送的文件
    qint64 totalBytes;             //文件总大小
    qint64 bytesWritten;           //已经发送的数据大小
    qint64 bytesToWrite;           //剩余数据大小
    qint64 loadSize;               //一次发送数据大小
    QString fileName;              //文件路径
    QByteArray outBlock;           //数据缓冲区，用于存放每次发送的数据
    /*-----------------------------------------------------------*/

    /*-----------------------------------------------------------*/
    //certreq.cpp中使用到的变量
    X509_REQ *req;                 //x509请求对象
    int ret;                       //返回值
    long version;                  //版本号
    X509_NAME *name;               //x509文件对象
    EVP_PKEY *pkey;                //秘钥
    RSA *rsa;                      //公钥参数
    X509_NAME_ENTRY *entry = NULL; //信息填充对象
    char bytes[100], mdout[20];    //strcpy参数及x509请求参数
    int len, mdlen;                //字符串长度
    int bits;                      //rsa参数1
    unsigned long e = RSA_3;       //rsa参数2
    unsigned char *der, *p;        //der请求参数
    FILE *fp;                      //文件对象
    const EVP_MD *md;              //EVP对象，生成请求用
    X509 *x509;                    //x509对象
    BIO *b;                        //BIO对象
    /*------------------------------------------------------------*/

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int certReq(); //生成证书请求文件函数

private slots:
    QString setAline(); //设置信息栏对齐

    QString getTime(); //获取当前时间

    void sendFile(); //连接服务器并发送文件函数

    void selectFile(); //选择文件函数

    void startTransfer(); //发送文件大小等信息

    void updateClientProgress(qint64); //发送数据并更新进度条

    void displayError(QAbstractSocket::SocketError); //显示错误信息

    void on_pushButton_4_clicked(); //点击[选择文件]按钮

    void on_pushButton_3_clicked(); //点击[发送]按钮

    void on_pushButton_clicked(); //点击[生成请求文件]按钮

private:
    Ui::MainWindow *ui;


};

#endif // MAINWINDOW_H
