#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

#include <openssl/x509.h>
#include <openssl/x509v3.h>

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
