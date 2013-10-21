# -------------------------------------------------
# Project created by QtCreator 2010-04-21T15:51:26
# -------------------------------------------------

TARGET = EksReflex
TEMPLATE = lib


include("../EksCore/GeneralOptions.pri")

SOURCES += \
    src/XReflex.cpp

HEADERS += \
    include/XReflex.h

INCLUDEPATH += include/ \
    $$ROOT/Eks/EksCore/include/

LIBS += -lEksCore
















