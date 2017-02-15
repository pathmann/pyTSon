#include "pyconversion.h"

bool deviceArrayToPyList(char*** devices, QString& error, PyObject** ret) {
  if (!devices) {
    error = QObject::tr("No array given in function arrayToPyList<anyID>");
    return false;
  }

  *ret = PyList_New(0);

  if (!ret) {
    error = QObject::tr("Memory error in function deviceArrayToPyList");
    return false;
  }

  PyObject* tuple;
  for (int i = 0; devices[i] != NULL; ++i) {
    tuple = PyTuple_New(2);

    if (!tuple) {
      Py_DECREF(*ret);
      error = QObject::tr("Memory error (2) in function deviceArrayToPyList");
      return false;
    }

    if (PyTuple_SetItem(tuple, 0, PyUnicode_FromString(devices[i][0])) != 0) {
      Py_DECREF(tuple);
      Py_DECREF(*ret);
      error = QObject::tr("Error appending item in tuple in function deviceArrayToPyList");
      return false;
    }

    if (PyTuple_SetItem(tuple, 1, PyUnicode_FromString(devices[i][1])) != 0) {
      Py_DECREF(tuple);
      Py_DECREF(*ret);
      error = QObject::tr("Error appending item in tuple (2) in function deviceArrayToPyList");
      return false;
    }

    if (PyList_Append(*ret, tuple) != 0) {
      Py_DECREF(tuple);
      Py_DECREF(*ret);
      error = QObject::tr("Error appending item in list in function deviceArrayToPyList");
      return false;
    }
    Py_DECREF(tuple);
  }

  return true;
}

bool bookmarksToPyList(struct PluginBookmarkList* bm, QString& error, PyObject** ret) {
  if (!bm) {
    error = QObject::tr("No bookmarks given in function bookMarksToPyList");
    return false;
  }

  *ret = PyList_New(bm->itemcount);
  if (!*ret) {
    error = QObject::tr("Memory error in function bookMarksToPyList");
    return false;
  }

  PyObject* it;
  for (int i = 0; i < bm->itemcount; ++i) {
    it = PyTuple_New(4);

    if (!it) {
      Py_DECREF(*ret);
      error = QObject::tr("Memory error (2) in function bookMarksToPyList");
      return false;
    }

    if (PyTuple_SetItem(it, 0, PyUnicode_FromString(bm->items[i].name)) != 0) {
      Py_DECREF(it);
      Py_DECREF(*ret);
      error = QObject::tr("Error appending item in tuple in function bookMarksToPyList");
      return false;
    }

    if (PyTuple_SetItem(it, 1, PyLong_FromLong(bm->items[i].isFolder ? 1 : 0)) != 0) {
      Py_DECREF(it);
      Py_DECREF(*ret);
      error = QObject::tr("Error appending item in tuple (2) in function bookMarksToPyList");
      return false;
    }

    if (bm->items[i].isFolder) {
      if (PyTuple_SetItem(it, 2, Py_None) != 0) {
        Py_DECREF(it);
        Py_DECREF(*ret);
        error = QObject::tr("Error appending item in tuple (3) in function bookMarksToPyList");
        return false;
      }
      Py_INCREF(Py_None);

      PyObject* childs;
      if (!bookmarksToPyList(bm->items[i].folder, error, &childs)) {
        Py_DECREF(it);
        Py_DECREF(*ret);
        return false;
      }

      if (PyTuple_SetItem(it, 3, childs) != 0) {
        Py_DECREF(childs);
        Py_DECREF(it);
        Py_DECREF(*ret);
        error = QObject::tr("Error appending item in tuple (4) in function bookMarksToPyList");
        return false;
      }
    }
    else {
      if (PyTuple_SetItem(it, 2, PyUnicode_FromString(bm->items[i].uuid)) != 0) {
        Py_DECREF(it);
        Py_DECREF(*ret);
        error = QObject::tr("Error appending item in tuple (5) in function bookMarksToPyList");
        return false;
      }

      if (PyTuple_SetItem(it, 3, Py_None) != 0) {
        Py_DECREF(it);
        Py_DECREF(*ret);
        error = QObject::tr("Error appending item in tuple (6) in function bookMarksToPyList");
        return false;
      }
      Py_INCREF(Py_None);
    }

    if (PyList_SetItem(*ret, i, it) != 0) {
      Py_DECREF(it);
      Py_DECREF(*ret);
      error = QObject::tr("Error appending item in list in function bookMarksToPyList");
      return false;
    }
  }

  return true;
}
