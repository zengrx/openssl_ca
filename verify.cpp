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
/// 从文件读取PEM证书
/// \return -1 --> ca cert empety
/// \return -2 --> user cert empety
/// \return -4 --> private key empty
/// \return 1r --> ok
///
int MainWindow::Load_Cer()
{
    X509 *userCert1 = NULL;          //用户1
    X509 *rootCert = NULL;          //根证书
    BIO *b;                         //接收证书等待格式化
    X509_CRL *Crl = NULL;           //证书撤销链表
    EVP_PKEY *pkey=NULL;

    b=BIO_new_file("rootca1.crt","r");
    if(b==NULL)
    {
        QMessageBox::information(NULL,"Error","Load rootcal.crt failed!\n");
        ui->textEdit->append(getTime() + "Load rootcal.crt failed! Please make sure file exist.\n");
        showMessage();
        BIO_free(b);
        return -1;
    }
    else
    {
        rootCert = PEM_read_bio_X509(b,NULL,NULL,NULL);
        b=NULL;
    }
    b=BIO_new_file(verify.userCerUrl.toStdString().data(),"r");
    if(b==NULL)
    {
        QMessageBox::information(NULL,"Error","Load userCer failed!\n");
        ui->textEdit->append(getTime() + "Load userCer failed! Please make sure file exist.\n");
        showMessage();
        BIO_free(b);
        return -2;
    }
    else
    {
        userCert1=PEM_read_bio_X509(b,NULL,NULL,NULL);
        b=NULL;
    }
    b=BIO_new_file("CRL.crl","r");
    if(b==NULL)
    {
        QMessageBox::information(NULL,"Error","Load CRL.crl failed!\n");
        ui->textEdit->append(getTime() + "Load CRL.crl failed! Please make sure file exist.\n");
        showMessage();
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
        QMessageBox::information(NULL,"Error","Load rootca1.key failed!\n");
        ui->textEdit->append(getTime() + "Load rootca1.key failed! Please make sure file exist.\n");
        showMessage();
        BIO_free(b);
        return -4;
    }
    else
    {
        pkey = PEM_read_bio_PrivateKey(b, NULL, 0, NULL);
    }
    BIO_free(b);
    verify.rootCert=rootCert;
    verify.userCert1=userCert1;
    verify.pkey=pkey;
    verify.Crl=Crl;
    return 1;
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
QString MainWindow::GetCertSerialNumber(X509 *x509)
{
    char * stringval = i2s_ASN1_INTEGER(NULL,X509_get_serialNumber(x509));
    QString str(stringval);
    return str;
}
///
/// \brief GetCertSubjectString
/// 获取证书的主题信息（全部信息），返回主题的字符串形式
/// \return QString
///
QString MainWindow::GetCertSubjectString(certInfo *info)
{
    QString tring=NULL;
    X509_NAME *name=X509_get_subject_name(verify.userCert1);
    int num=X509_NAME_entry_count(name);
    X509_NAME_ENTRY *entry;
    ASN1_OBJECT *obj;
    ASN1_STRING *str;
    char objtmp[80]={0};
    int fn_nid;
    const char *objbuf;
    setlocale(LC_CTYPE, "");
    for(int i=0;i<num;i++)
    {
        char out[255]={0};//输出
        entry=(X509_NAME_ENTRY *)X509_NAME_get_entry(name,i);
        obj=X509_NAME_ENTRY_get_object(entry);
        str=X509_NAME_ENTRY_get_data(entry);
        fn_nid = OBJ_obj2nid(obj);
        if(fn_nid==NID_undef)
            OBJ_obj2txt(objtmp, sizeof objtmp, obj, 1);
        else
        {
            objbuf = OBJ_nid2sn(fn_nid);
            strcpy(objtmp,objbuf);
            //objbuf = OBJ_nid2ln(fn_nid);
        }
        BIO *mem = BIO_new(BIO_s_mem());
        BIO_set_close(mem, BIO_CLOSE);
        ASN1_STRING_print_ex(mem,str,ASN1_STRFLGS_ESC_QUOTE );
        BUF_MEM *bptr;
        BIO_get_mem_ptr(mem, &bptr);
        int len=bptr->length;
        char * pbuf=new char[len+1];
        memset(pbuf,0,len+1);
        memcpy(pbuf,bptr->data,len);
        strncpy(out,pbuf,strlen(pbuf));
        if (mem != NULL)
            BIO_free(mem);
        switch(i)
        {
        case 0 :
            info->country=out;
            break;
        case 1:
            info->location=out;
            break;
        case 2 :
            info->state=out;
            break;
        case 3 :
            info->organization=out;
            break;
        case 4 :
            info->organizationalUnitName=out;
            break;
        case 5 :
            info->client=out;
            break;
        case 6 :
            info->email=out;
            break;
        default:
            break;
        }
        tring+=" \n";
        tring+=noTime();
        tring+=objtmp;
        tring+=":\t";
        tring+=out;
        delete [] pbuf;
    }
    return tring;
}
///
/// \brief MainWindow::CheckCertTime
/// \return ture or false
///
bool MainWindow::CheckCertTime()
{
    X509 *x509=verify.userCert1;
    time_t ct=QDateTime::currentDateTime().toTime_t();
    asn1_string_st *before=X509_get_notBefore(x509),*after=X509_get_notAfter(x509);
    ASN1_UTCTIME *be=ASN1_STRING_dup(before), *af=ASN1_STRING_dup(after);
    bool bf;
    if(ASN1_UTCTIME_cmp_time_t(be,ct)>=0||ASN1_UTCTIME_cmp_time_t(af,ct)<=0)
        bf=false;
    else
        bf=true;
    M_ASN1_UTCTIME_free(be);
    M_ASN1_UTCTIME_free(af);
    return bf;
}
///
/// 获取证书的颁发者名称（全部信息）
QString MainWindow::GetCertIssuer()
{
    X509 *x509=verify.userCert1;
    char buf[256];
    memset(buf,0,256);
    X509_get_issuer_name(x509);
    QString str=buf;
    return str;
}

///
/// \brief MainWindow::CheckCertWithCrl
/// 通过黑名单验证证书，验证通过返回真，否则返回假
/// None CRL to check certificate
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
    EVP_cleanup();
    return bf;
}

