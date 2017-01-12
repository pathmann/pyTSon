#include "pythonhost.h"

#include <stdlib.h>
#include <Python.h>

#include <QWidget>

#include "PythonQt.h"
#include "PythonQt_QtAll.h"

#include "pyconversion.h"
#include "global_shared.h"
#include "ts3logdispatcher.h"
#include "ts3module.h"


#if defined(Q_OS_WIN)
  #define INTERPRETER "python.exe"
#else
  //mac + linux
  #define INTERPRETER "python"
#endif

PythonHost::PythonHost(): m_interpreter(NULL), m_pmod(NULL), m_pyhost(NULL), m_callmeth(NULL), m_trace(NULL), m_inited(false) {

}

PythonHost::~PythonHost() {
  if (m_interpreter)
    PyMem_RawFree(m_interpreter);
}

bool PythonHost::setupDirectories(QString &error) {
  char path[256];
  ts3_funcs.getPluginPath(path, 256, ts3_pluginid);

  m_base.setPath(path);

  if (!m_base.exists()) {
    error = QObject::tr("Error getting pluginpath");
    return false;
  }

  if (!m_base.cd("pyTSon")) {
    error = QObject::tr("Error changing directory to plugin directory");
    return false;
  }

  QString interpreterpath = m_base.absoluteFilePath(INTERPRETER);
  if (!QFile::exists(interpreterpath)) {
    error = QObject::tr("Interpreter not found in").arg(interpreterpath);
    return false;
  }

  m_interpreter = Py_DecodeLocale(interpreterpath.toUtf8().data(), NULL);
  if (!m_interpreter) {
    error = QObject::tr("Error decoding interpreter path");
    return false;
  }

  m_scriptsdir = m_base;
  if (!m_scriptsdir.cd("scripts")) {
    error = QObject::tr("Error changing directory to scripts directory");
    return false;
  }

  m_includedir = m_base;
  if (!m_includedir.cd("include")) {
    error = QObject::tr("Error changing directory to include directory");
    return false;
  }

  m_libdir = m_base;
  bool havelib = true;
  if (!m_libdir.cd("lib"))
    havelib = false;

  /* the plugin was updated, so we need to replace the old libdir */
  QDir newincludelibdir = m_base;
  if (newincludelibdir.cd("lib_new")) {
    //delete the old lib
    if (havelib) {
      if (!m_libdir.removeRecursively()) {
        error = QObject::tr("Error deleting old lib directory");
        return false;
      }
    }

    //move the new lib dir
    QString newpath = m_libdir.absolutePath();
    if (!havelib) {
      newpath += QDir::separator();
      newpath += "lib";
    }

    if (!newincludelibdir.rename(newincludelibdir.path(), newpath)) {
      error = QObject::tr("Error moving new lib directory");
      return false;
    }

    if (!havelib) {
      if (!m_libdir.cd("lib")) {
        error = QObject::tr("Error changing directory to new installed lib directory");
        return false;
      }
    }

    ts3logdispatcher::instance()->add(QObject::tr("New lib directory installed"), LogLevel_INFO);
  }

  if (!m_libdir.cd("python3.5")) {
    error = QObject::tr("Error changing directory to standard lib directory");
    return false;
  }

  m_sitepackdir = m_libdir;
  if (!m_sitepackdir.cd("site-packages")) {
    error = QObject::tr("Error changing directory to site directory");
    return false;
  }

  m_dynloaddir = m_libdir;
  if (!m_dynloaddir.cd("lib-dynload")) {
    error = QObject::tr("Error changing directory to dynload directory");
    return false;
  }

  return true;
}

bool PythonHost::isReady() {
  return (m_inited && m_pmod && m_pyhost && m_callmeth && m_trace);
}

