#-------------------------------------------------
#
# Project created by QtCreator 2016-07-21T22:19:23
#
#-------------------------------------------------

QT       += core gui

TARGET = imu_cali
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    win_qextserialport.cpp \
    qextserialport.cpp \
    qextserialbase.cpp

HEADERS  += mainwindow.h \
    win_qextserialport.h \
    qextserialport.h \
    qextserialbase.h

FORMS    += mainwindow.ui
