TEMPLATE = app
TARGET = pythonqt_app

CONFIG += c++11

QT += gui widgets network uitools sql

DESTDIR = ../build
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build

include (../python.pri)
include (../pythonqt.pri)

SOURCES += main.cpp \
    dialog.cpp \
    ../shared/pythonhost.cpp \
    ../shared/pythonqt/eventfilterobject.cpp \
    ../shared/pythonqt/pythonqtpytson.cpp \
    dummyts3lib.cpp \
    global_shared.cpp

HEADERS += \
    dialog.h \
    ../shared/pythonhost.h \
    ../shared/pythonqt/eventfilterobject.h \
    ../shared/pythonqt/pythonqtpytson.h \
    pythonqtapphost.h \
    dummyts3lib.h \
    global_shared.h

FORMS += \
    dialog.ui

unix {
    QMAKE_RPATHDIR += $ORIGIN
}

macx {
  CONFIG -= app_bundle
  QMAKE_POST_LINK += install_name_tool -change $$absolute_path("$${PYTHONPATH}/lib/libpython3.5m.dylib") @loader_path/libpython3.5m.dylib ${DESTDIR}${TARGET} &
}

INCLUDEPATH += ../shared
