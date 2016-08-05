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
#include <QQueue>
#include <QJsonObject>

namespace Ui {
class MainWindow;
}

struct StoreCer
{
    QString userCerUrl = NULL;      //存储用户证书的路径
    X509 *userCert1 = NULL;         //用户1
    X509 *rootCert = NULL;          //根证书
    X509_CRL *Crl = NULL;           //证书撤销链表
    EVP_PKEY *pkey=NULL;
    QString ser;
};

//store certificate info for display
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

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_listWidget_currentRowChanged(int currentRow);

    void on_pushButton_9_clicked();

    void on_listWidget_2_currentRowChanged(int currentRow);

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

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
    int indexPtr;
    QList<QString> queue;
    QJsonObject signlistjson;

    //请求文件名，无后缀
    QString fname;

    //申请证书
    int careq();

    //显示消息
    void showMessage();

    //warning: app maybe crash
    QString GetCertIssuer();

    //create CRl
    int Crl();

    //useless code
    bool CreateCrl();

    //revoked certificate
    bool revokedCert();

    //display revoked certificate list
    void Init_DisCRL();
    void DisCRL();

    //display cert info
    QString GetCertSubjectString(certInfo *info);

    //display serialnumber
    QString GetCertSerialNumber(X509 *x509);

    //verify certificate life time
    bool CheckCertTime();

    //load certificate and ca
    int Load_Cer();
    int Revoked_Load();

    //verify certificate whit root
    bool CheckCertWithRoot();

    //verify certificate with CRL
    bool CheckCertWithCrl();

    //verify certificate's Serial with CRL
    bool CheckSerialWithCrl(ASN1_INTEGER   *serial);

    //display info but no time
    QString noTime();

    //chage ASN1_Time to time_t
    time_t ASN1_GetTimeT(ASN1_TIME* time);

    //delete revoked serial
    bool DeleteCRLItem();

    //Load sign.txt file,file to memory
    bool LoadSignFile(QList<QString>* que);

    //Write a serial to signlist file
    bool Write2SignList(int serial);

    //Write serial queue to file
    bool Mem2SignList(QList<QString>* queue);

    //Read signlist.txt to json
    bool ReadJson(QJsonObject &json);

    //Save to json
    bool SaveJson(QJsonObject &json);
    bool WriteSerial2Json(const int &serial);
    bool UpdataListWidget2();

    //显示证书详细信息
    void detail();

    //根证书签名
    bool CreateCertFromRequestFile(int serialNumber,int days,
                                   char *requestFile,char *pubCert,
                                   char *priCert, int format);

    QString getTime();
};

#endif // MAINWINDOW_H
