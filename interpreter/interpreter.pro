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
    QMAKE_RPATHDIR += $ORIGIN
}

macx {
  CONFIG -= app_bundle
  QMAKE_POST_LINK += install_name_tool -change $$absolute_path("../includes/python-352/install/lib/libpython3.5m.dylib") @loader_path/libpython3.5m.dylib ${DESTDIR}${TARGET} &
}

include ($$PWD/../python.pri)
