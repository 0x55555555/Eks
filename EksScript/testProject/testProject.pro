#-------------------------------------------------
#
# Project created by QtCreator 2012-02-16T11:35:58
#
#-------------------------------------------------

QT       += core

TEMPLATE = app
TARGET = testProject

include("../../EksCore/GeneralOptions.pri")

SOURCES += main.cpp \
    testobject.cpp

LIBS += -lEksCore -lEksScript
INCLUDEPATH += $$ROOT/EksScript/ $$ROOT/EksCore

HEADERS += \
    testobject.h
