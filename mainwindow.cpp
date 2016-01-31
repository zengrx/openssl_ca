#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>

using namespace std;

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

//选择待验证证书
void MainWindow::on_pushButton_7_clicked()
{
    verify.userCerUrl = QFileDialog::getOpenFileName(this,"select file","./","*.crt;*.der;*.pem;*.cer");
    if (verify.userCerUrl.isNull())
    {
        QMessageBox::information(NULL,"error","Select file failed!\n");
        message += getTime() + "Select file failed!\n";
        showMessage();
    }
    else
    {
        if(Load_Cer()>0)
        {
            message += getTime() + "rootCert loaded ...\n";
            message += noTime() + "userCert loaded ...\n";
            message += noTime() + "PrivateKey loaded ...\n";
            showMessage();
            certInfo info;
            QString tmpstr_0=GetCertSubjectString(&info);
            if(!tmpstr_0.isNull())
            {
                ui->lineEdit_22->setText(info.client);
                ui->lineEdit_23->setText(info.country);
                ui->lineEdit_24->setText(info.state);
                ui->lineEdit_25->setText(info.location);
                ui->lineEdit_26->setText(info.organization);
                ui->lineEdit_27->setText(info.organizationalUnitName);
                ui->lineEdit_28->setText(info.email);
                message += getTime() + "Certificate Detail:";
                QString tmpstr_1 = GetCertSerialNumber();
                if(!tmpstr_1.isNull())
                {
                    message += "\n" + noTime() + "SerialNumber: ";
                    message += tmpstr_1;
                }
                message += tmpstr_0;
                message += "\n";
            }
            showMessage();
        }
        else
        {
            message += getTime()+"Load file faild!\n";
            showMessage();
        }
    }
}

//验证证书
void MainWindow::on_pushButton_8_clicked()
{
    if (verify.userCerUrl==NULL)
    {
        QMessageBox::warning(this,"警告","请选择证书！","确定");
        return;
    }
    else
    {
        if(CheckCertWithRoot())
        {
            message += getTime() + "Verify with ca, ok ...\n";
        }
        else
        {
            QMessageBox::warning(this,"警告","不受根证书信任的证书！","确定");
            message+= noTime()+ "Verify with ca, false ...\n";
        }
        if(CheckCertTime())
        {
            message+= noTime() + "Verify certificate life time, ok ...\n";
        }
        else
        {
            QMessageBox::warning(this,"警告","证书过期！","确定");
            message+= noTime() + "Verify certificate life time, false ...\n";
        }
        if(CheckCertWithCrl())
        {
            message+= noTime() + "Verify certificate with CRL, ok ...\n";
        }
        else
        {
            QMessageBox::warning(this,"警告","证书已经被撤销！","确定");
            message+= noTime() + "Verify certificate with CRL, false ...\n";
        }
    }
    showMessage();
}

// (～￣▽￣)→))*￣▽￣*)o主要用来签名
void MainWindow::on_pushButton_2_clicked()
{
    int serial;     //证书编号
    int day;        //申请天数
    char name1[100];//申请文件名
    char name2[100];//签发证书名
    char name3[100];//子证书私钥
    strcpy(name1,(fname+".csr").toStdString().c_str());
    strcpy(name2,(fname+".crt").toStdString().c_str());
    strcpy(name3,(fname+".key").toStdString().c_str());
    day = ui->lineEdit_8->text().toInt();

    ifstream infile;
    infile.open("sign.txt");
    if(!infile)
    {
        QMessageBox::information(NULL,"error","open this file failed\n");
    }
    else
    {
        infile >> serial;
        infile.close();
    }

    if(CreateCertFromRequestFile(serial,day,name1,name2,name3,3))
    {
        ofstream outfile;
        outfile.open("sign.txt");
        serial += 1;
        outfile << serial;
        outfile.close();
        message += getTime() + "signature success\n";
        showMessage();
    }
    else
    {
        message+="signature failed\n";
        showMessage();
    }

    //测试输出
    //detail();
}

QString MainWindow::getTime()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("[hh:mm:ss]  ");
    return current_date;
}
QString MainWindow::noTime()
{
    return "[        ]  ";
}

//生成证书撤销链
void MainWindow::on_pushButton_3_clicked()
{
    X509 *rootCert = NULL;          //根证书
    EVP_PKEY *pkey;
    BIO *b;                         //接收证书等待格式化
    b=BIO_new_file("rootca1.crt","r");
    if(b==NULL)
    {
        QMessageBox::information(NULL,"Error","Load rootca1.crt failed!\n");
        message += getTime() + "Load CA(rootca1.crt) failed! Please make sure file exist.\n";
        showMessage();
        BIO_free(b);
        return;
    }
    else
    {
        rootCert = PEM_read_bio_X509(b,NULL,NULL,NULL);
        b=NULL;
    }
    verify.rootCert=rootCert;
    b = BIO_new_file("rootca1.key", "r");
    if(b==NULL)
    {
        QMessageBox::information(NULL,"Error","Load CRL.crl failed!\n");
        message += getTime() + "Load CRL.crl failed! Please make sure file exist.\n";
        showMessage();
        BIO_free(b);
        return;
    }
    else
    {
        pkey=PEM_read_bio_PrivateKey(b,NULL,NULL,NULL);
        b=NULL;
    }
    verify.pkey=pkey;
    if(Crl()>0)
    {
        QMessageBox::information(NULL,"Sucess","Create CRL Sucess!\n");
        message += getTime() + "Create CRL sucessed!\n";
        showMessage();
    }
    else
    {
        QMessageBox::information(NULL,"Fail","Create CRL Failed!\n");
        message += getTime() + "Create CRL failed!\n";
        showMessage();
    }
}

//撤销证书
void MainWindow::on_pushButton_4_clicked()
{
    /*
    CreateCrl();*/
    if(Revoked_Load_Cer()<=0)
    {
        QMessageBox::information(this,"Error","Revoked_Load_Cer failed","确定");
        return;
    }
    QString strtmp=ui->lineEdit_9->text();
    verify.ser=strtmp;
    if(strtmp==NULL)
        QMessageBox::warning(this,"警告","请输入证书序列号！","确定");
    else
        if(revokedCert())
            QMessageBox::information(this,"提示","撤销成功！","确定");
        else
            QMessageBox::information(this,"提示","撤销失败！","确定");
}

//选择证书请求文件
void MainWindow::on_pushButton_5_clicked()
{
    QFileInfo fileinfo;
    //获取文件绝对路径
    QString absurl = QFileDialog::getOpenFileName(this,"select file","./","*.csr");
    if (absurl.isNull())
    {
        QMessageBox::warning(NULL,"error","Select file failed!\n");
        message += getTime() + "Select file failed!\n";
        showMessage();
    }
    else
    {
        QString filename;
        fileinfo = QFileInfo(absurl);
        //获取文件名
        filename = fileinfo.fileName();
        ui->lineEdit_10->setText(filename);
        //除去后缀名
        int index = filename.lastIndexOf(".");
        filename.truncate(index);
        fname = filename;
        message += getTime() + "select request file " + fname + " success\n";
        showMessage();
    }
}
