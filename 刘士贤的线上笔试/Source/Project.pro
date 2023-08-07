#-------------------------------------------------
#
# Project created by QtCreator 2023-08-06T14:09:56
#
#-------------------------------------------------

QT       += core gui network sql
QMAKE_CXXFLAGS += -std=c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Project
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mywidget.cpp \
    mywritethread.cpp

HEADERS  += mainwindow.h \
    mywidget.h \
    mywritethread.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

OTHER_FILES += \
    icon_config.rc

RC_FILE += icon_config.rc
