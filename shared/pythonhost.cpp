#include "pythonhost.h"

#include <stdlib.h>
#include <Python.h>

#include <QWidget>

#include "PythonQt.h"
#include "PythonQt_QtAll.h"

#include "pythonqt/pythonqtpytson.h"
#include "pythonqt/eventfilterobject.h"

#if defined(Q_OS_WIN)
  #define INTERPRETER "python.exe"
#else
  //mac + linux
  #define INTERPRETER "python"
#endif

PythonHost::PythonHost(): m_interpreter(NULL), m_trace(NULL), m_inited(false) {

}

PythonHost::~PythonHost() {
  if (m_interpreter)
    PyMem_RawFree(m_interpreter);
}

bool PythonHost::setupDirectories(QString &error) {
  if (!m_base.cd("pyTSon")) {
    error = QObject::tr("Error changing directory to plugin directory");
    return false;
  }

  QString interpreterpath = m_base.absoluteFilePath(INTERPRETER);
  if (!QFile::exists(interpreterpath)) {
    error = QObject::tr("Interpreter not found in %1").arg(interpreterpath);
    return false;
  }

#ifdef Q_OS_UNIX
  if (chmod(interpreterpath.toUtf8().data(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1)
    error.append(QObject::tr("Setting file permissions of the python interpreter failed, you may need to manually set chmod u+x to %1 (%2)").arg(interpreterpath).arg(errno));
#endif

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

    error.append(QObject::tr("New lib directory installed"));
  }

#ifndef Q_OS_WIN
  if (!m_libdir.cd("python3.5")) {
    error = QObject::tr("Error changing directory to standard lib directory");
    return false;
  }
#endif

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
  return (m_inited /*&& m_pluginmod && m_pmod && m_pyhost && m_callmeth*/ && m_trace);
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

void PythonHost::initPythonQt() {
  PythonQt::init(PythonQt::PythonAlreadyInitialized);
  PythonQt_QtAll::init();

  PythonQt::self()->registerClass(&EventFilterObject::staticMetaObject, "pytson");
  PythonQt::self()->addDecorators(new pytsondecorator());
}

bool PythonHost::init(const QDir& basedir, QString& error) {
  m_base = basedir;

  if (!m_base.exists()) {
    error = QObject::tr("Basepath %1 does not exist").arg(basedir.absolutePath());
    return false;
  }

  if (!setupDirectories(error))
    return false;

  for (pythoninittab tab: m_inittabs) {
    if (PyImport_AppendInittab(tab.name, tab.initfunc) == -1) {
      error = QObject::tr("Error initializing module %1").arg(tab.name);
      return false;
    }
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

  initPythonQt();

  m_trace = PyImport_ImportModule("traceback");
  if (!m_trace) {
    error = QObject::tr("Error importing traceback module");
    return false;
  }

  m_inited = true;
  return true;
}

void PythonHost::shutdown() {
  m_inited = false;

  Py_XDECREF(m_trace);
  m_trace = NULL;

  PythonQt::cleanup();

  if (Py_IsInitialized())
    Py_Finalize();
}