QString PythonHost::formatError(const QString &fallback) {
  if (!m_trace || !PyErr_Occurred())
    return fallback;

  QString error(fallback + "\n");
  PyObject* pyerrtype = NULL, *pyerr = NULL, *pytrace = NULL;
  PyErr_Fetch(&pyerrtype, &pyerr, &pytrace);
  PyErr_NormalizeException(&pyerrtype, &pyerr, &pytrace);

  bool success = false;
  PyObject* formatfunc;
  if (pytrace)
    formatfunc = PyObject_GetAttrString(m_trace, "format_exception");
  else formatfunc = PyObject_GetAttrString(m_trace, "format_exception_only");

  if (formatfunc && PyCallable_Check(formatfunc)) {
    PyObject* ret;
    if (pytrace)
      ret = PyObject_CallFunction(formatfunc, const_cast<char*>("OOO"), pyerrtype, pyerr, pytrace);
    else ret = PyObject_CallFunction(formatfunc, const_cast<char*>("OO"), pyerrtype, pyerr);

    if (ret && PyList_Check(ret)) {
      for (int i = 0; i < PyList_Size(ret); ++i)
        error.append(PyUnicode_AsUTF8(PyList_GetItem(ret, i)));

      success = true;
    }
    Py_XDECREF(ret);
  }
  Py_XDECREF(formatfunc);


  Py_XDECREF(pyerrtype);
  Py_XDECREF(pyerr);
  Py_XDECREF(pytrace);

  if (success)
    return error;
  else return fallback;
}

bool PythonHost::setSysPath(QString& error) {
  PyObject* syspath = PyList_New(5);
  if (!syspath) {
    error = QObject::tr("Memory error");
    return false;
  }

  QDir pathdirs[] = {m_scriptsdir, m_includedir, m_libdir, m_dynloaddir, m_sitepackdir};
  PyObject* pypath = NULL;
  for (unsigned int i = 0; i < std::extent<decltype(pathdirs)>::value; ++i) {
    pypath = Py_BuildValue("s", pathdirs[i].absolutePath().toUtf8().data());
    if (!pypath) {
      error = QObject::tr("Error creating directory string from path %1").arg(i);
      Py_DECREF(syspath);
      return false;
    }

    if (PyList_SetItem(syspath, i, pypath) != 0) {
      error = QObject::tr("Error adding dir %1 to sys.path").arg(i);
      Py_DECREF(pypath);
      Py_DECREF(syspath);
      return false;
    }
  }

  //replace sys.path
  if (PySys_SetObject("path", syspath) != 0) {
    error = QObject::tr("Error setting sys.path");
    Py_DECREF(syspath);
    return false;
  }

  return true;
}

bool PythonHost::setModuleSearchpath(QString& error) {
  QString libdir = m_libdir.absolutePath();
#ifdef Q_OS_WIN
  libdir += ";";
#else
  libdir += ":";
#endif
  libdir += m_dynloaddir.absolutePath();

  wchar_t* wlibdir = Py_DecodeLocale(libdir.toUtf8().data(), NULL);
  if (!wlibdir) {
    error = QObject::tr("Error decoding module search path");
    return false;
  }
  Py_SetPath(wlibdir);
  PyMem_RawFree(wlibdir);

  return true;
}

bool PythonHost::init(QString& error) {
  if (!setupDirectories(error))
    return false;

  if (PyImport_AppendInittab("ts3lib", &PyInit_ts3lib) == -1) {
    ts3logdispatcher::instance()->add(QObject::tr("Error initializing ts3 module"), LogLevel_ERROR);
    return false;
  }

  Py_FrozenFlag = 1;
  Py_IgnoreEnvironmentFlag = 1;
  Py_SetProgramName(m_interpreter);
  Py_NoUserSiteDirectory = 1;

  if (!setModuleSearchpath(error))
    return false;

  Py_Initialize();
  if (PyErr_Occurred()) {
    PyErr_Print();
    return false;
  }

  if (!setSysPath(error))
    return false;

  PythonQt::init(PythonQt::PythonAlreadyInitialized);
  PythonQt_QtAll::init();

  m_trace = PyImport_ImportModule("traceback");
  if (!m_trace) {
    error = QObject::tr("Error importing traceback module");
    return false;
  }

  //import ts3plugin module
  m_pmod = PyImport_ImportModule("ts3plugin");
  if (!m_pmod) {
    error = formatError(QObject::tr("Error importing ts3plugin module"));
    return false;
  }

  //get PluginHost of ts3plugin module
  m_pyhost = PyObject_GetAttrString(m_pmod, "PluginHost");
  if (!m_pyhost) {
    error = formatError(QObject::tr("Error getting PluginHost class"));
    return false;
  }

  m_callmeth = PyObject_GetAttrString(m_pyhost, "callMethod");
  if (!m_callmeth) {
    error = formatError(QObject::tr("Error getting PluginHost.callMethod"));
    return false;
  }

  //call PluginHost.init method
  PyObject* ret = PyObject_CallMethod(m_pyhost, const_cast<char*>("init"), const_cast<char *>(""));
  if (!ret) {
    error = formatError(QObject::tr("Error calling PluginHost.init"));
    return false;
  }
  else Py_DECREF(ret);

  m_inited = true;
  return true;
}

