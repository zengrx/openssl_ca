#include "mainwindow.h"
#include "ui_mainwindow.h"

////
/// \brief MainWindow::generateKeypair
/// \return true or false
/// 生成用户公私钥对函数
///
bool MainWindow::generateKeypair()
{
    pkey = EVP_PKEY_new();
    rsapair = RSA_generate_key(bits, e, NULL, NULL); //生成公私钥对
    BIO *bp;
    char name1[100]; //
    QString g_filename, pwd;
    g_filename = ui->lineEdit_8->text();
    if(g_filename.isEmpty())
    {
        ui->textBrowser->append(getTime() + "请填写请求文件名称");
        return false;
    }
    strcpy(name1,(pradir + g_filename +".prakey").toStdString().c_str());
    bp = BIO_new_file(name1, "w");
    pwd = ui->lineEdit_11->text();
    if(pwd.isEmpty())
    {
        ui->textBrowser->append(getTime() + "未输入私钥保护口令，使用强制无口令方式生成私钥");
        PEM_write_bio_RSAPrivateKey(bp, rsapair, NULL,
                                    NULL ,0, NULL, NULL);
        return true;
    }
    else
    {
        strcpy(name1,pwd.toStdString().c_str());
        ui->textBrowser->append(getTime() + "已生成具备口令保护的用户私钥");
        PEM_write_bio_RSAPrivateKey(bp, rsapair, EVP_des_ede3_ofb(),
                            (unsigned char *)name1, strlen(name1), NULL, NULL);
        return true;
    }
    return false;
}
