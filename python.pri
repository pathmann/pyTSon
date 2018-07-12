JENKINS = $$(JENKINS_BUILD)

unix:!mac {
    isEmpty(PYTHONPATH) {
        equals(JENKINS, 1) {
            PYTHONPATH = /home/build/libs/python/3.5.2/install
        }
        else {
            PYTHONPATH = $$PWD/includes/python-352/install
        }
    }

    INCLUDEPATH += $${PYTHONPATH}/include/python3.5m
    LIBS += $${PYTHONPATH}/lib/libpython3.5m.so
    LIBS += -lpthread -ldl -lutil

    contains(QMAKE_HOST.arch, x86_64) {
        QMAKE_POST_LINK += patchelf --replace-needed libpython3.5m.so.1.0 libpython3.5m_64.so ${DESTDIR}${TARGET} &
    }
    else {
        QMAKE_POST_LINK += patchelf --replace-needed libpython3.5m.so.1.0 libpython3.5m_32.so ${DESTDIR}${TARGET} &
    }
}

macx {
    isEmpty(PYTHONPATH) {
        equals(JENKINS, 1) {
            PYTHONPATH = /Users/build/libs/python/3.5.2/install
        }
        else {
            PYTHONPATH = $$PWD/includes/python-352/install
        }
    }

    INCLUDEPATH += $${PYTHONPATH}/include/python3.5m
    LIBS += $${PYTHONPATH}/lib/libpython3.5m.dylib
    LIBS += -lpthread -ldl  -lutil
}

win32 {
    isEmpty(PYTHONPATH) {
        contains(QMAKE_TARGET.arch, x86_64) {
            equals(JENKINS, 1) {
                PYTHONPATH = C:\libs\amd64\python\3.5.2
            }
            else {
                PYTHONPATH = C:\python-352\64bit
            }
        }
        else {
            equals(JENKINS, 1) {
                PYTHONPATH = C:\libs\x86\python\3.5.2
            }
            else {
                PYTHONPATH = C:\python-352\32bit
            }
        }
    }

    LIBS += $${PYTHONPATH}\libs\python35.lib
    INCLUDEPATH += $${PYTHONPATH}\include
}
