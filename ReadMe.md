拖了一年多了啊啊啊啊，必须要更新readme了

new version [here](http://git.oschina.net/rx_z/openssl_ca)

A simple document in [my blog](https://zengrx.github.io/2017/12/16/digital-certificate-authority-system-in-C-language/)

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
>   git clone https://github.com/zengrx/openssl_ca.git    
    client: .\openssl_ca\V2.0\CAClient\CAClient, double click CAClient.pro file    
    server: .\openssl_ca\V2.0\CAServer\CAServer, double click CAServer.pro file   

### Client & Server
![client](https://github.com/zengrx/openssl_ca/blob/master/Doc/pictures/client.png)
![server3](https://github.com/zengrx/openssl_ca/blob/master/Doc/pictures/server3.png)
![server2](https://github.com/zengrx/openssl_ca/blob/master/Doc/pictures/server2.png)
![server1](https://github.com/zengrx/openssl_ca/blob/master/Doc/pictures/server1.png)

### _(:3 」∠)/
大三写的密码学大作业，啃了半个月openssl文档。bug挺多的，抛砖引玉啦
orz_amber@163.com

---------

## See also
* [List of cryptography GUI tools](https://gist.github.com/stokito/eea7ee50d51e1db30122e2e33a62723e)