void PythonHost::shutdown() {
  if (isReady()) {
    //call PluginHost.shutdown method
    PyObject* ret = PyObject_CallMethod(m_pyhost, const_cast<char*>("shutdown"), const_cast<char *>(""));
    if (!ret)
      ts3logdispatcher::instance()->add(QObject::tr("Error calling PluginHost.shutdown"), LogLevel_ERROR);
    else Py_DECREF(ret);
  }

  m_inited = false;

  Py_XDECREF(m_callmeth);
  m_callmeth = NULL;

  Py_XDECREF(m_pyhost);
  m_pyhost = NULL;

  Py_XDECREF(m_pmod);
  m_pmod = NULL;

  Py_XDECREF(m_trace);
  m_trace = NULL;

  PythonQt::cleanup();

  if (Py_IsInitialized())
    Py_Finalize();
}

void PythonHost::freeMemory(void* data) {
  if (data)
    free(data);
}

void PythonHost::configure(void* qParentWidget) {
  PyObject* pywidget = PythonQt::priv()->wrapQObject((QWidget*)qParentWidget);
  if (!pywidget) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling configure failed, qParentWidget not wrapped"), LogLevel_ERROR);
    return;
  }

  QString errstr;
  if (!callMethod(NULL, errstr, "(sO)", "configure", pywidget))
    ts3logdispatcher::instance()->add(QObject::tr("Calling configure failed with error \"%1\"").arg(errstr), LogLevel_ERROR);
  Py_DECREF(pywidget);
}

int PythonHost::processCommand(uint64 schid, const char* command) {
  PyObject* pyret;
  QString error;
  if (!callMethod(&pyret, error, "(sKs)", "processCommand", schid, command)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling processCommand failed with error \"%1\"").arg(error), LogLevel_ERROR);
    return 1;
  }

  if (PyObject_IsTrue(pyret)) {
    Py_DECREF(pyret);
    return 0;
  }
  else {
    Py_DECREF(pyret);
    return 1;
  }
}

void PythonHost::infoData(uint64 schid, uint64 id, enum PluginItemType type, char** data) {
  PyObject* pyret;
  QString error;
  if (!callMethod(&pyret, error, "(sKKI)", "infoData", (unsigned long long)schid, (unsigned long long)id, (unsigned int)type)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling infoData failed with error \"%1\"").arg(error), LogLevel_ERROR);
    return;
  }

  //pyret should be a list of strings
  if (!PyList_Check(pyret)) {
    Py_DECREF(pyret);
    ts3logdispatcher::instance()->add(QObject::tr("Calling infoData failed, no list returned"), LogLevel_ERROR);
    return;
  }

  PyObject* pystr;
  QString str;
  for (int i = 0; i < PyList_Size(pyret); ++i) {
    pystr = PyList_GetItem(pyret, i);

    if (!pystr || !PyUnicode_Check(pystr)) {
      Py_DECREF(pyret);
      ts3logdispatcher::instance()->add(QObject::tr("Calling infoData failed, no list of strings returned"), LogLevel_ERROR);
      return;
    }

    str.append(PyUnicode_AsUTF8(pystr));
    str.append("\n");
  }
  Py_DECREF(pyret);

  *data = (char*)malloc((str.length() +1) * sizeof(char));
  strncpy(*data, str.toUtf8().data(), str.size() +1);
}

