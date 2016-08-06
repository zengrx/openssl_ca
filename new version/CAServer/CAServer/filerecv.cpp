#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostInfo>

////
/// \brief MainWindow::start
///
///
///
void MainWindow::start()
{
    ui->pushButton->setEnabled(false);
    //bytesrecved = 0; //开始监听时清空之前数据
    QString hostip; //服务器ip
    int hostport; //服务器端口号
    hostip = ui->comboBox->currentText();
    hostport = ui->lineEdit->text().toInt();
    if(!tcpserver.listen(QHostAddress(hostip), hostport))
    {
        ui->textBrowser->append("error");
        close();
        return;
    }
    else
    {
        ui->textBrowser->append("服务器开启...正在监听...");
    }
}

////
/// \brief MainWindow::acceptConnection
///
///
void MainWindow::acceptConnection()
{
    //tcpsktconn = new QTcpSocket;
    tcpserconn = tcpserver.nextPendingConnection();
    connect(tcpserconn,SIGNAL(readyRead()),this,SLOT(updateServerProgress()));
    connect(tcpserconn,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayError(QAbstractSocket::SocketError)));
    ui->textBrowser->append("接受连接");
    tcpserver.close(); //单线程
}

////
/// \brief MainWindow::updateServerProgress
///
///
void MainWindow::updateServerProgress()
{
    QDataStream in(tcpserconn);
    in.setVersion(QDataStream::Qt_4_6);

    if(bytesrecved <= sizeof(qint64)*2)
    { //如果接收到的数据小于16个字节，那么是刚开始接收数据，我们保存到//来的头文件信息
        if((tcpserconn->bytesAvailable() >= sizeof(qint64)*2)&& (filenamesize == 0))
        { //接收数据总大小信息和文件名大小信息
            in >> totalbytes >> filenamesize;
            bytesrecved += sizeof(qint64) * 2;
        }
        if((tcpserconn->bytesAvailable() >= filenamesize)
                && (filenamesize != 0))
        {  //接收文件名，并建立文件
            in >> filename;
            ui->textBrowser->append(tr("正在接收文件 '%1' ...").arg(filename));
            bytesrecved += filenamesize;
            localfile = new QFile(filename);
            if(!localfile->open(QFile::WriteOnly))
            {
                qDebug() << "open file error!";
                return;
            }
        }
        else
        {
            return;
        }
    }
    if(bytesrecved < totalbytes)
    {  //如果接收的数据小于总数据，那么写入文件
        bytesrecved += tcpserconn->bytesAvailable();
        inblock = tcpserconn->readAll();
        localfile->write(inblock);
        inblock.resize(0);
    }
    ui->progressBar->setMaximum(totalbytes);
    ui->progressBar->setValue(bytesrecved);
    //更新进度条
    if(bytesrecved == totalbytes)
    { //接收数据完成时
        tcpserconn->close();
        localfile->close();
        ui->pushButton->setEnabled(true);
        ui->textBrowser->append(QString("接收文件 '%1' 成功！").arg(filename));
        //完成一个传输，数据清零
        totalbytes = 0;
        bytesrecved = 0;
        filenamesize = 0;
    }
}

////
/// \brief MainWindow::displayError
///
///
void MainWindow::displayError(QAbstractSocket::SocketError) //错误处理
{
    qDebug() << tcpserconn->errorString();
    tcpserconn->close();
    ui->progressBar->reset();
    //ui->serverStatusLabel->setText(tr("服务端就绪"));
    ui->pushButton->setEnabled(true);
}

////
/// \brief MainWindow::getLocalIpAddr
///
void MainWindow::getLocalIpAddr()
{
    ui->comboBox->addItem("127.0.0.1");
    QString localHostName = QHostInfo::localHostName();
    //qDebug() <<"localHostName:"<<localHostName;
    QHostInfo info = QHostInfo::fromName(localHostName);
    //循环添加获取的IP地址
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            //qDebug() <<"IPV4 Address: "<< address.toString();
            ui->comboBox->addItem(address.toString());
        }
    }
}
////
/// \brief MainWindow::getTime
/// \return
///
QString MainWindow::getTime()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("[hh:mm:ss]  ");
    return current_date;
}
