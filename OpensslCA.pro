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
    creatcrt.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH +=..\include

LIBS+= -L..\lib32 -llibeay32
LIBS+= -L..\lib32 -lssleay32
