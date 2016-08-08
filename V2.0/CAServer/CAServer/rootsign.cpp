#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <locale.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>
#include <openssl/engine.h>
#include <openssl/evp.h>
#include <string.h>
#include <fstream>

#define FORMAT_DER 1 //FORMAT_ASN1
#define FORMAT_PEM 3
#define FORMAT_NET 4
#define FORMAT_P12 5

////
/// \brief LoadCert
/// \return x509类型对象
/// X509函数
/// 载入根证书，存储为PEM格式数据
///
X509 * MainWindow::LoadCert()
{
    char name1[100]; //局部变量 存储根证书文件名
    X509 * x509 = NULL;
    BIO * in = NULL;
    QString r_dir = coredir + "rootca.crt"; //构造根证书路径
    //qDebug() << r_dir;
    strcpy(name1,r_dir.toStdString().c_str());
    in = BIO_new_file(name1,"r"); //读取根证书
    if(in == NULL)
    {
        ui->textBrowser->append(getTime() + "载入根证书错误，请检查文件");
    }
    //将BIO类型变量读取到X509对象中
    x509 = PEM_read_bio_X509(in, NULL, NULL, NULL);
    BIO_free(in);
    if(x509)
    {
        return x509;
    }
    else
    {
        ui->textBrowser->append(getTime() + "无法载入根证书，请检查文件完整性");
        return NULL;
    }
}

////
/// \brief MainWindow::LoadKey
/// \return
///
EVP_PKEY * MainWindow::LoadKey()
{
    char name1[100]; //局部变量 储存根证书私钥名
    EVP_PKEY *pkey = NULL;
    BIO * in = NULL;
    QString r_dir = coredir + "rootca.key"; //构造根证书路径
    strcpy(name1,r_dir.toStdString().c_str());
    in = BIO_new_file(name1, "r");
    if(in == NULL)
    {
        ui->textBrowser->append(getTime() + "载入根证书秘钥错误，请检查文件");
    }
    //将BIO类型变量读取到X509对象中
    pkey = PEM_read_bio_PrivateKey(in, NULL, 0, NULL);
    BIO_free(in);
    if(pkey)
    {
        return pkey;
    }
    else
    {
        ui->textBrowser->append(getTime() + "无法载入根证书秘钥，请检查文件完整性");
        return NULL;
    }
}

////
/// \brief MainWindow::CreateCertFromRequestFile
/// \param serialNumber 序列号
/// \param days 签发天数
/// \param requestFile 需要读取的请求文件
/// \param pubCert 公钥信息
/// \param priCert 私钥信息
/// \param format 签发文件格式
/// \return
/// 根据证书请求文件签发证书
///
bool MainWindow::CreateCertFromRequestFile(int serialNumber,
                        int days, char *requestFile,
                        char *pubCert, char *priCert, int format)
{
    X509 * rootCert = NULL; //x509根证书对象
    EVP_PKEY * rootKey = NULL; //根证书秘钥对象
    int i, j;
    bool ret; //返回值 太多写了一部分
    //调用上边两个函数生成对象
    rootKey = LoadKey();
    rootCert = LoadCert();
    if (rootKey == NULL || rootCert == NULL)
    {
        ui->textBrowser->append(getTime() + "加载根证书或秘钥失败，请重试");
        return false;
    }
    X509 * userCert = NULL;
    EVP_PKEY * userKey = NULL;
    X509_REQ *req = NULL;
    BIO *in;
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
    if(((bcert = BIO_new_file(pubCert, "w")) == NULL) ||
            ((bkey = BIO_new_file(priCert, "w")) == NULL))
    {
        ui->textBrowser->append(getTime() + "生成BIO型中间数据时发生错误");
        return false;
    }
    //按格式签发用户证书并生成私钥
    if(format == FORMAT_DER)
    {
        ret = true;
        i = i2d_X509_bio(bcert, userCert);
        j = i2d_PrivateKey_bio(bkey, userKey);
    }
    else if(format == FORMAT_PEM)
    {
        ret = true;
        i = PEM_write_bio_X509(bcert, userCert);
        j = PEM_write_bio_PrivateKey(bkey, userKey, NULL, NULL, 0, NULL, NULL);
    }

    if(!i || !j)
    {
        ui->textBrowser->append(getTime() + "签发PEM或DER用户文件时发生错误");
        ret = false;
    }
    BIO_free(bcert);
    BIO_free(bkey);
    X509_free(userCert);
    X509_free(rootCert);
    EVP_PKEY_free(rootKey);
    return true;
}

void MainWindow::SignCertFile()
{
    int serial;         //证书编号
    int days;           //申请天数
    QString msgout;     //输出内容
    char name1[100];    //申请文件名
    char name2[100];    //签发证书名
    char name3[100];    //子证书私钥
    QString r_reqfname; //局部变量 签名时暂存值
    r_reqfname = reqdir + reqfilename; //接收的请求文件相对路径
    strcpy(name1,(r_reqfname+".csr").toStdString().c_str());
    r_reqfname = signdir + reqfilename; //生成证书及秘钥文件相对路径
    strcpy(name2,(r_reqfname+".crt").toStdString().c_str());
    strcpy(name3,(r_reqfname+".key").toStdString().c_str());
    days = ui->comboBox_2->currentText().toInt();
    //读写存储签名序列号的文本
    std::ifstream infile; //文件流对象
    QString m_dir = coredir + "signSerial.txt"; //局部变量 储存序列号文件路径
    infile.open(m_dir.toStdString());
    if(!infile)
    {
        //QMessageBox::information(NULL,"error","open this file failed\n"); //使用时添加QMessageBox头文件
        ui->textBrowser->append(getTime() + "序列号读取错误");
        return;
    }
    else
    {
        infile >> serial;
        infile.close();
    }
    //调用签名函数
    if(CreateCertFromRequestFile(serial,days,name1,name2,name3,3))
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++
        //append by Qool in order to write serial to file
        /*if(WriteSerial2Json(serial))
            UpdataListWidget2();
        else
            qWarning("Write json failed.")*/;
        //+++++++++++++++++++++++++++++++++++++++++++++++
        std::ofstream outfile;
        outfile.open(m_dir.toStdString());
        serial += 1; //签发后序列号自增1
        outfile << serial;
        outfile.close();
        msgout = getTime() + "根证书签名成功，文件已生成";
    }
    else
    {
        msgout = getTime() + "根证书签名失败，请重试";
        return;
    }
    ui->textBrowser->append(getTime() + msgout);
}
