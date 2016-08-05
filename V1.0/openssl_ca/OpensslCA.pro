#-------------------------------------------------
#
# Project created by QtCreator 2015-12-15T21:37:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpensslCA
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    careq.cpp \
    verify.cpp \
    detail.cpp \
    creatcrt.cpp \
    revoked.cpp \
    iojson.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#INCLUDEPATH +=G:\Git\GitOSC\include
#LIBS+= -LG:\Git\GitOSC\lib32 -llibeay32
#LIBS+= -LG:\Git\GitOSC\lib32 -lssleay32

INCLUDEPATH +=E:\git\OpenSSLCA_program\include

LIBS+= -LE:\git\OpenSSLCA_program\lib32 -llibeay32
LIBS+= -LE:\git\OpenSSLCA_program\lib32 -lssleay32

DISTFILES += \
    ReadMe

RC_FILE = Book_UI.rc

DISTFILES += \
    Book_UI.rc