void PythonHost::initMenus(struct PluginMenuItem*** menuItems, char** menuIcon) {
  PyObject* pyret;
  QString error;
  if (!callMethod(&pyret, error, "(s)", "initMenus")) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling initMenus failed with error \"%1\"").arg(error), LogLevel_ERROR);
    return;
  }

  //return type: [(ts3defines.PLUGIN_MENU_TYPE_CLIENT, id, "text", "icon.png")]
  if (!PyList_Check(pyret)) {
    Py_DECREF(pyret);
    ts3logdispatcher::instance()->add(QObject::tr("Calling initMenus failed, no list returned"), LogLevel_ERROR);
    return;
  }

  *menuItems = (struct PluginMenuItem**)malloc((PyList_Size(pyret) +1) * sizeof(struct PluginMenuItem*));
  PyObject* tup;
  struct PluginMenuItem* item;
  for (int i = 0; i < PyList_Size(pyret); ++i) {
    tup = PyList_GetItem(pyret, i);

    if (!tup || !PyTuple_Check(tup) || PyTuple_Size(tup) != 4) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling initMenus failed, no list of tuples(4) returned"), LogLevel_ERROR);
      Py_DECREF(pyret);
      for (int k = i -1; k >= 0; --k)
        free((*menuItems)[k]);
      free(*menuItems);
      *menuItems = NULL;
      return;
    }

    if (!PyLong_Check(PyTuple_GetItem(tup, 0)) || !PyLong_Check(PyTuple_GetItem(tup, 1)) || !PyUnicode_Check(PyTuple_GetItem(tup, 2)) || !PyUnicode_Check(PyTuple_GetItem(tup, 3))) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling initMenus failed, no list of tuples(long, long, str, str) returned"), LogLevel_ERROR);
      Py_DECREF(pyret);
      for (int k = i -1; k >= 0; --k)
        free((*menuItems)[k]);
      free(*menuItems);
      *menuItems = NULL;
      return;
    }

    item = (struct PluginMenuItem*)malloc(sizeof(struct PluginMenuItem));
    item->type = (PluginMenuType)PyLong_AsLong(PyTuple_GetItem(tup, 0));
    item->id = (int)PyLong_AsLong(PyTuple_GetItem(tup, 1));
    strncpy(item->text, PyUnicode_AsUTF8(PyTuple_GetItem(tup, 2)), PLUGIN_MENU_BUFSZ);
    strncpy(item->icon, PyUnicode_AsUTF8(PyTuple_GetItem(tup, 3)), PLUGIN_MENU_BUFSZ);
    (*menuItems)[i] = item;
  }

  (*menuItems)[PyList_Size(pyret)] = NULL;
  *menuIcon = (char*)malloc((strlen("pyTSon.png") +1)* sizeof(char));
  strcpy(*menuIcon, "pyTSon.png");

  Py_DECREF(pyret);
}

void PythonHost::initHotkeys(struct PluginHotkey*** hotkeys) {
  *hotkeys = NULL;

  PyObject* pyret = NULL;
  QString error;
  if (!callMethod(&pyret, error, "(s)", "initHotkeys")) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling initHotkeys failed with error \"%1\"").arg(error), LogLevel_ERROR);
    return;
  }

  //return type: [("keyword", "description")]
  if (!PyList_Check(pyret)) {
    Py_DECREF(pyret);
    ts3logdispatcher::instance()->add(QObject::tr("Calling initHotkeys failed, no list returned"), LogLevel_ERROR);
    return;
  }

  if (PyList_Size(pyret) == 0) {
    Py_DECREF(pyret);
    return;
  }

  *hotkeys = (struct PluginHotkey**)malloc((PyList_Size(pyret) +1) * sizeof(struct PluginHotkey*));

  PyObject* tup;
  struct PluginHotkey* item;
  for (int i = 0; i < PyList_Size(pyret); ++i) {
    tup = PyList_GetItem(pyret, i);

    if (!tup || !PyTuple_Check(tup) || PyTuple_Size(tup) != 2) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling initHotkeys failed, no list of tuples(2) returned"), LogLevel_ERROR);
      Py_DECREF(pyret);
      for (int k = i -1; k >= 0; --i)
        free((*hotkeys)[k]);
      free(*hotkeys);
      *hotkeys = NULL;
      return;
    }

    if (!PyUnicode_Check(PyTuple_GetItem(tup, 0)) || !PyUnicode_Check(PyTuple_GetItem(tup, 1))) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling initHotkeys failed, no list of tuples(str, str) returned"), LogLevel_ERROR);
      Py_DECREF(pyret);
      for (int k = i -1; k >= 0; --i)
        free((*hotkeys)[k]);
      free(*hotkeys);
      *hotkeys = NULL;
      return;
    }

    item = (struct PluginHotkey*)malloc(sizeof(struct PluginHotkey));
    strncpy(item->keyword, PyUnicode_AsUTF8(PyTuple_GetItem(tup, 0)), PLUGIN_HOTKEY_BUFSZ);
    strncpy(item->description, PyUnicode_AsUTF8(PyTuple_GetItem(tup, 1)), PLUGIN_HOTKEY_BUFSZ);
    (*hotkeys)[i] = item;
  }
  Py_DECREF(pyret);

  (*hotkeys)[PyList_Size(pyret)] = NULL;
}

