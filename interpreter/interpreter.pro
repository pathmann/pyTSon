TEMPLATE = app
TARGET = python
QT -= core gui

DESTDIR = $$PWD/../build
OBJECTS_DIR = $$PWD/build
MOC_DIR = $$PWD/build
RCC_DIR = $$PWD/build
UI_DIR = $$PWD/build

HEADERS += \
    pytsonpathfactory.h

SOURCES += main.cpp \
    pytsonpathfactory.cpp

unix:!mac {
    QMAKE_RPATHDIR += $ORIGIN/../
}

include ($$PWD/../python.pri)
