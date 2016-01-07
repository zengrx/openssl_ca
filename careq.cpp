#include<mainwindow.h>
#include "ui_mainwindow.h"
#include <stdio.h>

//生成证书申请文件
int MainWindow::careq()
{
    req = X509_REQ_new();
    version=1;
    ret = X509_REQ_set_version(req, version);
    name=X509_NAME_new();
    strcpy(bytes,ui->lineEdit->text().toStdString().c_str());
    len=strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "commonName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 0, -1);
    strcpy(bytes,ui->lineEdit_2->text().toStdString().c_str());
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "countryName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    strcpy(bytes,ui->lineEdit_4->text().toStdString().c_str());
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "localityName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    strcpy(bytes,ui->lineEdit_3->text().toStdString().c_str());
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "stateOrProvinceName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    strcpy(bytes,ui->lineEdit_5->text().toStdString().c_str());
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "organizationName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    strcpy(bytes,ui->lineEdit_6->text().toStdString().c_str());
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "organizationalUnitName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    strcpy(bytes,ui->lineEdit_7->text().toStdString().c_str());
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "emailAddress", V_ASN1_IA5STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    /* subject name */
    ret = X509_REQ_set_subject_name(req, name);
    /* pub key */
    pkey = EVP_PKEY_new();
    rsa = RSA_generate_key(bits, e, NULL, NULL);
    EVP_PKEY_assign_RSA(pkey, rsa);
    ret = X509_REQ_set_pubkey(req, pkey);
    /* attribute */
    strcpy_s(bytes, "test");
    len = strlen(bytes);
    ret = X509_REQ_add1_attr_by_txt(req, "organizationName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    strcpy_s(bytes, "ttt");
    len = strlen(bytes);
    ret = X509_REQ_add1_attr_by_txt(req, "organizationalUnitName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    md = EVP_sha1();
    ret = X509_REQ_digest(req, md, (unsigned char *)mdout, (unsigned int*)&mdlen);
    ret = X509_REQ_sign(req, pkey, md);
    if (!ret)
    {
        printf("sign err!\n");
        X509_REQ_free(req);
        return -1;
    }
    /* 写入文件 PEM 格式 */
    char name[100];
    strcpy(name,(ui->lineEdit->text()+".csr").toStdString().c_str());
    b = BIO_new_file(name, "w");
    PEM_write_bio_X509_REQ(b, req);
    BIO_free(b);
    /* DER 编码 */
    len = i2d_X509_REQ(req, NULL);
    der = (unsigned char *)malloc(len);
    p = der;
    len = i2d_X509_REQ(req, &p);
    OpenSSL_add_all_algorithms();
    ret = X509_REQ_verify(req, pkey);
    if (ret<0)
    {
        printf("verify err.\n");
    }
    fp = fopen("certreq2.txt", "wb");
    fwrite(der, 1, len, fp);
    fclose(fp);
    free(der);
    X509_REQ_free(req);

    //写入消息显示窗
    QString commonName,countryName,province,city,organization,unit,emailaddr;
    message += "this is a test message:\n";
    commonName = "common:"+ui->lineEdit->text()+"\n";
    countryName = "country:"+ui->lineEdit_2->text()+"\n";
    province = "province:"+ui->lineEdit_3->text()+"\n";
    city = "city:"+ui->lineEdit_4->text()+"\n";
    organization = "organization:"+ui->lineEdit_5->text()+"\n";
    unit = "unit:"+ui->lineEdit_6->text()+"\n";
    emailaddr = "email:"+ui->lineEdit_7->text()+"\n";
    ui->textEdit->setText(message);
    //if(ui->lineEdit->text())
    message += commonName;
    message += countryName;
    message += province;
    message += city;
    message += organization;
    message += unit;
    message += emailaddr;
    showMessage();
    return 0;
}

//显示消息函数
//message为全局变量，添加信息显示时累加显示
void MainWindow::showMessage()
{
    message += "---------------------------------------------------------------------------------------------------------\n";
    ui->textEdit->setText(message);
}
