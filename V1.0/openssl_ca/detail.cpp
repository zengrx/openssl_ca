#include<mainwindow.h>
#include "ui_mainwindow.h"
#include <stdio.h>
#include <iostream>
//#include <openssl/applink.c>

#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>
#include <openssl/evp.h>

using namespace std;

void MainWindow::detail()
{
    X509 *x;
    BIO *b;
    int ret;
    b = BIO_new_file("test2.crt", "r");
    if(!b)
    {
        printf("error in detail\n");
    }
    x = PEM_read_bio_X509(b, NULL, NULL, NULL);
    //char buf[256];
    //memset(buf, 0, 256);
    //getName(X509_get_subject_name(x), buf);
    BIO_set_fp(b, stdout, BIO_NOCLOSE);
    ret = X509_print(b, x);
    if(ret)
    {
        cout << "success" << endl;
    }
    //string str = buf;
    //cout<<str<<endl;
    BIO_free(b);
    X509_free(x);
}
