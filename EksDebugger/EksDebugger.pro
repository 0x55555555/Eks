#-------------------------------------------------
#
# Project created by QtCreator 2012-10-12T19:27:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EksDebugger
TEMPLATE = app

include("../EksCore/GeneralOptions.pri")

SOURCES += main.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


LIBS += -lEksCore -lEksDebug

INCLUDEPATH += $$ROOT/Eks/EksCore \
        $$ROOT/Eks/EksDebug
