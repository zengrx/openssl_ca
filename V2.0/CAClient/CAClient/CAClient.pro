#-------------------------------------------------
#
# Project created by QtCreator 2016-08-03T19:53:55
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CAClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    certreq.cpp \
    genkeypair.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH +=E:\project\QTProject\OpenSSLCA\include

LIBS+= -LE:\project\QTProject\OpenSSLCA\lib32 -llibeay32
LIBS+= -LE:\project\QTProject\OpenSSLCA\lib32 -lssleay32

RC_FILE = certificate.rc
