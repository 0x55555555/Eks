#-------------------------------------------------
#
# Project created by QtCreator 2012-10-12T19:26:20
#
#-------------------------------------------------

QT       += network
QT       -= gui

TARGET = EksDebug
TEMPLATE = lib

include("../EksCore/GeneralOptions.pri")

SOURCES += \
    XDebugManager.cpp \
    XDebugInterface.cpp \
    XDebugLogger.cpp \
    XDebugManagerImpl.cpp \
    XDebugController.cpp

HEADERS += \
    XDebugGlobal.h \
    XDebugManager.h \
    XDebugInterface.h \
    XDebugLogger.h \
    XDebugManagerImpl.h \
    XDebugController.h


LIBS += -lEksCore

INCLUDEPATH += $$ROOT/Eks/EksCore