int PythonHost::onServerErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
  PyObject* pyret;
  QString errstr;
  if (!callMethod(&pyret, errstr, "(sKsIss)", "onServerErrorEvent", (unsigned long long)schid, errorMessage, error, returnCode, extraMessage)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onServerErrorEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    return 1;
  }

  if (PyObject_IsTrue(pyret)) {
    Py_DECREF(pyret);
    return 1;
  }
  else {
    Py_DECREF(pyret);
    return 0;
  }
}

int PythonHost::onTextMessageEvent(uint64 schid, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored) {
  PyObject* pyret;
  QString errstr;
  if (!callMethod(&pyret, errstr, "(sKIIIsssi)", "onTextMessageEvent", (unsigned long long)schid, (unsigned int)targetMode, (unsigned int)toID, (unsigned int)fromID, fromName, fromUniqueIdentifier, message, ffIgnored)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onTextMessageEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    return 1;
  }

  if (PyObject_IsTrue(pyret)) {
    Py_DECREF(pyret);
    return 1;
  }
  else {
    Py_DECREF(pyret);
    return 0;
  }
}

int PythonHost::onClientPokeEvent(uint64 schid, anyID fromClientID, const char* pokerName, const char* pokerUniqueIdentity, const char* message, int ffIgnored) {
  PyObject* pyret;
  QString errstr;
  if (!callMethod(&pyret, errstr, "(sKIsssi)", "onClientPokeEvent", (unsigned long long)schid, (unsigned int)fromClientID, pokerName, pokerUniqueIdentity, message, ffIgnored)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onClientPokeEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    return 1;
  }

  if (PyObject_IsTrue(pyret)) {
    Py_DECREF(pyret);
    return 1;
  }
  else {
    Py_DECREF(pyret);
    return 0;
  }
}

int PythonHost::onServerPermissionErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, unsigned int failedPermissionID) {
  PyObject* pyret;
  QString errstr;
  if (!callMethod(&pyret, errstr, "(sKsIsI)", "onServerPermissionErrorEvent", (unsigned long long)schid, errorMessage, error, returnCode, failedPermissionID)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onServerPermissionErrorEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    return 1;
  }

  if (PyObject_IsTrue(pyret)) {
    Py_DECREF(pyret);
    return 1;
  }
  else {
    Py_DECREF(pyret);
    return 0;
  }
}

void PythonHost::onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString) {
  return;
  QString callerror;
  if (!PythonHost::instance()->callMethod(NULL, callerror, "(ssisKss)", "onUserLoggingMessageEvent",  logMessage,  logLevel,  logChannel, (unsigned long long) logID,  logTime,  completeLogString)) {
    printf("%s\n", QObject::tr("Calling onUserLoggingMessageEvent failed with error \"%1\"\n").arg(callerror).toUtf8().data());
  }
}

