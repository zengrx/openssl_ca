#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <fstream>
#include <QFile>
#include <QTextStream>
#include <QJsonArray>

///
/// \brief MainWindow::revokedCert
/// 撤销证书
/// \return
///
bool MainWindow::revokedCert()
{
    BIO  *bpint=NULL;
    if(Revoked_Load()<0)
        QMessageBox::warning(this,"警告","缺少程序必须文件!\n","确定");
    time_t t;
    ASN1_TIME *rvTime;
    X509_REVOKED  *revoked;
    ASN1_INTEGER   *serial;
    if(verify.Crl==NULL)
    {
        bpint=BIO_new_file("CRL.crl","r");
        if(bpint==NULL)
        {
            if(!CreateCrl())
            {
                QMessageBox::information(this,"Error","Create CRL.crl failed!\n");
                BIO_free(bpint);
                return false;
            }
        }
        else
        {
            verify.Crl=PEM_read_bio_X509_CRL(bpint,NULL,NULL,NULL);
        }
    }
    /* 添加被撤销证书序列号*/
    revoked=X509_REVOKED_new();
    serial=ASN1_INTEGER_new();
    ASN1_INTEGER_set(serial,verify.ser.toLong());
    if(!CheckSerialWithCrl(serial))    //验证是否重复撤销
    {
        QMessageBox::information(this,"提示","证书已经被吊销，请勿重复吊销!\n","确定");
        BIO_free(bpint);
        return false;
    }
    if(X509_REVOKED_set_serialNumber(revoked,serial))
    {
        rvTime=ASN1_TIME_new();
        t=time(NULL);
        ASN1_TIME_set(rvTime,t+2000);
        X509_CRL_set_nextUpdate(verify.Crl,rvTime);
        X509_REVOKED_set_revocationDate(revoked,rvTime);
        if(X509_CRL_add0_revoked(verify.Crl,revoked))
        {
            X509_CRL_sort(verify.Crl);// 排序
            bpint=BIO_new_file("CRL.crl","wb");
            PEM_write_bio_X509_CRL(bpint,verify.Crl);
            BIO_free(bpint);
            return true;
        }
        else
            return false;
    }
    return false;
}

///
/// \brief MainWindow::Revoked_Load_Cer
/// 从文件读取PEM证书
/// \return -1 --> ca cert empety
/// \return -4 --> private key empty
/// \return 1 --> ok
///
int MainWindow::Revoked_Load()
{
    X509 *rootCert = NULL;          //根证书
    BIO *b;                         //接收文件等待格式化
    EVP_PKEY *pkey=NULL;

    b=BIO_new_file("rootca1.crt","r");
    if(b==NULL)
    {
        BIO_free(b);
        return -1;
    }
    else
    {
        rootCert = PEM_read_bio_X509(b,NULL,NULL,NULL);
        b=NULL;
    }
    b = BIO_new_file("rootca1.key", "r");
    if(b == NULL)
    {
        BIO_free(b);
        return -4;
    }
    else
    {
        pkey = PEM_read_bio_PrivateKey(b, NULL, 0, NULL);
    }
    BIO_free(b);
    verify.rootCert=rootCert;
    verify.pkey=pkey;
    return 1;
}

