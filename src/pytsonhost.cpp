#include "pytsonhost.h"

#include <QWidget>

#include "ts3logdispatcher.h"
#include "pyconversion.h"
#include "ts3lib.h"

#include "PythonQt.h"

pytsonhost::pytsonhost(): QObject(), PythonHost(), m_pluginmod(0), m_pmod(0), m_pyhost(0), m_callmeth(0) {
  qRegisterMetaType<ts3callbackarguments>("ts3callbackarguments");

  m_inittabs.append({"_ts3lib", PyInit_ts3lib});

  m_mainthread = std::this_thread::get_id();
  connect(this, &pytsonhost::callInMainThread, this, &pytsonhost::onCallInMainThread, Qt::QueuedConnection);
}

pytsonhost::~pytsonhost() {

}

bool pytsonhost::init(const QDir& basedir, QString& error) {
  bool superinit = PythonHost::init(basedir, error);

  if (superinit) {
    m_pluginmod = PyImport_ImportModule("ts3plugin");
    if (!m_pluginmod) {
      error = formatError(QObject::tr("Error importing ts3plugin module"));
      return false;
    }

    //import pluginhost module
    m_pmod = PyImport_ImportModule("pluginhost");
    if (!m_pmod) {
      error = formatError(QObject::tr("Error importing pluginhost module"));
      return false;
    }

    //get PluginHost class of module
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

    return true;
  }
  else return false;
}

bool pytsonhost::isReady() {
  return PythonHost::isReady() && m_callmeth;
}


void pytsonhost::shutdown() {
  if (isReady()) {
    //call PluginHost.shutdown method
    PyObject* ret = PyObject_CallMethod(m_pyhost, const_cast<char*>("shutdown"), const_cast<char *>(""));
    if (!ret)
      ts3logdispatcher::instance()->add(QObject::tr("Error calling PluginHost.shutdown"), LogLevel_ERROR);
    else Py_DECREF(ret);
  }

  Py_XDECREF(m_callmeth);
  m_callmeth = NULL;

  Py_XDECREF(m_pyhost);
  m_pyhost = NULL;

  Py_XDECREF(m_pmod);
  m_pmod = NULL;

  Py_XDECREF(m_pluginmod);
  m_pluginmod = NULL;

  PythonHost::shutdown();
}

void pytsonhost::freeMemory(void* data) {
  if (data)
    free(data);
}

void pytsonhost::configure(void* qParentWidget) {
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

int pytsonhost::processCommand(uint64 schid, const char* command) {
  PyObject* pyret;
  QString error;
  if (!callMethod(&pyret, error, "(sKs)", "processCommand", schid, command)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling processCommand failed with error \"%1\"").arg(error), LogLevel_ERROR);
    return 1;
  }

  /* Currently, this should not happen, processCommand is called from the mainthread, so shouldn't be queued */
  if (!pyret) {
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

void pytsonhost::infoData(uint64 schid, uint64 id, enum PluginItemType type, char** data) {
  PyObject* pyret;
  QString error;
  if (!callMethod(&pyret, error, "(sKKI)", "infoData", (unsigned long long)schid, (unsigned long long)id, (unsigned int)type)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling infoData failed with error \"%1\"").arg(error), LogLevel_ERROR);
    return;
  }

  /* Currently, this should not happen, infoData is called from the mainthread, so shouldn't be queued */
  if (!pyret) {
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
  QByteArray str;
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
  strncpy(*data, str.data(), str.size() +1);
}

void pytsonhost::initMenus(struct PluginMenuItem*** menuItems, char** menuIcon) {
  PyObject* pyret;
  QString error;
  if (!callMethod(&pyret, error, "(s)", "initMenus")) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling initMenus failed with error \"%1\"").arg(error), LogLevel_ERROR);
    return;
  }

  /* Currently, this should not happen, initMenus is called from the mainthread, so shouldn't be queued */
  if (!pyret) {
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
  *menuIcon = (char*)malloc((strlen("python.png") +1)* sizeof(char));
  strcpy(*menuIcon, "python.png");

  Py_DECREF(pyret);
}

