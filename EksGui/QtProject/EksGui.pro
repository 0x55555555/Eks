# -------------------------------------------------
# Project created by QtCreator 2009-11-30T14:53:34
# -------------------------------------------------
TARGET = EksGui
TEMPLATE = lib

QT += widgets

include("../../EksCore/GeneralOptions.pri")

# moc files are embedded here to prevent error under qt 5 and msvc2012

SOURCES += ../src/XFloatWidget.cpp \
    ../src/XVector2DWidget.cpp \
    ../src/XVector3DWidget.cpp \
    ../src/XVector4DWidget.cpp \
    ../src/XColourWidget.cpp \
    ../src/pickerWidget/qtcolortriangle.cpp

HEADERS += \
    ../XFloatWidget \
    ../XVector2DWidget \
    ../XVector3DWidget \
    ../XVector4DWidget \
    ../XColourWidget \
    ../src/pickerWidget/qtcolortriangle.h \
    ../XGuiGlobal

LIBS += -lEksCore

INCLUDEPATH += $$ROOT/Eks/EksCore \
    ../


