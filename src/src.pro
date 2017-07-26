QT       += gui widgets network uitools sql

CONFIG += c++11

TARGET = pyTSon
TEMPLATE = lib

DESTDIR = ../build
OBJECTS_DIR = ../objs
MOC_DIR = ../objs
RCC_DIR = ../objs
UI_DIR = ../objs

INCLUDEPATH += ../includes/pluginsdk/21/include

SOURCES += pytson.cpp \
    pytsonhost.cpp \
    global_shared.cpp \
    ts3logdispatcher.cpp \
    pyconversion.cpp \
    ts3callbackarguments.cpp \
    ../shared/pythonhost.cpp \
    ../shared/pythonqt/eventfilterobject.cpp \
    ../shared/pythonqt/pythonqtpytson.cpp

HEADERS += pytson.h \
    pytsonhost.h \
    singleton.h \
    global_shared.h \
    ts3logdispatcher.h \
    pyconversion.h \
    ts3callbackarguments.h \
    ../shared/pythonhost.h \
    ../shared/pythonqt/eventfilterobject.h \
    ../shared/pythonqt/pythonqtpytson.h

OTHER_FILES += ../templates/ts3lib.h.tpl \
    ../templates/ts3lib.cpp.tpl \
    ../templates/ts3plugin.h.tpl \
    ../templates/ts3plugin.cpp.tpl \
    ../templates/ts3defines.py.tpl \
    ../ressources/ui/pyTSon-configdialog.ui \
    ../ressources/ui/repository.ui \
    ../ressources/ui/repository-manager.ui \
    ../ressources/ui/installer.ui \
    ../ressources/python/pytsonui/__init__.py \
    ../ressources/python/pytsonui/console.py \
    ../ressources/python/pytsonui/config.py \
    ../ressources/python/pytsonui/dialogs.py \
    ../ressources/python/pytsonui/repository.py \
    ../ressources/python/devtools.py \
    ../ressources/repositorymaster.json \
    ../ressources/python/pytson.py \
    ../ressources/python/ts3client.py \
    ../ressources/python/ts3plugin.py \
    ../ressources/python/pluginhost.py \
    ../ressources/python/pythonqtpytson.py \
    ../ressources/python/ts3widgets/__init__.py \
    ../ressources/python/ts3widgets/serverview.py \
    ../ressources/python/ts3widgets/filetransfer.py \
    ../ressources/ui/filebrowser.ui \
    ../ressources/ui/filecollision.ui \
    ../ressources/ui/filetransfer.ui

INCLUDEPATH += ../shared

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
SOURCES += ../generated/pregen/ts3lib.cpp \
    ../generated/pregen/ts3plugin.cpp

HEADERS += ../generated/pregen/ts3lib.h \
    ../generated/pregen/ts3plugin.h

INCLUDEPATH += ../generated/pregen


#########PythonQt Settings#########
include (../pythonqt.pri)
