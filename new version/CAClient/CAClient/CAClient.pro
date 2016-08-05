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
    certreq.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH +=E:\git\OpenSSLCA_program\include

LIBS+= -LE:\git\OpenSSLCA_program\lib32 -llibeay32
LIBS+= -LE:\git\OpenSSLCA_program\lib32 -lssleay32
