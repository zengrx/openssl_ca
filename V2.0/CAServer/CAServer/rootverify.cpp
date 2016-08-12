#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <openssl/evp.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>

////
/// \brief MainWindow::selectCertFile
/// 选择证书文件函数
///
void MainWindow::selectCertFile()
{
    char name1[100];           //局部变量 撤销链路径
    X509 *r_usercert = NULL;   //待验证证书
    BIO *b = NULL;             //接收证书等待格式化
    X509_CRL *Crl = NULL;      //证书撤销链表
    certop.usrurl = QFileDialog::getOpenFileName(this,"select file",signdir,"*.crt;*.der;*.pem;*.cer");
    if(certop.usrurl.isNull())
    {
        ui->textBrowser->append(getTime() + "选择待验证证书失败");
        return;
    }
    else
    {
        ui->textBrowser->append(getTime() + "已选择 '" + certop.usrurl + "' 文件");
        loadRootCA(); //加载根证书信息
        /*开始加载根证书撤销链*/
        QString r_crlname = coredir + "Crl.crl";
        strcpy(name1,r_crlname.toStdString().c_str());
        b = BIO_new_file(name1,"r"); //读取根证书撤销链
        if(b == NULL)
        {
            ui->textBrowser->append(getTime() + "加载根证书撤销链失败，请确认文件存在");
            BIO_free(b);
            return;
        }
        else
        {
            Crl = PEM_read_bio_X509_CRL(b,NULL,NULL,NULL);
            ui->textBrowser->append(getTime() + "根证书撤销链文件加载成功...");
            b = NULL;
        }
        /*加载待验证证书文件*/
        b = BIO_new_file(certop.usrurl.toStdString().data(),"r"); //BIO读取待验证证书
        if(b == NULL)
        {
            ui->textBrowser->append(getTime() + "加载待验证证书文件失败");
            BIO_free(b);
            return;
        }
        else
        {
            r_usercert=PEM_read_bio_X509(b,NULL,NULL,NULL);
            certop.usercert = r_usercert;
            ui->textBrowser->append(getTime() + "待验证证书文件加载成功...");
            b = NULL;
        }
        /*实例化结构体并调用读取信息函数*/
        certInfo info;
        getCertSubInfo(&info);
    }
}

////
/// \brief MainWindow::getCertSubInfo
/// \param info 证书信息结构体实例
/// \return cert info string
/// 获取待验证证书信息函数
///
QString MainWindow::getCertSubInfo(certInfo *info)
{
    QString r_str = NULL;
    //qDebug() << certop.usercert;
    X509_NAME *name = X509_get_subject_name(certop.usercert);
    int num = X509_NAME_entry_count(name); //条目总数
    //qDebug() << num;
    X509_NAME_ENTRY *entry;
    ASN1_OBJECT *obj;
    ASN1_STRING *str;
    char objtmp[80] = {0};
    int fn_nid;
    const char *objbuf;
    setlocale(LC_CTYPE, "");
    for(int i=0; i<num; i++)
    {
        char out[255] = {0};//输出
        entry = (X509_NAME_ENTRY *)X509_NAME_get_entry(name,i);
        obj = X509_NAME_ENTRY_get_object(entry);
        str = X509_NAME_ENTRY_get_data(entry);
        fn_nid = OBJ_obj2nid(obj);
        if(fn_nid == NID_undef)
        {
            OBJ_obj2txt(objtmp, sizeof objtmp, obj, 1);
        }
        else
        {
            objbuf = OBJ_nid2sn(fn_nid);
            strcpy(objtmp,objbuf);
        }
        BIO *mem = BIO_new(BIO_s_mem());
        BIO_set_close(mem, BIO_CLOSE);
        ASN1_STRING_print_ex(mem,str,ASN1_STRFLGS_ESC_QUOTE );
        BUF_MEM *bptr;
        BIO_get_mem_ptr(mem, &bptr);
        int len = bptr->length;
        char * pbuf = new char[len+1];
        memset(pbuf,0,len+1);
        memcpy(pbuf,bptr->data,len);
        strncpy(out,pbuf,strlen(pbuf));
        if (mem != NULL)
        {
            BIO_free(mem);
        }
        switch(i)
        {
        case 0 :
            info->country = out;
            break;
        case 1:
            info->province = out;
            break;
        case 2 :
            info->locality = out;
            break;
        case 3 :
            info->organization = out;
            break;
        case 4 :
            info->ogUnit = out;
            break;
        case 5 :
            info->common = out;
            break;
        case 6 :
            info->emailAddr = out;
            break;
        default:
            break;
        }
        r_str += " \n";
        //tring+=noTime();
        r_str += objtmp;
        r_str += ":\t";
        r_str += out;
        delete [] pbuf;
    }
    ui->textBrowser->append(getTime() + r_str);
    return r_str;
}