void PythonHost::onEditPlaybackVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels) {
  return;
  QString errstr;
  PyObject* pysamples;
  if (!arrayToPyList<short>(samples, "h", errstr, &pysamples, sampleCount)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPlaybackVoiceDataEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    return;
  }

  PyObject* pyret;
  if (!callMethod(&pyret, errstr, "(sKIOi)", "onEditPlaybackVoiceDataEvent", (unsigned long long)schid, (unsigned int)clientID, pysamples, channels)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPlaybackVoiceDataEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    Py_DECREF(pysamples);
    return;
  }
  Py_DECREF(pysamples);

  if (!PyTuple_Check(pyret) || PyTuple_Size(pyret) != 2) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPlaybackVoiceDataEvent failed, no tuple(2) returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  PyObject* pychanged = PyTuple_GetItem(pyret, 0);
  pysamples = PyTuple_GetItem(pyret, 1);

  if (!PyBool_Check(pychanged) || !PyList_Check(pysamples)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPlaybackVoiceDataEvent failed, no tuple(bool, list) returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  if (PyList_Size(pysamples) != sampleCount) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPlaybackVoiceDataEvent failed, no valid list returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  if (PyObject_IsTrue(pychanged)) {
    PyObject* item;
    for (int i = 0; i < sampleCount; ++i) {
      item = PyList_GetItem(pyret, i);

      if (!item || !PyLong_Check(item)) {
        ts3logdispatcher::instance()->add(QObject::tr("Error in onEditPlaybackVoiceDataEvent, no valid long in list, will be ignored"), LogLevel_ERROR, schid);
        continue;
      }

      samples[i] = (short)PyLong_AsLong(item);
    }
  }
  Py_DECREF(pyret);
}

void PythonHost::onEditPostProcessVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
  return;
  PyObject* pysamples = PyList_New(sampleCount);
  if (!pysamples) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPostProcessVoiceDataEvent failed, memory error"), LogLevel_ERROR, schid);
    return;
  }

  PyObject* pysample;
  for (int i = 0; i < sampleCount; ++i) {
    pysample = Py_BuildValue("h", samples[i]);

    if (!pysample) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPostProcessVoiceDataEvent failed, memory error (2)"), LogLevel_ERROR, schid);
      Py_DECREF(pysamples);
      return;
    }

    if (PyList_SetItem(pysamples, i, pysample) != 0) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPostProcessVoiceDataEvent failed, list error"), LogLevel_ERROR, schid);
      Py_DECREF(pysample);
      Py_DECREF(pysamples);
      return;
    }
  }

  PyObject* pychannelSpeakerArray = PyList_New(channels);
  if (!pychannelSpeakerArray) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPostProcessVoiceDataEvent failed, memory error (3)"), LogLevel_ERROR, schid);
    Py_DECREF(pysamples);
    return;
  }

  PyObject* pyspeaker;
  for (int i = 0; i < channels; ++i) {
    pyspeaker = Py_BuildValue("I", channelSpeakerArray[i]);

    if (!pyspeaker) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPostProcessVoiceDataEvent failed, memory error (4)"), LogLevel_ERROR, schid);
      Py_DECREF(pysamples);
      Py_DECREF(pychannelSpeakerArray);
      return;
    }

    if (PyList_SetItem(pychannelSpeakerArray, i, pyspeaker) != 0) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPostProcessVoiceDataEvent failed, list error (2)"), LogLevel_ERROR, schid);
      Py_DECREF(pysamples);
      Py_DECREF(pyspeaker);
      Py_DECREF(pychannelSpeakerArray);
    }
  }

  PyObject* pyret;
  QString errstr;
  if (!callMethod(&pyret, errstr, "(sKIOiOI)", "onEditPostProcessVoiceDataEvent", (unsigned long long)schid, (unsigned int)clientID, pysamples, channels, pychannelSpeakerArray, *channelFillMask)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPostProcessVoiceDataEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    Py_DECREF(pysamples);
    Py_DECREF(pychannelSpeakerArray);
    return;
  }
  Py_DECREF(pysamples);
  Py_DECREF(pychannelSpeakerArray);

  if (!PyTuple_Check(pyret) || PyTuple_Size(pyret) != 3) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPostProcessVoiceDataEvent failed, no tuple(3) returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  PyObject* pychanged = PyTuple_GetItem(pyret, 0);
  pysamples = PyTuple_GetItem(pyret, 1);
  PyObject* pyfillMask = PyTuple_GetItem(pyret, 2);

  if (!PyBool_Check(pychanged) || !PyList_Check(pysamples) || !PyLong_Check(pyfillMask)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPostProcessVoiceDataEvent failed, no tuple(bool, list, int) returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  if (PyList_Size(pysamples) != sampleCount) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditPostProcessVoiceDataEvent failed, no valid list returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  if (PyObject_IsTrue(pychanged)) {
    PyObject* item;
    for (int i = 0; i < PyList_Size(pysamples); ++i) {
      item = PyList_GetItem(pysamples, i);

      if (!item || !PyLong_Check(item)) {
        ts3logdispatcher::instance()->add(QObject::tr("Error in onEditPostProcessVoiceDataEvent, no valid long in list, will be ignored"), LogLevel_ERROR, schid);
        continue;
      }

      samples[i] = (short)PyLong_AsLong(item);
    }

    *channelFillMask = (unsigned int)PyLong_AsUnsignedLong(pyfillMask);
  }
  Py_DECREF(pyret);
}

