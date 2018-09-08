拖了一年多了啊啊啊啊，必须要更新readme了

new version [here](http://git.oschina.net/rx_z/openssl_ca)

## Certificate Authority using Openssl API

### Introduction

OpenSSL_CA is a simulate software of digital certificate authority, programmed with C++ using QT framework. Well it is Cryptography class homework actually. This project may provide you some help when you start learning openssl API.

### Base Functions

 - generate certificate request file (*.csr)
 - file transfer between client and server
 - csr file signed by root certificate
 - root certificate verify
 - certificate revocation or restore the revoked certificate
 - generate certificate revocation list (*.crl)

### Usage
> git clone https://github.com/zengrx/openssl_ca.git    
client: .\openssl_ca\V2.0\CAClient\CAClient, double click CAClient.pro file    
server: .\openssl_ca\V2.0\CAServer\CAServer, double click CAServer.pro file   

### Client & Server
![client](https://github.com/zengrx/openssl_ca/blob/master/doc/pictures/client.png)
![server3](https://github.com/zengrx/openssl_ca/blob/master/doc/pictures/server3.png)
![server2](https://github.com/zengrx/openssl_ca/blob/master/doc/pictures/server2.png)
![server1](https://github.com/zengrx/openssl_ca/blob/master/doc/pictures/server1.png)

### _(:3 」∠)/
大三写的密码学大作业，啃了半个月openssl文档。bug挺多的，抛砖引玉啦
基于OpenSSL的证书中心 桂林电子科技大学 计算机与信息安全学院 信息安全13402 曾若星 张德信   
orz_amber@163.com

---------