///
/// \brief MainWindow::CreateCrl
/// 生成证书撤销链
/// \return true or false
///
bool MainWindow::CreateCrl()
{
    time_t t;
    X509_NAME *issuer;
    ASN1_TIME *lastUpdate,*nextUpdate;
    X509_CRL  *crl=NULL;
    EVP_PKEY *pkey;
    BIO  *bp;
    X509 *x509;
    Revoked_Load();
    pkey=verify.pkey;
    x509=verify.rootCert;
    crl=X509_CRL_new();
    /* 设置版本*/
    X509_CRL_set_version(crl,3);
    /* 设置颁发者*/
    issuer=X509_NAME_dup(x509->cert_info->issuer);
    X509_CRL_set_issuer_name(crl,issuer);
    /* 设置上次发布时间*/
    lastUpdate=ASN1_TIME_new();
    t=time(NULL);
    ASN1_TIME_set(lastUpdate,t);
    X509_CRL_set_lastUpdate(crl,lastUpdate);
    /* 设置下次发布时间*/
    nextUpdate=ASN1_TIME_new();
    t=time(NULL);
    ASN1_TIME_set(nextUpdate,t+1000);
    X509_CRL_set_nextUpdate(crl,nextUpdate);
    /* 签名*/
    X509_CRL_sign(crl,pkey,EVP_md5());
    /* 写入文件*/
    bp=BIO_new_file("CRL.crl","wb");
    PEM_write_bio_X509_CRL(bp,crl);
    verify.Crl=crl;
    BIO_free(bp);
    return true;
}

//初始化证书撤销列表
void MainWindow::Init_DisCRL()
{
    //存储初始化函数中消息
    BIO *biof=NULL;                         //接收CRL等待格式化
    if(verify.Crl==NULL)
    {
        biof=BIO_new_file("CRL.crl","r");
        if(biof==NULL)
        {
            BIO_free(biof);
            return;
        }
        else
        {
            verify.Crl=PEM_read_bio_X509_CRL(biof,NULL,NULL,NULL);
        }
    }
    STACK_OF(X509_REVOKED) *revoked=verify.Crl->crl->revoked;
    int num=sk_X509_REVOKED_num(revoked);
    X509_REVOKED *rc=NULL;
    ui->listWidget->clear();
    ui->listWidget->addItem("序号\t撤销序列号\t证书撤销时间");
    for(int i=0;i<num;i++)
    {
        rc=sk_X509_REVOKED_value(revoked,i);
        asn1_string_st *revTime=rc->revocationDate;
        ASN1_TIME *rt=ASN1_STRING_dup(revTime);
        time_t tt=ASN1_GetTimeT(rt);
        QDateTime dt = QDateTime::fromTime_t(tt);
        ui->listWidget->addItem(QString::number(i+1)+'\t'+i2s_ASN1_INTEGER(NULL,rc->serialNumber)+"\t"+dt.toString(Qt::TextDate));
    }
    BIO_free(biof);
}

void MainWindow::DisCRL()
{
    //存储初始化函数中消息
    BIO *biof=NULL;                         //接收CRL等待格式化
    if(verify.Crl==NULL)
    {
        biof=BIO_new_file("CRL.crl","r");
        if(biof==NULL)
        {
            QMessageBox::information(this,"提示","没有证书吊销列表文件!\n");
            ui->textEdit->append(getTime()+"Load CRL.crl failed! Please make sure file exist.\n");
            BIO_free(biof);
            return;
        }
        else
        {
            verify.Crl=PEM_read_bio_X509_CRL(biof,NULL,NULL,NULL);
        }
    }
    STACK_OF(X509_REVOKED) *revoked=verify.Crl->crl->revoked;
    int num=sk_X509_REVOKED_num(revoked);
    X509_REVOKED *rc=NULL;
    ui->listWidget->clear();
    ui->listWidget->addItem("序号\t撤销序列号\t证书撤销时间");
    for(int i=0;i<num;i++)
    {
        rc=sk_X509_REVOKED_value(revoked,i);
        asn1_string_st *revTime=rc->revocationDate;
        ASN1_TIME *rt=ASN1_STRING_dup(revTime);
        time_t tt=ASN1_GetTimeT(rt);
        QDateTime dt = QDateTime::fromTime_t(tt);
        ui->listWidget->addItem(QString::number(i+1)+'\t'+i2s_ASN1_INTEGER(NULL,rc->serialNumber)+"\t"+dt.toString(Qt::TextDate));
    }
    BIO_free(biof);
}

