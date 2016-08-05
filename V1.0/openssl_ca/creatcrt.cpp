#include "mainwindow.h"
#include <locale.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>
#include <openssl/engine.h>
#include <openssl/evp.h>
#include <string.h>
#include <iostream>

#define FORMAT_DER 1 //FORMAT_ASN1
#define FORMAT_PEM 3
#define FORMAT_NET 4
#define FORMAT_P12 5

X509 * LoadCert()
{
    X509 * x509 = NULL;
    BIO * in = NULL;

    in = BIO_new_file("rootca1.crt","r");
    if(in == NULL)
    {
        printf("read cert error\n");
    }

    x509 = PEM_read_bio_X509(in, NULL, NULL, NULL);

    BIO_free(in);

    if (x509)
        return x509;
    else
    {
        printf("unable to load certificate\n");
        return NULL;
    }
}

EVP_PKEY * LoadKey()
{
    EVP_PKEY *pkey = NULL;
    BIO * in = NULL;

    in = BIO_new_file("rootca1.key", "r");
    if(in == NULL)
    {
        printf("read key error\n");
    }

    pkey = PEM_read_bio_PrivateKey(in, NULL, 0, NULL);
    //pkey = d2i_PrivateKey_bio(in,NULL);

    BIO_free(in);

    if (pkey)
        return pkey;
    else
    {
        printf("unable to load private key\n");
        return NULL;
    }
}

// 根据证书请求文件签发证书

bool MainWindow::CreateCertFromRequestFile(int serialNumber,
                        int days, char *requestFile,
                        char *pubCert, char *priCert, int format)
{
    X509 * rootCert = NULL;
    EVP_PKEY * rootKey = NULL;
    int i, j;
    bool ret;

    OpenSSL_add_all_digests();

    rootKey = LoadKey();
    rootCert = LoadCert();

    if (rootKey == NULL || rootCert == NULL)
    {
        printf("rootkey or rootcert wrong\n");
        return false;
    }

    X509 * userCert = NULL;
    EVP_PKEY * userKey = NULL;
    X509_REQ *req = NULL;
    BIO *in;
//    QString tmp = "../" + QString(QLatin1String(requestFile));
//    qDebug() << tmp << endl;
//    //QString转char *
//    QByteArray ba = tmp.toLatin1();
//    requestFile = ba.data();
    in = BIO_new_file(requestFile, "r");
    req = PEM_read_bio_X509_REQ(in, NULL, NULL, NULL);
    BIO_free(in);

    userKey = X509_REQ_get_pubkey(req);
    userCert = X509_new();

    X509_set_version(userCert, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(userCert), serialNumber);
    X509_gmtime_adj(X509_get_notBefore(userCert), 0);
    X509_gmtime_adj(X509_get_notAfter(userCert), (long)60 * 60 * 24 * days);
    X509_set_pubkey(userCert, userKey);
    EVP_PKEY_free(userKey);

    X509_set_subject_name(userCert, req->req_info->subject);

    X509_set_issuer_name(userCert, X509_get_issuer_name(rootCert));
    X509_sign(userCert, rootKey, EVP_sha1());

    BIO * bcert = NULL, *bkey = NULL;
    if (((bcert = BIO_new_file(pubCert, "w")) == NULL) || ((bkey = BIO_new_file(priCert, "w")) == NULL))
    {
        printf("bcert or bkey wrong\n");
        return false;
    }

    if (format == FORMAT_DER)
    {
        ret = true;
        i = i2d_X509_bio(bcert, userCert);
        j = i2d_PrivateKey_bio(bkey, userKey);
    }
    else if (format == FORMAT_PEM)
    {
        ret = true;
        i = PEM_write_bio_X509(bcert, userCert);
        j = PEM_write_bio_PrivateKey(bkey, userKey, NULL, NULL, 0, NULL, NULL);
    }
    if (!i || !j)
    {
        printf("pem or der wrong\n");
        ret = false;
    }

    BIO_free(bcert);
    BIO_free(bkey);
    X509_free(userCert);
    X509_free(rootCert);
    EVP_PKEY_free(rootKey);
    return true;
}