void PythonHost::onEditMixedPlaybackVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
  return;
  PyObject* pysamples = PyList_New(sampleCount);
  if (!pysamples) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditMixedPlaybackVoiceDataEvent failed, memory error"), LogLevel_ERROR, schid);
    return;
  }

  PyObject* pysample;
  for (int i = 0; i < sampleCount; ++i) {
    pysample = Py_BuildValue("h", samples[i]);

    if (!pysample) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling onEditMixedPlaybackVoiceDataEvent failed, memory error (2)"), LogLevel_ERROR, schid);
      Py_DECREF(pysamples);
      return;
    }

    if (PyList_SetItem(pysamples, i, pysample) != 0) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling onEditMixedPlaybackVoiceDataEvent failed, list error"), LogLevel_ERROR, schid);
      Py_DECREF(pysample);
      Py_DECREF(pysamples);
      return;
    }
  }

  PyObject* pychannelSpeakerArray = PyList_New(channels);
  if (!pychannelSpeakerArray) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditMixedPlaybackVoiceDataEvent failed, memory error (3)"), LogLevel_ERROR, schid);
    Py_DECREF(pysamples);
    return;
  }

  PyObject* pyspeaker;
  for (int i = 0; i < channels; ++i) {
    pyspeaker = Py_BuildValue("I", channelSpeakerArray[i]);

    if (!pyspeaker) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling onEditMixedPlaybackVoiceDataEvent failed, memory error (4)"), LogLevel_ERROR, schid);
      Py_DECREF(pysamples);
      Py_DECREF(pychannelSpeakerArray);
      return;
    }

    if (PyList_SetItem(pychannelSpeakerArray, i, pyspeaker) != 0) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling onEditMixedPlaybackVoiceDataEvent failed, list error (2)"), LogLevel_ERROR, schid);
      Py_DECREF(pysamples);
      Py_DECREF(pyspeaker);
      Py_DECREF(pychannelSpeakerArray);
    }
  }

  PyObject* pyret;
  QString errstr;
  if (!callMethod(&pyret, errstr, "(sKOiOI)", "onEditMixedPlaybackVoiceDataEvent", (unsigned long long)schid, pysamples, channels, pychannelSpeakerArray, *channelFillMask)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditMixedPlaybackVoiceDataEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    Py_DECREF(pysamples);
    Py_DECREF(pychannelSpeakerArray);
    return;
  }
  Py_DECREF(pysamples);
  Py_DECREF(pychannelSpeakerArray);

  if (!PyTuple_Check(pyret) || PyTuple_Size(pyret) != 3) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditMixedPlaybackVoiceDataEvent failed, no tuple(3) returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  PyObject* pychanged = PyTuple_GetItem(pyret, 0);
  pysamples = PyTuple_GetItem(pyret, 1);
  PyObject* pyfillMask = PyTuple_GetItem(pyret, 2);

  if (!PyBool_Check(pychanged) || !PyList_Check(pysamples) || !PyLong_Check(pyfillMask)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditMixedPlaybackVoiceDataEvent failed, no tuple(bool, list, int) returned, got tuple(%1, %2, %3) instead").arg(pychanged->ob_type->tp_name).arg(pysamples->ob_type->tp_name).arg(pyfillMask->ob_type->tp_name), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  if (PyList_Size(pysamples) != sampleCount) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditMixedPlaybackVoiceDataEvent failed, no valid list returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  if (PyObject_IsTrue(pychanged)) {
    PyObject* item;
    for (int i = 0; i < PyList_Size(pysamples); ++i) {
      item = PyList_GetItem(pysamples, i);

      if (!item || !PyLong_Check(item)) {
        ts3logdispatcher::instance()->add(QObject::tr("Error in onEditMixedPlaybackVoiceDataEvent, no valid long in list, will be ignored"), LogLevel_ERROR, schid);
        continue;
      }

      samples[i] = (short)PyLong_AsLong(item);
    }

    *channelFillMask = (unsigned int)PyLong_AsUnsignedLong(pyfillMask);
  }
  Py_DECREF(pyret);
}

