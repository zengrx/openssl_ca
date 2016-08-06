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
    filerecv.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RC_FILE = certificate_server.rc
