#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>

#include <QJsonArray>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    bits=512;
    indexPtr=-1;
    ui->setupUi(this);
    setFixedSize(722,481);
    QRegExp regx("[0-9]{10}$");
    QValidator *validator=new QRegExpValidator(regx,ui->lineEdit_9);
    ui->lineEdit_9->setValidator(validator);
    ui->lineEdit_9->setPlaceholderText("需要吊销的证书序列号");
    ui->lineEdit_10->setPlaceholderText("证书请求文件名");
    ui->pushButton_9->setDisabled(true);
    ui->pushButton_11->setDisabled(true);
    ui->pushButton_3->setVisible(false);
    ui->pushButton_6->setVisible(false);
    ui->pushButton_10->setVisible(false);
    ui->pushButton_12->setDisabled(false);
    ui->pushButton_13->setDisabled(true);
    Init_DisCRL();
    UpdataListWidget2();
//    LoadSignFile(&queue);
}

MainWindow::~MainWindow()
{
    delete ui;
    /*没有载入相应内容在关闭应用时会crash*/
//    BIO_free(b);
//    X509_free(x509);
//    X509_free(verify.rootCert);
//    X509_free(verify.userCert1);
//    X509_CRL_free(verify.Crl);
//    EVP_PKEY_free(verify.pkey);
//    EVP_PKEY_free(pkey);
//    EVP_cleanup();
}

//证书请求文件按钮事件
void MainWindow::on_pushButton_clicked()
{
    //执行请求文件
    careq();
    //写入message
    //showMessage();
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
            //message1保存CA问文件载入信息，message2保存操作后信息
            QString message1,message2;
            message1 += getTime() + "rootCert loaded ...\n";
            message1 += getTime() + "userCert loaded ...\n";
            message1 += getTime() + "PrivateKey loaded ...\n";
            showMessage();
            ui->textEdit->append(message1);
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
                message2 += getTime() + "Certificate Detail:";
                QString tmpstr_1 = GetCertSerialNumber(verify.userCert1);
                if(!tmpstr_1.isNull())
                {
                    message2 += "\n" + noTime() + "SerialNumber: ";
                    message2 += tmpstr_1;
                }
                message2 += tmpstr_0;
                message2 += "\n";
            }
            ui->textEdit->append(message2);
            showMessage();
        }
        else
        {
            ui->textEdit->append(getTime()+"Load file faild!\n");
            showMessage();
        }
    }
}

//验证证书
void MainWindow::on_pushButton_8_clicked()
{
    bool ret_check=true;
    if (verify.userCerUrl==NULL)
    {
        QMessageBox::warning(this,"警告","请选择证书！","确定");
        return;
    }
    else
    {
        if(CheckCertWithRoot())
        {
            ui->textEdit->append(getTime()+"Verify with ca, ok ...\n");
        }
        else
        {
            ret_check=false;
            QMessageBox::warning(this,"警告","不受根证书信任的证书！","确定");
            ui->textEdit->append(getTime()+"Verify with ca, false ...\n");
        }
        if(CheckCertTime())
        {
            ui->textEdit->append(getTime()+"Verify certificate life time, ok ...\n");
        }
        else
        {
            ret_check=false;
            QMessageBox::warning(this,"警告","证书过期！","确定");
            ui->textEdit->append(getTime()+"Verify certificate life time, false ...\n");
        }
        if(CheckCertWithCrl())
        {
            ui->textEdit->append(getTime()+"Verify certificate with CRL, ok ...\n");
        }
        else
        {
            ret_check=false;
            QMessageBox::warning(this,"警告","证书已经被撤销！","确定");
            ui->textEdit->append(getTime()+"Verify certificate with CRL, false ...\n");
        }
        if(ret_check)
            QMessageBox::information(this,"提示","证书通过验证","确定");
    }
    showMessage();
}

// (～￣▽￣)→))*￣▽￣*)o主要用来签名
void MainWindow::on_pushButton_2_clicked()
{
    QString msgout;
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
        //+++++++++++++++++++++++++++++++++++++++++++++++
        //append by Qool in order to write serial to file
        if(WriteSerial2Json(serial))
            UpdataListWidget2();
        else
            qWarning("Write json failed.");
        //+++++++++++++++++++++++++++++++++++++++++++++++
        ofstream outfile;
        outfile.open("sign.txt");
        serial += 1;
        outfile << serial;
        outfile.close();
        msgout = getTime() + "signature success\n";
    }
    else
    {
        msgout = getTime() + "signature failed\n";
    }

    ui->textEdit->append(msgout);
    showMessage();

    //测试输出
    //detail();
}

