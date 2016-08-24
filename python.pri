unix:!mac {
    INCLUDEPATH += $$PWD/includes/python-352/install/include/python3.5m
    LIBS += $$PWD/includes/python-352/install/lib/libpython3.5m.so.1.0
    LIBS += -lpthread -ldl  -lutil
}

macx {
    INCLUDEPATH += $$PWD/includes/python-352/install/include/python3.5m
    LIBS += $$PWD/includes/python-352/install/lib/libpython3.5m.dylib
    LIBS += -lpthread -ldl  -lutil
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
