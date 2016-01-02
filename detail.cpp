#include<mainwindow.h>
#include "ui_mainwindow.h"
#include <stdio.h>
#include <iostream>
#include <openssl/applink.c>

//#include <locale.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>
//#include <openssl/rand.h>
//#include <openssl/engine.h>
#include <openssl/evp.h>

using namespace std;

//bool getName(X509_NAME * name,char * outname)
//{
//    if(name == NULL)
//    {
//        return false;
//    }
//    int num = X509_NAME_entry_count(name);
//    X509_NAME_ENTRY *entry;
//    ASN1_OBJECT *obj;
//    ASN1_STRING *str;
//    char objtmp[80] = { 0 };
//    char pmbbuf[3] = { 0 };
//    int fn_nid;
//    const char *objbuf;

//    setlocale(LC_CTYPE, "");

//    for (int i = 0; i<num; i++)
//    {
//        //UINT uChina[255] = { 0 };//存放中文
//        char cEnglish[64][128] = { 0 };//存放英文
//        char out[255] = { 0 };//输出
//        entry = (X509_NAME_ENTRY *)X509_NAME_get_entry(name, i);
//        obj = X509_NAME_ENTRY_get_object(entry);
//        str = X509_NAME_ENTRY_get_data(entry);
//        fn_nid = OBJ_obj2nid(obj);
//        if (fn_nid == NID_undef)
//            OBJ_obj2txt(objtmp, sizeof objtmp, obj, 1);
//        else
//        {
//            objbuf = OBJ_nid2sn(fn_nid);
//            strcpy(objtmp, objbuf);
//            //objbuf = OBJ_nid2ln(fn_nid);
//        }

//        BIO *mem = BIO_new(BIO_s_mem());
//        BIO_set_close(mem, BIO_CLOSE); /* BIO_free() free BUF_MEM */
//        ASN1_STRING_print_ex(mem, str, ASN1_STRFLGS_ESC_QUOTE);
//        BUF_MEM *bptr;
//        BIO_get_mem_ptr(mem, &bptr);
//        int len = bptr->length;
//        char * pbuf = new char[len + 1];
//        memset(pbuf, 0, len + 1);
//        memcpy(pbuf, bptr->data, len);

//        //检索\U位置,存入数组
//        char * pdest = NULL;
//        //UINT uUlocal = 0;
//        char * ptemp = pbuf;
//        for (char j = 0;; j++)//检索位置信息,分别存入英文,中文
//        {
//            pdest = strstr(ptemp, "\\U");
//            if (pdest == NULL)//没有中文字符
//            {
//                strncpy(cEnglish[j], ptemp, strlen(ptemp));//保存最后一段英文
//                break;
//            }
//            uUlocal = pdest - ptemp + 1;

//            strncpy(cEnglish[j], ptemp, uUlocal - 1);//保存英文
//            char hex[5] = { 0 };
//            strncpy(hex, ptemp + uUlocal + 1, 4);//保存中文
//            int ten = HexToTen(hex);
//            uChina[j] = ten;
//            ptemp = ptemp + uUlocal + 5;

//        }
//        if (mem != NULL)
//            BIO_free(mem);
//        wchar_t pwchello[2] = { 0 };
//        for (UINT k = 0; k <= j; k++)//包含最后一段英文
//        {
//            if (k>63)//达到英文最大数量
//                break;
//            strcat(out, cEnglish[k]);//加入英文
//            pwchello[0] = uChina[k];
//            int result = wcstombs(pmbbuf, pwchello, 2);

//            if (result != -1)
//                strncat(out, pmbbuf, 2);//加入中文
//        }
//        delete[] pbuf;
//        strcat(outName, objtmp);//C
//        strcat(outName, "=");//=
//        strcat(outName, out);
//        strcat(outName, "\n");
//    }
//    return true;
//}

void MainWindow::detail()
{
    X509 *x;
    BIO *b;
    int ret;
    b = BIO_new_file("test1.crt", "r");
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
