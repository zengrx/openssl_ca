#ifndef CAREQ_H
#define CAREQ_H

#include <string.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <QString>

#pragma comment (lib,"libeay32.lib")
#pragma comment (lib,"ssleay32.lib")
#pragma warning (disable:4996)

X509_REQ *req;
int ret;
long version;
X509_NAME *name;
EVP_PKEY *pkey;
RSA *rsa;
X509_NAME_ENTRY *entry = NULL;
char bytes[100], mdout[20];
int len, mdlen;
int bits = 512;
unsigned long e = RSA_3;
unsigned char *der, *p;
FILE *fp;
const EVP_MD *md;
X509 *x509;
BIO *b;
STACK_OF(X509_EXTENSION) *exts;

//证书申请
int careq();

#endif // CAREQ_H

