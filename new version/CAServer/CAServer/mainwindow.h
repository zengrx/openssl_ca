#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QDateTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    /****************************************/
    QTcpServer tcpserver;   //tcpserver对象
    QTcpSocket *tcpserconn; //tcp套接字连接对象
    qint64 totalbytes;      //接收数据总大小
    qint64 bytesrecved;     //已接受到的文件大小
    qint64 filenamesize;    //文件名大小
    QString filename;       //接收文件名
    QFile *localfile;       //本地文件
    QByteArray inblock;     //接收数据缓冲区
    /****************************************/

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

    void on_pushButton_clicked(); //[]槽函数

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
