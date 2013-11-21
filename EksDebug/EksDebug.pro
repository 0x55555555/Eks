#-------------------------------------------------
#
# Project created by QtCreator 2012-10-12T19:26:20
#
#-------------------------------------------------

QT       += network

TARGET = EksDebug
TEMPLATE = lib

include("../EksCore/GeneralOptions.pri")

SOURCES += \
    src/XDebugManager.cpp \
    src/XDebugInterface.cpp \
    src/XDebugLogger.cpp \
    src/XDebugManagerImpl.cpp \
    src/XDebugController.cpp

HEADERS += \
    include/XDebugGlobal.h \
    include/XDebugManager.h \
    include/XDebugInterface.h \
    include/XDebugLogger.h \
    include/XDebugManagerImpl.h \
    include/XDebugController.h


LIBS += -lEksCore

INCLUDEPATH += $$ROOT/Eks/EksCore/include $$ROOT/Eks/EksDebug/include
