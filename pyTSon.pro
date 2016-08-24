QT       += gui widgets network uitools sql

CONFIG += c++11

TARGET = pyTSon
TEMPLATE = lib

DESTDIR = $$PWD/build
OBJECTS_DIR = $$PWD/build
MOC_DIR = $$PWD/build
RCC_DIR = $$PWD/build
UI_DIR = $$PWD/build

INCLUDEPATH += src
INCLUDEPATH += includes/pluginsdk/21/include

SOURCES += src/pytson.cpp \
    src/pythonhost.cpp \
    src/global_shared.cpp \
    src/ts3logdispatcher.cpp \
    src/pyconversion.cpp

HEADERS += src/pytson.h \
    src/pythonhost.h \
    src/singleton.h \
    src/global_shared.h \
    src/ts3logdispatcher.h \
    src/pyconversion.h

macx {
    QT_MODULES = Core Gui Widgets Network Sql
    for(m, QT_MODULES) {
        QMAKE_POST_LINK += install_name_tool -change @rpath/libQt5$${m}.5.dylib @executable_path/../Frameworks/libQt5$${m}.dylib ${DESTDIR}${TARGET} &
    }
}

########PYTHON SETTINGS############
include ($$PWD/python.pri)


#########GENERATED SOURCES#########
SOURCES += generated/pregen/ts3module.cpp \
    generated/pregen/plugin.cpp

HEADERS += generated/pregen/ts3module.h \
    generated/pregen/plugin.h

INCLUDEPATH += generated/pregen


#########PythonQt Settings#########
PYTHONQT_PATH = $${PWD}/includes/PythonQt

INCLUDEPATH += $${PYTHONQT_PATH}/src \
  $${PYTHONQT_PATH}/extensions/PythonQt_QtAll

include ( $${PYTHONQT_PATH}/src/src.pri )

include( $${PYTHONQT_PATH}/generated_cpp/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin.pri)
include( $${PYTHONQT_PATH}/generated_cpp/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin.pri)


#########PythonQt_QtAll Settings#########
HEADERS +=                \
  $${PYTHONQT_PATH}/extensions/PythonQt_QtAll/PythonQt_QtAll.h

SOURCES +=                \
  $${PYTHONQT_PATH}/extensions/PythonQt_QtAll/PythonQt_QtAll.cpp

include ($${PYTHONQT_PATH}/generated_cpp/com_trolltech_qt_core/com_trolltech_qt_core.pri)
include ($${PYTHONQT_PATH}/generated_cpp/com_trolltech_qt_gui/com_trolltech_qt_gui.pri)
include ($${PYTHONQT_PATH}/generated_cpp/com_trolltech_qt_sql/com_trolltech_qt_sql.pri)
include ($${PYTHONQT_PATH}/generated_cpp/com_trolltech_qt_network/com_trolltech_qt_network.pri)
include ($${PYTHONQT_PATH}/generated_cpp/com_trolltech_qt_uitools/com_trolltech_qt_uitools.pri)
