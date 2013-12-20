# -------------------------------------------------
# Project created by QtCreator 2009-11-30T14:53:34
# -------------------------------------------------
TARGET = EksGui
TEMPLATE = lib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include("../../EksCore/GeneralOptions.pri")

# moc files are embedded here to prevent error under qt 5 and msvc2012

SOURCES += ../src/XFloatWidget.cpp \
    ../src/XVector2DWidget.cpp \
    ../src/XVector3DWidget.cpp \
    ../src/XVector4DWidget.cpp \
    ../src/XColourWidget.cpp \
    ../src/pickerWidget/qtcolortriangle.cpp

HEADERS += \
    ../include/XFloatWidget.h \
    ../include/XVector2DWidget.h \
    ../include/XVector3DWidget.h \
    ../include/XVector4DWidget.h \
    ../include/XColourWidget.h \
    ../src/pickerWidget/qtcolortriangle.h \
    ../include/XGuiGlobal.h

LIBS += -lEksCore

INCLUDEPATH += $$ROOT/Eks/EksCore/include \
    ../include


