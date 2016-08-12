#-------------------------------------------------
#
# Project created by QtCreator 2016-08-03T17:25:38
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CAServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filerecv.cpp \
    rootsign.cpp \
    rvkcert.cpp \
    rootverify.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH +=E:\project\QTProject\OpenSSLCA\include

LIBS+= -LE:\project\QTProject\OpenSSLCA\lib32 -llibeay32
LIBS+= -LE:\project\QTProject\OpenSSLCA\lib32 -lssleay32

RC_FILE = certificate_server.rc
