# -------------------------------------------------
# Project created by QtCreator 2009-11-30T14:53:34
# -------------------------------------------------
TARGET = EksGui
TEMPLATE = lib

include("../../EksCore/GeneralOptions.pri")

SOURCES += ../src/XFloatWidget.cpp \
    ../src/XVector2DWidget.cpp \
    ../src/Eks::Vector3DWidget.cpp \
    ../src/XVector4DWidget.cpp \
    ../src/ColourWidget.cpp \
    ../src/pickerWidget/qtcolortriangle.cpp \
    ../src/XProfileWidget.cpp

HEADERS += ../XFloatWidget \
    ../XVector2DWidget \
    ../Eks::Vector3DWidget \
    ../XVector4DWidget \
    ../ColourWidget \
    ../src/pickerWidget/qtcolortriangle.h \
    ../XGuiGlobal \
    ../XProfileWidget

LIBS += -lEksCore

INCLUDEPATH += $$ROOT/Eks/EksCore \
    ../


