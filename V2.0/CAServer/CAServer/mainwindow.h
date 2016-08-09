#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <fstream>
#include <QFile>
#include <QFileDialog>
#include <QDateTime>
#include <qstring.h>
#include <qdir.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    /*/----------------------公共变量-----------------------\*/
    QString coredir;        //储存core文件夹路径 存放根证书及密钥
    QString reqdir;         //储存reqfiles文件夹路径 存放待处理请求文件
    QString signdir;        //存储signedfiles文件夹路径 存放生成用户证书及密钥
    QString reqfindir;      //储存reqfin文件夹路径 存放处理完成的请求文件
    QString reqfilename;    //接受的证书请求文件名
    /*\---------------------------------------------------/*/


    /*/-----------------文件接收使用到的变量------------------\*/
    QTcpServer tcpserver;   //tcpserver对象
    QTcpSocket *tcpserconn; //tcp套接字连接对象
    qint64 totalbytes;      //接收数据总大小
    qint64 bytesrecved;     //已接受到的文件大小
    qint64 filenamesize;    //文件名大小
    QString filename;       //接收文件名
    QFile *localfile;       //本地文件
    QByteArray inblock;     //接收数据缓冲区
    /*\---------------------------------------------------/*/

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void start(); //开启监听函数

    void getLocalIpAddr(); //获取本地IPv4地址函数

    void acceptConnection(); //建立连接

    void updateServerProgress(); //更新进度条并接收数据

    void displayError(QAbstractSocket::SocketError socketerror); //网络错误显示

    QString getTime(); //获取当前系统时间函数

    X509 * LoadCert(); //X509 载入根证书函数

    EVP_PKEY * LoadKey(); //载入根证书私钥函数

    //根证书签名
    bool CreateCertFromRequestFile(int serialNumber,int days,
                                   char *requestFile,char *pubCert,
                                   char *priCert, int format);

    void SignCertFile(); //签名处理函数

    void selectFile(); //选择文件函数

    void on_pushButton_clicked(); //点击[接收文件]按钮

    void on_pushButton_6_clicked(); //点击[刷新IP]按钮

    void on_pushButton_5_clicked(); //点击[根证书签名]按钮

    void on_pushButton_4_clicked(); //点击[选择文件]按钮

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
