#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include <string.h>
#include <stdio.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <QDateTime>

namespace Ui {
class MainWindow;
}

struct StoreCer
{
    QString userCerUrl = NULL;      //存储用户证书的路径
    X509 *userCert1 = NULL;         //用户1
    X509 *rootCert = NULL;          //根证书
    X509_CRL *Crl = NULL;           //证书撤销链表
    X509_STORE_CTX *ctx = NULL;     //存储证书相关设置
    STACK_OF(X509) *caCertStack = NULL;     //用于证书链？
    X509_STORE *rootCertStore = NULL;
    EVP_PKEY *pkey=NULL;
};
struct certInfo
{
    QString client;
    QString state;
    QString location;
    QString organization;
    QString organizationalUnitName;
    QString country;
    QString email;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //证书申请按钮
    void on_pushButton_clicked();

    //选择待验证书按钮
    void on_pushButton_7_clicked();

    //验证证书按钮
    void on_pushButton_8_clicked();

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
    int bits;
    unsigned long e = RSA_3;
    unsigned char *der, *p;
    FILE *fp;

    const EVP_MD *md;
    X509 *x509;
    BIO *b;
    STACK_OF(X509_EXTENSION) *exts;
    StoreCer verify;

    //申请证书
    int careq();

    //显示消息
    void showMessage();

    //warning: app crash
    bool CheckCertWithCrl();
    QString GetCertIssuer();
    int Crl();

    //display cert info
    QString GetCertSubjectString(certInfo *info);

    //display serialnumber
    QString GetCertSerialNumber();

    //verify certificate life time
    bool CheckCertTime();

    //load certificate and ca
    int Load_Cer();

    //verify certificate whit rooyt
    bool CheckCertWithRoot();

    //显示证书详细信息
    void detail();

    //根证书签名
    bool CreateCertFromRequestFile(int serialNumber,int days,
                                   char *requestFile,char *pubCert,
                                   char *priCert, int format);

    QString getTime();
    QString noTime();
};

#endif // MAINWINDOW_H
