#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

#include <openssl/x509.h>
#include <openssl/x509v3.h>

///
/// \brief MainWindow::revokedCert
/// 撤销证书
/// \return
///
bool MainWindow::revokedCert()
{
    BIO  *bp;
    Revoked_Load_Cer();
    time_t t;
    ASN1_TIME *rvTime;
    X509_REVOKED  *revoked;
    ASN1_INTEGER   *serial;
    /* 添加被撤销证书序列号*/
    revoked=X509_REVOKED_new();
    serial=ASN1_INTEGER_new();
    ASN1_INTEGER_set(serial,verify.ser.toLong());
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
            bp=BIO_new_file("CRL.crl","wb");
            PEM_write_bio_X509_CRL(bp,verify.Crl);
            BIO_free(bp);
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
int MainWindow::Revoked_Load_Cer()
{
    X509 *rootCert = NULL;          //根证书
    BIO *b;                         //接收文件等待格式化
    X509_CRL *Crl = NULL;           //证书撤销链表
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
    b=BIO_new_file("CRL.crl","r");
    if(b==NULL)
    {
        BIO_free(b);
        return -3;
    }
    else
    {
        Crl=PEM_read_bio_X509_CRL(b,NULL,NULL,NULL);
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
    verify.Crl=Crl;
    return 1;
}

///
/// \brief MainWindow::CreateCrl
/// 生成证书撤销链（没有使用）
/// \return true or false
///
bool MainWindow::CreateCrl()
{
//    int ret=1;
    X509 *rootCert = NULL;          //根证书
    BIO *b;                         //接收证书等待格式化
    b=BIO_new_file("rootca1.crt","r");
    if(b==NULL)
    {
        BIO_free(b);
        return false;
    }
    else
        rootCert = PEM_read_bio_X509(b,NULL,NULL,NULL);
    verify.rootCert=rootCert;
    char *FileName="CRL.crl";
    int hours=QDateTime::currentDateTime().time().hour();
    OpenSSL_add_all_algorithms();
    EVP_PKEY *pkey=verify.pkey;
    X509 *x509=verify.rootCert;
    const EVP_MD *dgst=EVP_get_digestbyname("sha1");
    X509_CRL *crl=X509_CRL_new();
    X509_CRL_INFO *ci =crl->crl;
    X509_NAME_free(ci->issuer);
    ci->issuer=X509_NAME_dup(x509->cert_info->subject);
    X509_gmtime_adj(ci->lastUpdate,0);
    if (ci->nextUpdate == NULL)
        ci->nextUpdate=ASN1_UTCTIME_new();
    X509_gmtime_adj(ci->nextUpdate,hours*60*60);
    if(!ci->revoked)
        ci->revoked = sk_X509_REVOKED_new_null();
    /*
    int i=0;
    X509_REVOKED *r = NULL;
    BIGNUM *serial_bn = NULL;
    char buf[512];
    for(i=0;i<certNum;i++)
    {
        r = X509_REVOKED_new();
        ASN1_TIME_set(r->revocationDate,crlInfo[i].RevokeTime);
        BN_hex2bn(&serial_bn,ltoa(crlInfo[i].CertSerial,buf,10));
        BN_to_ASN1_INTEGER(serial_bn,r->serialNumber);
        sk_X509_REVOKED_push(ci->revoked,r);
    }
    for (i=0; i<sk_X509_REVOKED_num(ci->revoked); i++)
    {
        r=sk_X509_REVOKED_value(ci->revoked,i);
        r->sequence=i;
    }
*/

    ci->version=ASN1_INTEGER_new();
    ASN1_INTEGER_set(ci->version,1);
    X509_CRL_sign(crl,pkey,dgst);
    BIO *out=BIO_new(BIO_s_file());
    if(BIO_write_filename(out,FileName) > 0)
    {
        PEM_write_bio_X509_CRL(out,crl);
    }
    X509V3_EXT_cleanup();
    BIO_free_all(out);
    EVP_PKEY_free(pkey);
    X509_CRL_free(crl);
    X509_free(x509);
    EVP_cleanup();
    return true;
}

struct stuREVOKE//证书作废结构链表
{
    int Index;//证书序号
    time_t time;//吊销时间
    stuREVOKE * Link;
    stuREVOKE()
    {
        memset(this,0,sizeof(stuREVOKE));
    }
    stuREVOKE(int index,time_t t)
    {
        Index=index;
        time=t;;
        Link=NULL;
    }
};

void AddRevoke(stuREVOKE *& Head,int index,time_t time)
{
    stuREVOKE * End=new stuREVOKE(index,time);//钥增加的节点
    if(Head==NULL)
    {
        Head=End;
    }
    else
    {
        stuREVOKE * p=Head;
        while(p->Link!=NULL)
            p=p->Link;
        p->Link=End;
    }
    return;
}

//初始化证书撤销列表
void MainWindow::Init_DisCRL()
{
    //存储初始化函数中消息
    QString rvkinit;
    BIO *b;                         //接收CRL等待格式化
    if(verify.Crl==NULL)
    {
        b=BIO_new_file("CRL.crl","r");
        if(b==NULL)
        {
            QMessageBox::information(this,"Error","Load CRL.crl failed!\n");
            //message += getTime() + "Load CRL.crl failed! Please make sure file exist.\n";
            ui->textEdit->append(getTime()+"Load CRL.crl failed! Please make sure file exist.\n");
            showMessage();
            BIO_free(b);
            return;
        }
        else
        {
            verify.Crl=PEM_read_bio_X509_CRL(b,NULL,NULL,NULL);
        }
    }
    STACK_OF(X509_REVOKED) *revoked=verify.Crl->crl->revoked;
    int num=sk_X509_REVOKED_num(revoked);
    X509_REVOKED *rc;
    ui->listWidget->clear();
    ui->listWidget->addItem("序号\t撤销序列号\t撤销时间");
    rvkinit+=getTime()+"序号\t撤销序列号\t撤销时间\n";
    for(int i=0;i<num;i++)
    {
        rc=sk_X509_REVOKED_value(revoked,i);
        asn1_string_st *revTime=rc->revocationDate;
        ASN1_TIME *rt=ASN1_STRING_dup(revTime);
        time_t tt=ASN1_GetTimeT(rt);
        QDateTime dt = QDateTime::fromTime_t(tt);
        ui->listWidget->addItem(QString::number(i)+'\t'+i2s_ASN1_INTEGER(NULL,rc->serialNumber)+"\t"+dt.toString(Qt::TextDate));
        rvkinit+=getTime()+QString::number(i)+"   \t"+i2s_ASN1_INTEGER(NULL,rc->serialNumber)+"\t"+dt.toString(Qt::TextDate)+'\n';
    }
    ui->textEdit->append(rvkinit);
    showMessage();
    BIO_free(b);
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
