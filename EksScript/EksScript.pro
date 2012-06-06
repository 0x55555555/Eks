#-------------------------------------------------
#
# Project created by QtCreator 2012-02-16T11:35:32
#
#-------------------------------------------------

TARGET = EksScript
TEMPLATE = lib

include("../EksCore/GeneralOptions.pri")

SOURCES += XScriptGlobal.cpp \
    XInterface.cpp \
    XScriptException.cpp \
    XScriptValue.cpp \
    XScriptFunction.cpp \
    XScriptSource.cpp \
    XScriptObject.cpp \
    XQObjectWrapper.cpp \
    XFunctions.cpp \
    XScriptEngine.cpp \
    XQtWrappers.cpp

HEADERS += XScriptGlobal.h \
    XInterface.h \
    XSignature.h \
    XSignatureHelpers.h \
    XSignatureSpecialisations.h \
    XTemplateMetaHelpers.h \
    XScriptTypeInfo.h \
    XConvertToScript.h \
    XConvertFromScript.h \
    XConvertScriptSTL.h \
    XConvert.h \
    XScriptException.h \
    XScriptValue.h \
    XScriptValueV8Internals.h \
    XScriptFunction.h \
    XInterfaceUtilities.h \
    XProperties.h \
    XScriptConstructors.h \
    XConvertToScriptMap.h \
    XFunctions.h \
    XFunctionSpecialisations.h \
    XScriptSource.h \
    XScriptObject.h \
    XQObjectWrapper.h \
    XScriptEngine.h \
    XQtWrappers.h \
    XScriptValueDartInternals.h \
    XScriptDefinitions.h

LIBS += -lQtV8 -lEksCore

INCLUDEPATH += $$ROOT/Eks/EksCore $$ROOT/Eks/EksScript/qtjsbackend/src/3rdparty/v8/include

#DEFINES += X_DART
#LIBS += -L./dart/ -llibdart_builtin -llibdart_withcore -llibdart_lib_withcore -llibdouble_conversion -llibdart_vm -llibjscre

OTHER_FILES += \
    License.txt
