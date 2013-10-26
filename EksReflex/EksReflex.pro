# -------------------------------------------------
# Project created by QtCreator 2010-04-21T15:51:26
# -------------------------------------------------

TARGET = EksReflex
TEMPLATE = lib


include("../EksCore/GeneralOptions.pri")

SOURCES += \
    src/EmbeddedTypes.cpp

HEADERS += \
    include/Reflex/Type.h \
    include/Reflex/Function.h \
    include/Reflex/EmbeddedTypes.h \
    include/Reflex/Global.h \
    include/Reflex/BasicBuilder.h \
    include/Reflex/Class.h

INCLUDEPATH += include/ \
    $$ROOT/Eks/EksCore/include/

LIBS += -lEksCore
















