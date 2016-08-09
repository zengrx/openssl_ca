#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <openssl/x509v3.h>

////
/// \brief MainWindow::revokeCert
/// \return true or false
/// 撤销证书事件函数
///
bool MainWindow::revokeCert()
{
    int ret;               //接收返回值
    BIO *bpint = NULL;     //BIO大数对象
    char name1[100];       //局部变量 数据类型转换中间值
    X509 *x509;            //x509根证书对象
    EVP_PKEY *pkey;        //根证书密钥对象
    time_t t;              //time_t类型时间
    ASN1_TIME *rvtime;     //用户证书被撤销时间
    X509_REVOKED *revoked; //
    ASN1_INTEGER *serial;  //
    QString r_crlname;     //局部变量 存储core路径
    loadRootCA(); //调用载入根证书信息函数
    x509 = certop.rootcert;
    pkey = certop.pkey;
    r_crlname = coredir + "Crl.crl";
    strcpy(name1,r_crlname.toStdString().c_str());
    if(certop.crl == NULL)
    {
        bpint = BIO_new_file(name1,"w");
        if(bpint == NULL)
        {
            ui->textBrowser->append(getTime() + "读取证书链失败，尝试创建");
            if(!createCrl())
            {
                ui->textBrowser->append(getTime() + "创建证书撤销链文件失败，请重试");
                return false;
            }
        }
        else
        {
            certop.crl = PEM_read_bio_X509_CRL(bpint,NULL,NULL,NULL);
        }
    }
    /*添加被撤销的证书序列号*/
    revoked = X509_REVOKED_new();
    //qDebug() << "revoked" << revoked;
    serial = ASN1_INTEGER_new();
    //qDebug() << "serial" << serial;
    ret = ASN1_INTEGER_set(serial,certop.ser.toLong());
    //qDebug() << ret << serial;
    //qDebug() << certop.ser;
//    if(!CheckSerialWithCrl(serial))    //验证是否重复撤销
//    {
//        QMessageBox::information(this,"提示","证书已经被吊销，请勿重复吊销!\n","确定");
//        BIO_free(bpint);
//        return false;
//    }
    if(X509_REVOKED_set_serialNumber(revoked,serial))
    {
        rvtime = ASN1_TIME_new();
        t = time(NULL);
        ASN1_TIME_set(rvtime,t+2000);
        X509_CRL_set_nextUpdate(certop.crl,rvtime);
        X509_REVOKED_set_revocationDate(revoked,rvtime);
        if(X509_CRL_add0_revoked(certop.crl,revoked))
        {
            qDebug() << "ok1";
            X509_CRL_sort(certop.crl);// 排序
            bpint = BIO_new_file(name1,"wb");
            //qDebug() << name1;
            PEM_write_bio_X509_CRL(bpint,certop.crl);
            BIO_free(bpint);
            return true;
        }
        else
            return false;
    }
    return false;
}

////
/// \brief MainWindow::createCrl
/// \return true or false
/// 生成根证书撤销链函数
///
bool MainWindow::createCrl()
{
    time_t t;
    X509_NAME *issuer;
    ASN1_TIME *lastUpdate, *nextUpdate;
    X509_CRL  *crl = NULL;
    EVP_PKEY *pkey;
    BIO  *bp;
    X509 *x509;
    loadRootCA();
    pkey = certop.pkey;
    x509 = certop.rootcert;
    crl = X509_CRL_new();
    /*设置版本*/
    X509_CRL_set_version(crl,3);
    /*设置颁发者*/
    issuer = X509_NAME_dup(x509->cert_info->issuer);
    X509_CRL_set_issuer_name(crl,issuer);
    /*设置上次发布时间*/
    lastUpdate = ASN1_TIME_new();
    t = time(NULL);
    ASN1_TIME_set(lastUpdate,t);
    X509_CRL_set_lastUpdate(crl,lastUpdate);
    /*设置下次发布时间*/
    nextUpdate = ASN1_TIME_new();
    t = time(NULL);
    ASN1_TIME_set(nextUpdate,t+1000);
    X509_CRL_set_nextUpdate(crl,nextUpdate);
    /*签名*/
    X509_CRL_sign(crl,pkey,EVP_md5());
    /*写入文件*/
    char name1[100]; //局部变量 数据类型转换中间值
    QString r_crlname = coredir + "Crl.crl";
    strcpy(name1,r_crlname.toStdString().c_str());
    bp=BIO_new_file(name1,"wb");
    PEM_write_bio_X509_CRL(bp,crl);
    certop.crl = crl;
    BIO_free(bp);
    return true;
}

