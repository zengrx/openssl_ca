#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>


///
/// \brief X509_Pem_Verify
/// 验证Pem证书
/// \return 0/-1
/// 关键函数
/// int X509_STORE_CTX_init(X509_STORE_CTX *ctx,
///     X509_STORE *store,X509 *x509, STACK_OF(X509) *chain);
/// int X509_verify_cert(X509_STORE_CTX *ctx);
///
int MainWindow::X509_Pem_Verify()
{
    X509_STORE_CTX *ctx = NULL;     //存储证书
    X509 *usrCert1 = NULL;          //用户1
    X509 *rootCert = NULL;          //根证书
    BIO *b;                         //接收证书等待格式化
    X509_CRL *Crl = NULL;           //证书撤销链表
    STACK_OF(X509) *caCertStack = NULL;     //用于证书链？
    X509_STORE *rootCertStore = NULL;       //存储根证书
    int rv;

    b=BIO_new_file("cacert.pem","r");
    rootCert = PEM_read_bio_X509(b,NULL,NULL,NULL);
    b=BIO_new_file(userCerUrl.toStdString().c_str(),"r");
    usrCert1=PEM_read_bio_X509(b,NULL,NULL,NULL);
    b=BIO_new_file("Crl.pem","r");
    Crl=PEM_read_bio_X509_CRL(b,NULL,NULL,NULL);
    BIO_free(b);

    rootCertStore = X509_STORE_new();
    X509_STORE_add_cert(rootCertStore,rootCert);//添加CA的证书？
    X509_STORE_add_crl(rootCertStore,Crl);    //初始化验证
    rv = X509_STORE_CTX_init(ctx,rootCertStore,usrCert1,caCertStack);
    if(rv != 1)
    {
        perror("X509_STORE_CTX_init failed\n");
        X509_free(usrCert1);
        X509_free(rootCert);
        X509_STORE_CTX_cleanup(ctx);
        X509_STORE_CTX_free(ctx);
        X509_STORE_free(rootCertStore);
        return -1;
    }
    //验证客户1的证书
    rv = X509_verify_cert(ctx);
    if(rv != 1)
    {
        printf("verify usercert1 failed err=%d,info:%s\n",ctx->error,X509_verify_cert_error_string(ctx->error));
    }
    else
    {
        printf("verify usercert1 ok\n");
    }
    //释放资源
    X509_free(usrCert1);
    X509_free(rootCert);
    X509_STORE_CTX_cleanup(ctx);
    X509_STORE_CTX_free(ctx);
    X509_STORE_free(rootCertStore);
    return 0;
}


///
/// \brief X509_Der_Verify
/// 验证Der证书
/// \return 0/-1
///
int MainWindow::X509_Der_Verify()
{
    unsigned char usrCertificate1[4096];
    unsigned long usrCertificate1Len;
    unsigned char derCrl[4096];
    unsigned long derCrlLen;
    unsigned char derRootCert[4096];
    unsigned long derRooCertLen;
    int rv;

    X509_STORE_CTX *ctx = NULL;     //存储证书
    X509 *usrCert1 = NULL;          //用户1
    X509 *rootCert = NULL;          //根证书
    X509_CRL *Crl = NULL;           //证书撤销链表
    STACK_OF(X509) *caCertStack = NULL;     //用于证书链？
    X509_STORE *rootCertStore = NULL;       //存储根证书

    //        X509 *caCert = NULL;
    //        int j = 0;
    unsigned char *pTmp = NULL;
    FILE *fp;
    fp = fopen("rootca.crt","rb");
    if(fp == NULL){
        perror("open file failed\n");
        return -1;
    }
    //证书导入，为初始化准备
    derRooCertLen = fread(derRootCert,1,4096,fp);
    fclose(fp);

    fp = fopen("user1.csr","rb");
    if(fp == NULL){
        perror("open file failed\n");
        return -1;
    }

    derCrlLen = fread(derCrl,1,4096,fp);
    fclose(fp);


    fp = fopen("user1req.pem","rb");
    if(fp == NULL){
        perror("open file failed\n");
        return -1;
    }
    usrCertificate1Len = fread(usrCertificate1,1,4096,fp);
    fclose(fp);

    //内置化证书，即初始化
    //Der格式
    printf("1\n");
    pTmp =  derRootCert;
    rootCert = d2i_X509(NULL,(unsigned const char **)&pTmp,derRooCertLen);
    if( NULL == rootCert){
        printf("d2i_X509 failed1,ERR_get_error=%s\n",ERR_reason_error_string(ERR_get_error()));
        return -1;
    }
    printf("2\n");
    pTmp = usrCertificate1;
    usrCert1 = d2i_X509(NULL,(unsigned const char **)&pTmp,usrCertificate1Len);
    if(usrCert1 == NULL){
        perror("d2i_X509 failed\n");
        return -1;
    }
    printf("3\n");
    printf("4\n");
    pTmp = derCrl;
    Crl = d2i_X509_CRL(NULL,(unsigned const char **)&pTmp,derCrlLen);
    if(Crl == NULL){
        perror("d2i_X509 failed\n");
        return -1;
    }
    printf("5\n");
    rootCertStore = X509_STORE_new();
    X509_STORE_add_cert(rootCertStore,rootCert);//添加CA的证书？
    X509_STORE_set_flags(rootCertStore,X509_V_FLAG_CRL_CHECK);//will error
    //证书撤销列表
    X509_STORE_add_crl(rootCertStore,Crl);
    printf("6\n");
    //初始化验证
    rv = X509_STORE_CTX_init(ctx,rootCertStore,usrCert1,caCertStack);
    printf("1234\n");
    if(rv != 1)
    {
        perror("X509_STORE_CTX_init failed\n");
        X509_free(usrCert1);
        X509_free(rootCert);
        X509_STORE_CTX_cleanup(ctx);
        X509_STORE_CTX_free(ctx);
        X509_STORE_free(rootCertStore);
        return -1;
    }
    //验证客户1的证书
    rv = X509_verify_cert(ctx);
    if(rv != 1)
    {
        printf("verify usercert1 failed err=%d,info:%s\n",ctx->error,X509_verify_cert_error_string(ctx->error));
    }
    else
    {
        printf("verify usercert1 ok\n");
    }
    //释放空间
    X509_free(usrCert1);
    X509_free(rootCert);
    X509_STORE_CTX_cleanup(ctx);
    X509_STORE_CTX_free(ctx);
    X509_STORE_free(rootCertStore);
    return 0;
}

