unix:!mac {
    INCLUDEPATH += $$PWD/includes/python-352/install/include/python3.5m
    LIBS += $$PWD/includes/python-352/install/lib/libpython3.5m.so
    LIBS += -lpthread -ldl -lutil

    contains(QMAKE_HOST.arch, x86_64) {
        QMAKE_POST_LINK += patchelf --replace-needed libpython3.5m.so.1.0 libpython3.5m_64.so ${DESTDIR}${TARGET} &
    }
    else {
        QMAKE_POST_LINK += patchelf --replace-needed libpython3.5m.so.1.0 libpython3.5m_32.so ${DESTDIR}${TARGET} &
    }
}

macx {
    INCLUDEPATH += $$PWD/includes/python-352/install/include/python3.5m
    LIBS += $$PWD/includes/python-352/install/lib/libpython3.5m.dylib
    LIBS += -lpthread -ldl  -lutil

    QMAKE_POST_LINK += install_name_tool -change $$PWD/includes/python-352/install/lib/libpython3.5m.dylib @loader_path/pyTSon/libpython3.5m.dylib ${DESTDIR}${TARGET} &
}

win32 {
    contains(QMAKE_TARGET.arch, x86_64) {
        LIBS += C:\python-352\64bit\libs\python35.lib
        INCLUDEPATH += C:\python-352\64bit\include
    }
    else {
        LIBS += C:\python-352\32bit\libs\python35.lib
        INCLUDEPATH += C:\python-352\32bit\include
    }
}
