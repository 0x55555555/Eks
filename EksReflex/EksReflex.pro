# -------------------------------------------------
# Project created by QtCreator 2010-04-21T15:51:26
# -------------------------------------------------

TARGET = EksReflex
TEMPLATE = lib


include("../EksCore/GeneralOptions.pri")

SOURCES += \
    src/EmbeddedTypes.cpp \
    src/NamespaceBuilder.cpp

HEADERS += \
    include/Reflex/Type.h \
    include/Reflex/EmbeddedTypes.h \
    include/Reflex/Global.h \
    include/Reflex/BasicBuilder.h \
    include/Reflex/FunctionBuilder.h \
    include/Reflex/ClassBuilder.h \
    include/Reflex/Symbol.h \
    include/Reflex/NamespaceBuilder.h

INCLUDEPATH += include/ \
    $$ROOT/Eks/EksCore/include/

LIBS += -lEksCore
