//格式化消息流
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
    QString rvkmsg;
    X509 *rootCert = NULL;          //根证书
    EVP_PKEY *pkey;
    BIO *b;                         //接收证书等待格式化
    b=BIO_new_file("rootca1.crt","r");
    if(b==NULL)
    {
        QMessageBox::information(NULL,"Error","Load rootca1.crt failed!\n");
        ui->textEdit->append(getTime() + "Load CA(rootca1.crt) failed! Please make sure file exist.\n");
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
        ui->textEdit->append(getTime() + "Load CRL.crl failed! Please make sure file exist.\n");
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
    if(CreateCrl()>0)
    {
        QMessageBox::information(NULL,"Sucess","Create CRL Sucess!\n");
        ui->textEdit->append(getTime() + "Create CRL sucessed!\n");
        showMessage();
    }
    else
    {
        QMessageBox::information(NULL,"Fail","Create CRL Failed!\n");
        ui->textEdit->append(getTime() + "Create CRL failed!\n");
        showMessage();
    }
}

//撤销证书
void MainWindow::on_pushButton_4_clicked()
{
    if(Revoked_Load()<=0)
    {
        QMessageBox::warning(this,"警告","缺少程序依赖文件","确定");
        return;
    }
    QString strtmp=ui->lineEdit_9->text();
    if(strtmp==NULL||strtmp=="0")
        QMessageBox::warning(this,"警告","请输入有效的证书序列号！","确定");
    else
    {
        verify.ser=strtmp;
        if(revokedCert())
        {
            QMessageBox::information(this,"提示","证书吊销成功！","确定");
            DisCRL();
            queue.clear();
            ReadJson(signlistjson);
            if(signlistjson.isEmpty())
            {
                qWarning("Error: json empty.");
                return;
            }
            QJsonArray array = signlistjson["signlist"].toArray();
            QJsonObject JsonSerial = array[strtmp.toInt()].toObject();
            JsonSerial["stats"]=true;
            array[strtmp.toInt()]=JsonSerial;
            signlistjson["signlist"]=array;
            SaveJson(signlistjson);
            UpdataListWidget2();
        }
        else
            QMessageBox::information(this,"提示","证书吊销失败！","确定");
    }
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
        ui->textEdit->append(getTime() + "Select file failed!\n");
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
        ui->textEdit->append(getTime() + "select request file " + fname + " success\n");
        showMessage();
    }
}

//显示撤销的证书列表
void MainWindow::on_pushButton_6_clicked()
{
    DisCRL();
}

//Select serial num by list row change
void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    indexPtr = currentRow-1;
    ui->pushButton_9->setDisabled(false);
}

//Undo revoked
void MainWindow::on_pushButton_9_clicked()
{
    if(DeleteCRLItem())
        QMessageBox::information(this,"提示","恢复证书成功！");
    else
        QMessageBox::information(this,"提示","恢复失败,请选择需要恢复的证书！");
    ui->pushButton_9->setDisabled(true);
}

void MainWindow::on_listWidget_2_currentRowChanged(int currentRow)
{
    indexPtr = currentRow-1;
    ui->pushButton_11->setDisabled(false);
}

void MainWindow::on_pushButton_11_clicked()
{
    ReadJson(signlistjson);
    if(signlistjson.isEmpty())
    {
        qWarning("Error: json empty.");
        return;
    }
    QJsonArray ArrayJson = signlistjson["signlist"].toArray();
    QJsonObject objson = ArrayJson[indexPtr].toObject();
    verify.ser = QString::number(objson["serialNumber"].toInt());
    if(verify.ser.isEmpty()||verify.ser.isNull())
    {
        QMessageBox::information(this,"提示","证书序列号为空！","确定");
        return;
    }
    if(revokedCert())
    {
        objson["stats"]=true;
        ArrayJson[indexPtr]=objson;
        signlistjson["signlist"]=ArrayJson;
        SaveJson(signlistjson);
        UpdataListWidget2();
        DisCRL();
        QMessageBox::information(this,"提示","证书吊销成功！","确定");
    }
    else
        QMessageBox::information(this,"提示","证书吊销失败！","确定");
    verify.ser=-1;
    ui->pushButton_11->setDisabled(true);
}

void MainWindow::on_pushButton_12_clicked()
{
        ui->pushButton_3->setVisible(true);
        ui->pushButton_6->setVisible(true);
        ui->pushButton_10->setVisible(true);
        ui->pushButton_12->setDisabled(true);
        ui->pushButton_13->setDisabled(false);
}

void MainWindow::on_pushButton_13_clicked()
{
    ui->pushButton_3->setVisible(false);
    ui->pushButton_6->setVisible(false);
    ui->pushButton_10->setVisible(false);
    ui->pushButton_12->setDisabled(false);
    ui->pushButton_13->setDisabled(true);
}