void PythonHost::onEditCapturedVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, int* edited) {
  return;
  PyObject* pysamples = PyList_New(sampleCount);
  if (!pysamples) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditCapturedVoiceDataEvent failed, memory error"), LogLevel_ERROR, schid);
    return;
  }

  PyObject* pysample;
  for (int i = 0; i < sampleCount; ++i) {
    pysample = Py_BuildValue("h", samples[i]);

    if (!pysample) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling onEditCapturedVoiceDataEvent failed, memory error (2)"), LogLevel_ERROR, schid);
      Py_DECREF(pysamples);
      return;
    }

    if (PyList_SetItem(pysamples, i, pysample) != 0) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling onEditCapturedVoiceDataEvent failed, list error"), LogLevel_ERROR, schid);
      Py_DECREF(pysample);
      Py_DECREF(pysamples);
      return;
    }
  }

  PyObject* pyret;
  QString errstr;
  if (!callMethod(&pyret, errstr, "(sKOii)", "onEditCapturedVoiceDataEvent", (unsigned long long)schid, pysamples, channels, *edited)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditCapturedVoiceDataEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    Py_DECREF(pysamples);
    return;
  }
  Py_DECREF(pysamples);

  if (!PyTuple_Check(pyret) || PyTuple_Size(pyret) != 3) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditCapturedVoiceDataEvent failed, no tuple(3) returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  PyObject* pychanged = PyTuple_GetItem(pyret, 0);
  pysamples = PyTuple_GetItem(pyret, 1);
  PyObject* pyedited = PyTuple_GetItem(pyret, 2);

  if (!PyBool_Check(pychanged) || !PyList_Check(pysamples) || !PyLong_Check(pyedited)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditCapturedVoiceDataEvent failed, no tuple(bool, list, int) returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  if (PyList_Size(pysamples) != sampleCount) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onEditCapturedVoiceDataEvent failed, no valid list returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  if (PyObject_IsTrue(pychanged)) {
    PyObject* item;
    for (int i = 0; i < PyList_Size(pysamples); ++i) {
      item = PyList_GetItem(pysamples, i);

      if (!item || !PyLong_Check(item)) {
        ts3logdispatcher::instance()->add(QObject::tr("Error in onEditCapturedVoiceDataEvent, no valid long in list, will be ignored"), LogLevel_ERROR, schid);
        continue;
      }

      samples[i] = (short)PyLong_AsLong(item);
    }

    *edited = (unsigned int)PyLong_AsUnsignedLong(pyedited);
  }
  Py_DECREF(pyret);
}

void PythonHost::onCustom3dRolloffCalculationClientEvent(uint64 schid, anyID clientID, float distance, float* volume) {
  return;
  PyObject* pyret;
  QString errstr;
  if (!callMethod(&pyret, errstr, "(sKIff)", "onCustom3dRolloffCalculationClientEvent", (unsigned long long)schid, (unsigned int)clientID, distance, *volume)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onCustom3dRolloffCalculationClientEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    return;
  }

  if (!PyFloat_Check(pyret)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onCustom3dRolloffCalculationClientEvent failed, no float returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  *volume = (float)PyFloat_AsDouble(pyret);
  Py_DECREF(pyret);
}

void PythonHost::onCustom3dRolloffCalculationWaveEvent(uint64 schid, uint64 waveHandle, float distance, float* volume) {
  return;
  PyObject* pyret;
  QString errstr;
  if (!callMethod(&pyret, errstr, "(sKKff)", "onCustom3dRolloffCalculationWaveEvent", (unsigned long long)schid, (unsigned long long)waveHandle, distance, *volume)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onCustom3dRolloffCalculationWaveEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    return;
  }

  if (!PyFloat_Check(pyret)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onCustom3dRolloffCalculationWaveEvent failed, no float returned"), LogLevel_ERROR, schid);
    Py_DECREF(pyret);
    return;
  }

  *volume = (float)PyFloat_AsDouble(pyret);
  Py_DECREF(pyret);
}

bool PythonHost::callMethod(PyObject** ret, QString& error, const char *format, ...) {
  if (!isReady()) {
    error = QObject::tr("PythonHost is not ready");
    return false;
  }

  PyObject* args = NULL;

  if (strlen(format) > 0) {
    va_list vl;
    va_start(vl, format);

    args = Py_VaBuildValue(format, vl);
    if (!args) {
      va_end(vl);
      error = QObject::tr("Error creating argumentlist");
      return false;
    }
    else va_end(vl);
  }

  PyObject* pyret = PyObject_CallObject(m_callmeth, args);
  Py_XDECREF(args);

  if (!pyret) {
    error = formatError(QObject::tr("Error calling method"));
    return false;
  }

  if (ret)
    *ret = pyret;
  else Py_DECREF(pyret);

  return true;
}
