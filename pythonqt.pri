#########PythonQt Settings#########
PYTHONQT_PATH = $$PWD/includes/PythonQt

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
