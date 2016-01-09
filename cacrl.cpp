#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>

int Crl()
{

    int ret;
    unsigned long e=RSA_3;
    time_t t;
    X509_NAME *issuer;
    ASN1_TIME *lastUpdate,*nextUpdate,*rvTime;
    X509_CRL  *crl=NULL;
    X509_REVOKED  *revoked;
    EVP_PKEY *pkey;
    ASN1_INTEGER   *serial;
    RSA   *r;
    BIGNUM  *bne;
    BIO  *bp;

    /* 生成密钥*/
    bne=BN_new();
    ret=BN_set_word(bne,e);
    r=RSA_new();
    ret=RSA_generate_key_ex(r,1024,bne,NULL);
    if(ret!=1)
    {
           printf("RSA_generate_key_ex err!\n");
           return -1;
    }
    pkey=EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pkey,r);
    crl=X509_CRL_new();
    /* 设置版本*/
    ret=X509_CRL_set_version(crl,3);
    /* 设置颁发者*/
    issuer=X509_NAME_new();
    ret=X509_NAME_add_entry_by_NID(issuer,NID_commonName,V_ASN1_PRINTABLESTRING,(unsigned char *)"CRL issuer",10,-1,0);
    ret=X509_CRL_set_issuer_name(crl,issuer);
    /* 设置上次发布时间*/
    lastUpdate=ASN1_TIME_new();
    t=time(NULL);
    ASN1_TIME_set(lastUpdate,t);
    ret=X509_CRL_set_lastUpdate(crl,lastUpdate);
    /* 设置下次发布时间*/
    nextUpdate=ASN1_TIME_new();
    t=time(NULL);
    ASN1_TIME_set(nextUpdate,t+1000);
    ret=X509_CRL_set_nextUpdate(crl,nextUpdate);
    /* 添加被撤销证书序列号*/
    revoked=X509_REVOKED_new();
    serial=ASN1_INTEGER_new();
    ret=ASN1_INTEGER_set(serial,1000);
    ret=X509_REVOKED_set_serialNumber(revoked,serial);
    rvTime=ASN1_TIME_new();
    t=time(NULL);
    ASN1_TIME_set(rvTime,t+2000);
    ret=X509_CRL_set_nextUpdate(crl,rvTime);
    ret=X509_REVOKED_set_revocationDate(revoked,rvTime);
    ret=X509_CRL_add0_revoked(crl,revoked);
    /* 排序*/
    ret=X509_CRL_sort(crl);
    /* 签名*/
    ret=X509_CRL_sign(crl,pkey,EVP_md5());
    /* 写入文件*/
    bp=BIO_new_file("CRL.crl","wb");
    PEM_write_bio_X509_CRL(bp,crl);
    BIO_free(bp);
    X509_CRL_free(crl);
    return 0;
}
