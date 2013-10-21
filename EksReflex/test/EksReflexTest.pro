#-------------------------------------------------
#
# Project created by QtCreator 2013-02-21T09:02:34
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

include("../../EksCore/GeneralOptions.pri")

TARGET = EksReflexTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += XReflexTest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += $$ROOT/Eks/EksCore/include \
    $$ROOT/Eks/EksReflex/include

LIBS += -lEksCore -lEksReflex

HEADERS += \
    XReflexTest.h
