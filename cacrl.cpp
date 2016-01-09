#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>

int MainWindow::Crl()
{

    time_t t;
    X509_NAME *issuer;
    ASN1_TIME *lastUpdate,*nextUpdate,*rvTime;
    X509_CRL  *crl=NULL;
    X509_REVOKED  *revoked;
    EVP_PKEY *pkey;
    ASN1_INTEGER   *serial;
    BIO  *bp;
    X509 *x509;

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
    /* 添加被撤销证书序列号*/
    revoked=X509_REVOKED_new();
    serial=ASN1_INTEGER_new();
    ASN1_INTEGER_set(serial,1000);
    X509_REVOKED_set_serialNumber(revoked,serial);
    rvTime=ASN1_TIME_new();
    t=time(NULL);
    ASN1_TIME_set(rvTime,t+2000);
    X509_CRL_set_nextUpdate(crl,rvTime);
    X509_REVOKED_set_revocationDate(revoked,rvTime);
    X509_CRL_add0_revoked(crl,revoked);
    /* 排序*/
    X509_CRL_sort(crl);
    /* 签名*/
    X509_CRL_sign(crl,pkey,EVP_md5());
    /* 写入文件*/
    bp=BIO_new_file("CRL.crl","wb");
    PEM_write_bio_X509_CRL(bp,crl);
    BIO_free(bp);
    X509_CRL_free(crl);
    return 0;
}
