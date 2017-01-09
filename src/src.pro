QT       += gui widgets network uitools sql

CONFIG += c++11

TARGET = pyTSon
TEMPLATE = lib

DESTDIR = ../build
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build

INCLUDEPATH += ../includes/pluginsdk/21/include

SOURCES += pytson.cpp \
    pythonhost.cpp \
    global_shared.cpp \
    ts3logdispatcher.cpp \
    pyconversion.cpp

HEADERS += pytson.h \
    pythonhost.h \
    singleton.h \
    global_shared.h \
    ts3logdispatcher.h \
    pyconversion.h

OTHER_FILES += ../templates/ts3module.h.tpl \
    ../templates/ts3module.cpp.tpl \
    ../templates/plugin.h.tpl \
    ../templates/plugin.cpp.tpl \
    ../templates/ts3defines.py.tpl \
    ../templates/ts3plugin.py.tpl \
    ../ressources/pyTSon-configdialog.ui \
    ../ressources/repository.ui \
    ../ressources/repository-manager.ui \
    ../ressources/installer.ui \
    ../ressources/pytsonui.py \
    ../ressources/devtools.py \
    ../ressources/repositorymaster.json \
    ../ressources/pytson.py


unix:!mac {
    QMAKE_RPATHDIR += $ORIGIN/pyTSon
}


########PYTHON SETTINGS############
include (../python.pri)

macx {
    QT_MODULES = Core Gui Widgets Network Sql
    for(m, QT_MODULES) {
        QMAKE_POST_LINK += install_name_tool -change @rpath/libQt5$${m}.5.dylib @executable_path/../Frameworks/libQt5$${m}.dylib $${DESTDIR}/libpyTSon.dylib &&
    }

    QMAKE_POST_LINK += install_name_tool -change $${PYTHONPATH}/lib/libpython3.5m.dylib @loader_path/pyTSon/libpython3.5m.dylib ${DESTDIR}${TARGET} &
}


#########GENERATED SOURCES#########
SOURCES += ../generated/pregen/ts3module.cpp \
    ../generated/pregen/plugin.cpp

HEADERS += ../generated/pregen/ts3module.h \
    ../generated/pregen/plugin.h

INCLUDEPATH += ../generated/pregen


#########PythonQt Settings#########
PYTHONQT_PATH = ../includes/PythonQt

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
