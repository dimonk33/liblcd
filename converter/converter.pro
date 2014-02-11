#-------------------------------------------------
#
# Project created by QtCreator 2014-02-06T11:49:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fontgen
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h

FORMS    +=

RESOURCES += \
    images.qrc

RC_FILE = ./converter.rc
