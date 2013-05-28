#-------------------------------------------------
#
# Project created by QtCreator 2012-02-16T11:35:32
#
#-------------------------------------------------

TARGET = EksScript
TEMPLATE = lib

QT += widgets

include("../EksCore/GeneralOptions.pri")

SOURCES += \
    XInterface.cpp \
    XScriptValue.cpp \
    XScriptFunction.cpp \
    XScriptSource.cpp \
    XScriptObject.cpp \
    XQObjectWrapper.cpp \
    XScriptEngine.cpp \
    XQtWrappers.cpp \
    Engines/XScriptDartEngine.cpp \
    Engines/XScriptJavascriptEngine.cpp \
    XScriptInterfaceBase.cpp

HEADERS += XScriptGlobal.h \
    XScriptTypeInfo.h \
    XScriptValue.h \
    XScriptFunction.h \
    XInterfaceUtilities.h \
    XScriptSource.h \
    XScriptObject.h \
    XQObjectWrapper.h \
    XScriptEngine.h \
    XQtWrappers.h \
    XScriptDefinitions.h \
    XScriptInterfaceBase.h \
    XScriptFunctionWrapper.h \
    XScriptInterface.h

LIBS += -lEksCore

INCLUDEPATH += $$ROOT/Eks/EksCore ./

#QT += v8-private
#define X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT

#win32-msvc2010 {
#  DEFINES += X_SCRIPT_ENGINE_ENABLE_DART
#  LIBS += -L./dart/ -llibdart_builtin -llibdart_withcore -llibdart_lib_withcore -llibdouble_conversion -llibdart_vm -llibjscre
#}

OTHER_FILES +=
