unix {
    INCLUDEPATH += $$PWD/includes/python-352/install/include/python3.5m
    LIBS += $$PWD/includes/python-352/install/lib/libpython3.5m.so.1.0
    LIBS += -lpthread -ldl  -lutil
}

win32 {
    contains(QMAKE_TARGET.arch, x64_64) {
        PYTHON_PATH = C:/python-352/64bit
    }
    else {
        PYTHON_PATH = C:/python-352/32bit
    }
    
    INCLUDEPATH += $$PYTHON_PATH/Include
    
    IMPORT_LIBS = $$files($$PYTHON_PATH/*.lib)
    for(FILE, IMPORT_LIBS) {
        LIBS += $$FILE
    }
}