////
/// \brief MainWindow::initCrlList
/// 初始化根证书撤销列表函数
///
void MainWindow::initCrlList()
{
    //存储初始化函数中消息
    BIO *biof = NULL; //接收CRL等待格式化
    char name1[100]; //局部变量 数据类型转换中间值
    QString r_crlname = coredir + "Crl.crl";
    strcpy(name1,r_crlname.toStdString().c_str());
    if(certop.crl == NULL)
    {
        biof=BIO_new_file(name1,"r");
        if(biof == NULL)
        {
            BIO_free(biof);
            return;
        }
        else
        {
            certop.crl=PEM_read_bio_X509_CRL(biof,NULL,NULL,NULL);
        }
    }
    STACK_OF(X509_REVOKED) *revoked;
    revoked = certop.crl->crl->revoked;
    int num = sk_X509_REVOKED_num(revoked);
    X509_REVOKED *rc = NULL;
    ui->listWidget_2->clear();
    ui->listWidget_2->addItem("序号\t撤销序列号\t证书撤销时间");
    for(int i=0; i<num; i++)
    {
        rc = sk_X509_REVOKED_value(revoked,i);
        asn1_string_st *revTime = rc->revocationDate;
        ASN1_TIME *rt = ASN1_STRING_dup(revTime);
        time_t tt = ASN1_GetTimeT(rt);
        QDateTime dt = QDateTime::fromTime_t(tt);
        ui->listWidget_2->addItem(QString::number(i+1)+'\t'+
                                  i2s_ASN1_INTEGER(NULL,rc->serialNumber)+
                                  "\t"+dt.toString(Qt::TextDate));
    }
    BIO_free(biof);
}

////
/// \brief MainWindow::showCrlInfo
/// 显示撤销链信息函数
///
void MainWindow::showCrlInfo()
{
    //存储初始化函数中消息
    BIO *biof=NULL; //接收CRL等待格式化
    char name1[100]; //局部变量 数据类型转换中间值
    QString r_crlname = coredir + "Crl.crl";
    strcpy(name1,r_crlname.toStdString().c_str());
    if(certop.crl==NULL)
    {
        biof=BIO_new_file(name1,"r");
        if(biof==NULL)
        {
            ui->textBrowser->append(getTime() + "载入撤销链失败，请确认文件存在");
            BIO_free(biof);
            return;
        }
        else
        {
            certop.crl=PEM_read_bio_X509_CRL(biof,NULL,NULL,NULL);
        }
    }
    STACK_OF(X509_REVOKED) *revoked;
    revoked = certop.crl->crl->revoked;
    int num = sk_X509_REVOKED_num(revoked);
    X509_REVOKED *rc = NULL;
    ui->listWidget->clear();
    ui->listWidget->addItem("序号\t撤销序列号\t证书撤销时间");
    for(int i=0; i<num; i++)
    {
        rc = sk_X509_REVOKED_value(revoked,i);
        asn1_string_st *revTime = rc->revocationDate;
        ASN1_TIME *rt = ASN1_STRING_dup(revTime);
        time_t tt = ASN1_GetTimeT(rt);
        QDateTime dt = QDateTime::fromTime_t(tt);
        ui->listWidget->addItem(QString::number(i+1)+'\t'+
                                i2s_ASN1_INTEGER(NULL,rc->serialNumber)+
                                "\t"+dt.toString(Qt::TextDate));
    }
    BIO_free(biof);
}

//change ASN1_Time to time_t
//copy from http://stackoverflow.com/questions/10975542/asn1-time-to-time-t-conversion
//copy date 2016.2.1
//from openssl_ca/v1.0
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


