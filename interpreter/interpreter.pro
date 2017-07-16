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

include ($$PWD/../python.pri)

macx {
  CONFIG -= app_bundle
  QMAKE_POST_LINK += install_name_tool -change $$absolute_path("$${PYTHONPATH}/lib/libpython3.5m.dylib") @loader_path/libpython3.5m.dylib ${DESTDIR}${TARGET} &
}