//change ASN1_Time to time_t
//copy from http://stackoverflow.com/questions/10975542/asn1-time-to-time-t-conversion
//copy date 2016.2.1
time_t MainWindow::ASN1_GetTimeT(ASN1_TIME* time)
{
    struct tm t;
    const char* str = (const char*) time->data;
    size_t i = 0;
    memset(&t, 0, sizeof(t));
    if (time->type == V_ASN1_UTCTIME) {/* two digit year */
        t.tm_year = (str[i++] - '0') * 10;
        t.tm_year += (str[i++] - '0');
        if (t.tm_year < 70)
            t.tm_year += 100;
    } else if (time->type == V_ASN1_GENERALIZEDTIME) {/* four digit year */
        t.tm_year = (str[i++] - '0') * 1000;
        t.tm_year+= (str[i++] - '0') * 100;
        t.tm_year+= (str[i++] - '0') * 10;
        t.tm_year+= (str[i++] - '0');
        t.tm_year -= 1900;
    }
    t.tm_mon  = (str[i++] - '0') * 10;
    t.tm_mon += (str[i++] - '0') - 1; // -1 since January is 0 not 1.
    t.tm_mday = (str[i++] - '0') * 10;
    t.tm_mday+= (str[i++] - '0');
    t.tm_hour = (str[i++] - '0') * 10;
    t.tm_hour+= (str[i++] - '0');
    t.tm_min  = (str[i++] - '0') * 10;
    t.tm_min += (str[i++] - '0');
    t.tm_sec  = (str[i++] - '0') * 10;
    t.tm_sec += (str[i++] - '0');
    /* Note: we did not adjust the time based on time zone information */
    return mktime(&t);
}

//Delete CRL item one by one
bool MainWindow::DeleteCRLItem()
{
    int iptr=indexPtr;//Bug maybe occurs here,indexPtr will begin a big num
    QString CRLserial;
    STACK_OF(X509_REVOKED) *revoked=verify.Crl->crl->revoked;
    X509_REVOKED *rc=sk_X509_REVOKED_value(revoked,iptr);
    CRLserial = i2s_ASN1_INTEGER(NULL,rc->serialNumber);
    ReadJson(signlistjson);
    if(signlistjson.isEmpty())
    {

        qWarning("Error: json empty.");
        return false;
    }
    QJsonArray SignArray = signlistjson["signlist"].toArray();
    for(int i=0;i<SignArray.size();i++)
    {
        QJsonObject objson=SignArray[i].toObject();
        if(objson["serialNumber"]==CRLserial.toInt())
        {
            objson["stats"]=false;
            SignArray[i]=objson;
            signlistjson["signlist"]=SignArray;
            SaveJson(signlistjson);
        }
    }
    UpdataListWidget2();
    BIO *bp=NULL;
    if(verify.Crl==NULL||indexPtr<0)
        return false;
    sk_X509_REVOKED_delete(verify.Crl->crl->revoked,indexPtr);
    X509_CRL_sort(verify.Crl);// 排序
    bp=BIO_new_file("CRL.crl","wb");
    PEM_write_bio_X509_CRL(bp,verify.Crl);
    DisCRL();
    BIO_free(bp);
    indexPtr=-1;
    return true;
}


///
/// \brief MainWindow::UpdataListWidget2
/// function: display sign list info to ui ListWidget2
/// use var: class member signlistjson
/// effect: change json file "stats"
/// \return true or false
///
bool MainWindow::UpdataListWidget2()
{
    if(false==ReadJson(signlistjson))
        return false;
    ui->listWidget_2->clear();
    ui->listWidget_2->addItem("序列号\t发布时间\t\t是否吊销");
    QJsonArray SignArray = signlistjson["signlist"].toArray();
    for(int i=0;i<SignArray.size();i++)
    {
        QJsonObject objson=SignArray[i].toObject();
        QString list;
        if(true==objson["stats"].toBool())
            list = QString::number(objson["serialNumber"].toInt())
                +"\t"+objson["time"].toString()+"\t吊销";
        else
            list = QString::number(objson["serialNumber"].toInt())
                +"\t"+objson["time"].toString()+"\t没有";
        ui->listWidget_2->addItem(list);
    }
    return true;
}
