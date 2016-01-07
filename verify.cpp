#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>

///
/// \brief MainWindow::Load_Cer
/// 从文件读取证书
/// \return -1 --> ca cert empety
/// \return -2 --> user cert empety
/// \return -4 --> private key empty
/// \return 0 --> ok
///
int MainWindow::Load_Cer()
{
    X509 *userCert1 = NULL;          //用户1
    X509 *rootCert = NULL;          //根证书
    BIO *b;                         //接收证书等待格式化
//    X509_CRL *Crl = NULL;           //证书撤销链表
    EVP_PKEY *pkey=NULL;

    b=BIO_new_file("rootca1.crt","r");
    if(b==NULL)
    {
        return -1;
    }
    rootCert = PEM_read_bio_X509(b,NULL,NULL,NULL);
    b=BIO_new_file(verify.userCerUrl.toStdString().data(),"r");
    if(b==NULL)
    {
        return -2;
    }
    userCert1=PEM_read_bio_X509(b,NULL,NULL,NULL);
//    b=BIO_new_file("Crl.crl","r");
//    if(b==NULL)
//    {
//        return -3;
//    }
//    Crl=PEM_read_bio_X509_CRL(b,NULL,NULL,NULL);
    b = BIO_new_file("rootca1.key", "r");
    if(b == NULL)
    {
        return -4;
    }

    pkey = PEM_read_bio_PrivateKey(b, NULL, 0, NULL);
    BIO_free(b);
    verify.rootCert=rootCert;
    verify.userCert1=userCert1;
    verify.pkey=pkey;
    //verify.Crl=Crl;
    return 0;
}
///
/// \brief CheckCertWithRoot
/// 使用根证书验证证书
/// \return true or false
///
bool MainWindow::CheckCertWithRoot()
{
    OpenSSL_add_all_algorithms();
    X509 *x509 = verify.userCert1;
    X509 *root = verify.rootCert;

    EVP_PKEY * pcert=X509_get_pubkey(root);
    int ret=X509_verify(x509,pcert);
    EVP_PKEY_free (pcert);

    if(ret==1)
        return true;
    else
        return false;
}
///
/// \brief MainWindow::GetCertSerialNumber
/// display certificate SerialNumber
/// \return QString or null
///
QString MainWindow::GetCertSerialNumber()
{
    X509 *x509= verify.userCert1;
    char * stringval = i2s_ASN1_INTEGER(NULL,X509_get_serialNumber(x509));
    QString str(stringval);
    return str;
}
///
/// \brief GetCertSubjectString
/// 获取证书的主题信息（全部信息），返回主题的字符串形式
/// crash while read certificate name
/// \return QString
///
QString MainWindow::GetCertSubjectString()
{
    char buf[256];
    memset(buf,0,256);
    X509 *x509= verify.userCert1;
    int fn_nid;
    X509_NAME *name;
    ASN1_OBJECT *obj;
    char objtmp[80];
    const char *objbuf;
    X509_NAME_ENTRY *entry;
    setlocale(LC_CTYPE, "");
    name=X509_get_subject_name(x509);
    int num=X509_NAME_entry_count(name);
    for(int i=0;i<num;i++)
    {
        entry=(X509_NAME_ENTRY *)X509_NAME_get_entry(name,0);
        obj=X509_NAME_ENTRY_get_object(entry);
        fn_nid = OBJ_obj2nid(obj);
        if(fn_nid==NID_undef)
            OBJ_obj2txt(objtmp, sizeof objtmp, obj, i);
        else
        {
            objbuf = OBJ_nid2sn(fn_nid);
            strcpy(objtmp,objbuf);
        }
    }
    QString st=buf;
    return st;
}
///
/// \brief MainWindow::CheckCertTime
/// crash while read certificate date
/// \return
///
bool MainWindow::CheckCertTime()
{
    bool bf;
    X509 *x509=verify.userCert1;
    QDateTime qtime = QDateTime::currentDateTime();
    time_t ct=qtime.toTime_t();
    asn1_string_st *before=X509_get_notBefore(x509),*after=X509_get_notAfter(x509);
    ASN1_UTCTIME *be=ASN1_STRING_dup(before),*af=ASN1_STRING_dup(after);
    if(ASN1_UTCTIME_cmp_time_t(be,ct)>=0||ASN1_UTCTIME_cmp_time_t(af,ct)<=0)
        bf=false;
    else
        bf=true;
    M_ASN1_UTCTIME_free(be);
    M_ASN1_UTCTIME_free(af);
    return bf;
}

///
/// \brief MainWindow::CheckCertWithCrl
/// 通过黑名单验证证书，验证通过返回真，否则返回假
/// \return ture or false
///
bool MainWindow::CheckCertWithCrl()
{
    X509 *x509=verify.userCert1;
    X509_CRL *crl=verify.Crl;
    STACK_OF(X509_REVOKED) *revoked=crl->crl->revoked;
    X509_REVOKED *rc;

    ASN1_INTEGER *serial=X509_get_serialNumber(x509);
    int num=sk_X509_REVOKED_num(revoked);
    bool bf=true;
    for(int i=0;i<num;i++)
    {
        rc=sk_X509_REVOKED_pop(revoked);
        if(ASN1_INTEGER_cmp(serial,rc->serialNumber)==0)
            bf=false;
    }
    X509_CRL_free(crl);
    X509_free(x509);
    EVP_cleanup();
    return bf;
}