void pytsonhost::initHotkeys(struct PluginHotkey*** hotkeys) {
  *hotkeys = NULL;

  PyObject* pyret = NULL;
  QString error;
  if (!callMethod(&pyret, error, "(s)", "initHotkeys")) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling initHotkeys failed with error \"%1\"").arg(error), LogLevel_ERROR);
    return;
  }

  /* Currently, this should not happen, initHotkeys is called from the mainthread, so shouldn't be queued */
  if (!pyret) {
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
      for (int k = i -1; k >= 0; --k)
        free((*hotkeys)[k]);
      free(*hotkeys);
      *hotkeys = NULL;
      return;
    }

    if (!PyUnicode_Check(PyTuple_GetItem(tup, 0)) || !PyUnicode_Check(PyTuple_GetItem(tup, 1))) {
      ts3logdispatcher::instance()->add(QObject::tr("Calling initHotkeys failed, no list of tuples(str, str) returned"), LogLevel_ERROR);
      Py_DECREF(pyret);
      for (int k = i -1; k >= 0; --k)
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

int pytsonhost::onServerErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
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

int pytsonhost::onTextMessageEvent(uint64 schid, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored) {
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

int pytsonhost::onClientPokeEvent(uint64 schid, anyID fromClientID, const char* pokerName, const char* pokerUniqueIdentity, const char* message, int ffIgnored) {
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

int pytsonhost::onServerPermissionErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, unsigned int failedPermissionID) {
  PyObject* pyret = NULL;
  QString errstr;
  if (!callMethod(&pyret, errstr, "(sKsIsI)", "onServerPermissionErrorEvent", (unsigned long long)schid, errorMessage, error, returnCode, failedPermissionID)) {
    ts3logdispatcher::instance()->add(QObject::tr("Calling onServerPermissionErrorEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
    return 1;
  }

  if (!pyret) {
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

void pytsonhost::onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString) {
  QString callerror;
  if (!callMethod(NULL, callerror, "(ssisKss)", "onUserLoggingMessageEvent",  logMessage,  logLevel,  logChannel, (unsigned long long) logID,  logTime,  completeLogString)) {
    printf("%s\n", QObject::tr("Calling onUserLoggingMessageEvent failed with error \"%1\"\n").arg(callerror).toUtf8().data());
  }
}

void pytsonhost::onEditPlaybackVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels) {
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

void pytsonhost::onEditPostProcessVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
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

void pytsonhost::onEditMixedPlaybackVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
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

void pytsonhost::onEditCapturedVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, int* edited) {
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

void pytsonhost::onCustom3dRolloffCalculationClientEvent(uint64 schid, anyID clientID, float distance, float* volume) {
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

void pytsonhost::onCustom3dRolloffCalculationWaveEvent(uint64 schid, uint64 waveHandle, float distance, float* volume) {
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

void pytsonhost::onFileTransferStatusEvent(anyID transferID, unsigned int status, const char *statusMessage, uint64 remotefileSize, uint64 schid) {
  QString errstr;
  if (!callMethod(NULL, errstr, "(sIIsKK)", "onFileTransferStatusEvent", (unsigned int)transferID, status, statusMessage, (unsigned long long)remotefileSize, (unsigned long long)schid))
    ts3logdispatcher::instance()->add(QObject::tr("Calling onFileTransferStatusEvent failed with error \"%1\"").arg(errstr), LogLevel_ERROR, schid);
}

bool pytsonhost::callMethod(PyObject** ret, QString& error, const char *format, ...) {
  if (!isReady()) {
    error = QObject::tr("pytsonhost is not ready");
    return false;
  }

  PyObject* args = NULL;

  if (strlen(format) > 0) {
    va_list vl;
    va_start(vl, format);

    if (std::this_thread::get_id() != m_mainthread) {
      ts3callbackarguments cbargs;
      if (cbargs.init(error, format, vl)) {
        emit callInMainThread(cbargs);
        error = QObject::tr("Event is queued");
        return true;
      }
      else return false;
    }
    else {
      args = Py_VaBuildValue(format, vl);
      if (!args) {
        va_end(vl);
        error = QObject::tr("Error creating argumentlist");
        return false;
      }
      else va_end(vl);
    }
  }
  else return false;

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

void pytsonhost::onCallInMainThread(const ts3callbackarguments args) {
  if (std::this_thread::get_id() == m_mainthread) {
    if (!isReady()) {
      ts3logdispatcher::instance()->add(QObject::tr("Internal error in queued event, pytsonhost not ready anymore"), LogLevel_ERROR);
    }
    else {
      QString err;
      PyObject* pyargs = args.toPythonTuple(err);

      if (!pyargs)
        ts3logdispatcher::instance()->add(QObject::tr("Internal error in queued event, error creating argument tuple: \"%1\"").arg(err), LogLevel_ERROR);
      else {
        PyObject* pyret = PyObject_CallObject(m_callmeth, pyargs);
        Py_XDECREF(pyargs);

        if (!pyret)
          ts3logdispatcher::instance()->add(QObject::tr("Error calling queued method"), LogLevel_ERROR);
        else Py_DECREF(pyret);
      }
    }
  }
  else ts3logdispatcher::instance()->add(QObject::tr("Internal error in queued event, not in mainthread"), LogLevel_ERROR);
}
