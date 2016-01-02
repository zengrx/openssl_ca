#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <string.h>
#include <stdio.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //证书申请按钮
    void on_pushButton_clicked();

    //证书签名按钮
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

    QString message;
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

    //申请证书
    int careq();

    //显示消息
    void showMessage();

    //显示证书详细信息
    void detail();

    //签名
    bool CreateCertFromRequestFile(int serialNumber,
                                   int days, char *requestFile,
                                   char *pubCert, char *priCert, int format);

};

#endif // MAINWINDOW_H
