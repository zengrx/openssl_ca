#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>

///
/// \brief MainWindow::Load_Cer
/// 从文件读取证书
/// \return -1 --> ca empety
/// \return -2 --> user empety
/// \return 0 --> ok
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
        return -1;
    }
    rootCert = PEM_read_bio_X509(b,NULL,NULL,NULL);
    b=BIO_new_file(verify.userCerUrl.toStdString().data(),"r");
    if(b==NULL)
    {
        return -2;
    }
    userCert1=PEM_read_bio_X509(b,NULL,NULL,NULL);
    b=BIO_new_file("Crl.crl","r");
    if(b==NULL)
    {
        return -3;
    }
    Crl=PEM_read_bio_X509_CRL(b,NULL,NULL,NULL);
    b = BIO_new_file("rootca1.key.insecure", "r");
    if(b == NULL)
    {
        return -4;
    }

    pkey = PEM_read_bio_PrivateKey(b, NULL, 0, NULL);
    BIO_free(b);
    verify.rootCert=rootCert;
    verify.userCert1=userCert1;
    verify.Crl=Crl;
    verify.pkey=pkey;
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

    X509_free(x509);
    X509_free(root);
    if(ret==1)
        return true;
    else
        return false;
}
