#ifndef PYCONVERSION_H
#define PYCONVERSION_H

#include <Python.h>

#include <QString>
#include <QObject>


#include "teamspeak/public_definitions.h"
#include "plugin_definitions.h"

/**
 * @brief
 *
 * @param array
 * @param formatChar
 * @param error
 * @param ret
 * @param len
 * @return bool
 */
template<typename T>
inline bool arrayToPyList(const T* array, const char* formatChar, QString& error, PyObject** ret, unsigned int len) {
  if (!array) {
    error = QObject::tr("No array given in function arrayToPyList");
    return false;
  }

  *ret = PyList_New(len);
  if (!ret) {
    error = QObject::tr("Memory error in function arrayToPyList");
    return false;
  }

  PyObject* item;
  if (len != 0) {
    for (unsigned int i = 0; i < len; ++i) {
      item = Py_BuildValue(formatChar, array[i]);

      if (!item) {
        Py_DECREF(*ret);
        error = QObject::tr("Memory error (2) in function arrayToPyList");
        return false;
      }

      if (PyList_SetItem(*ret, i, item) != 0) {
        Py_DECREF(item);
        Py_DECREF(*ret);
        error = QObject::tr("Error setting item in list in function arrayToList");
        return false;
      }
    }
  }
  else {
    for (unsigned int i = 0; array[i] != 0; ++i) {
      item = Py_BuildValue(formatChar, array[i]);

      if (!item) {
        Py_DECREF(*ret);
        error = QObject::tr("Memory error (3) in function arrayToPyList");
        return false;
      }

      if (PyList_Append(*ret, item) != 0) {
        Py_DECREF(item);
        Py_DECREF(*ret);
        error = QObject::tr("Error appending item in list in function arrayToList");
        return false;
      }
    }
  }

  return true;
}

template<>
inline bool arrayToPyList<uint64>(const uint64* array, const char* , QString& error, PyObject** ret, unsigned int len) {
  if (!array) {
    error = QObject::tr("No array given in function arrayToPyList<uint64>");
    return false;
  }

  *ret = PyList_New(len);
  if (!ret) {
    error = QObject::tr("Memory error in function arrayToPyList<uint64>");
    return false;
  }

  PyObject* item;
  if (len != 0) {
    for (unsigned int i = 0; i < len; ++i) {
      item = Py_BuildValue("K", (unsigned long long)array[i]);

      if (!item) {
        Py_DECREF(*ret);
        error = QObject::tr("Memory error (2) in function arrayToPyList<uint64>");
        return false;
      }

      if (PyList_SetItem(*ret, i, item) != 0) {
        Py_DECREF(item);
        Py_DECREF(*ret);
        error = QObject::tr("Error setting item in list in function arrayToList<uint64>");
        return false;
      }
    }
  }
  else {
    for (unsigned int i = 0; array[i] != 0; ++i) {
      item = Py_BuildValue("K", (unsigned long long)array[i]);

      if (!item) {
        Py_DECREF(*ret);
        error = QObject::tr("Memory error (3) in function arrayToPyList<uint64>");
        return false;
      }

      if (PyList_Append(*ret, item) != 0) {
        Py_DECREF(item);
        Py_DECREF(*ret);
        error = QObject::tr("Error appending item in list in function arrayToList<uint64>");
        return false;
      }
    }
  }

  return true;
}

template<>
inline bool arrayToPyList<anyID>(const anyID* array, const char* , QString& error, PyObject** ret, unsigned int len) {
  if (!array) {
    error = QObject::tr("No array given in function arrayToPyList<anyID>");
    return false;
  }

  *ret = PyList_New(len);
  if (!ret) {
    error = QObject::tr("Memory error in function arrayToPyList<anyID>");
    return false;
  }

  PyObject* item;
  if (len != 0) {
    for (unsigned int i = 0; i < len; ++i) {
      item = Py_BuildValue("I", (unsigned int)array[i]);

      if (!item) {
        Py_DECREF(*ret);
        error = QObject::tr("Memory error (2) in function arrayToPyList<anyID>");
        return false;
      }

      if (PyList_SetItem(*ret, i, item) != 0) {
        Py_DECREF(item);
        Py_DECREF(*ret);
        error = QObject::tr("Error setting item in list in function arrayToList<anyID>");
        return false;
      }
    }
  }
  else {
    for (unsigned int i = 0; array[i] != 0; ++i) {
      item = Py_BuildValue("I", (unsigned int)array[i]);

      if (!item) {
        Py_DECREF(*ret);
        error = QObject::tr("Memory error (3) in function arrayToPyList<anyID>");
        return false;
      }

      if (PyList_Append(*ret, item) != 0) {
        Py_DECREF(item);
        Py_DECREF(*ret);
        error = QObject::tr("Error appending item in list in function arrayToList<anyID>");
        return false;
      }
    }
  }

  return true;
}

bool deviceArrayToPyList(char*** devices, QString& error, PyObject** ret);

bool bookmarksToPyList(struct PluginBookmarkList* bm, QString& error, PyObject** ret);

/**
 * @brief
 *
 * @param list
 * @param error
 * @param ret
 * @param appendNull
 * @return bool
 */
template<typename T>
inline bool pyListToArray(PyObject* , QString& error, T** , bool ) {
  error = QObject::tr("pyListToArray not implemented for this datatime");
  return false;
}

template<>
inline bool pyListToArray<char*>(PyObject* list, QString& error, char*** ret, bool appendNull) {
  if (!PyList_Check(list)) {
    error = QObject::tr("No PyList given in function pyListToArray<char*>");
    return false;
  }

  unsigned int len = PyList_Size(list);
  if (appendNull) {
    *ret = (char**)malloc(sizeof(char*) * (len +1));
    (*ret)[len] = NULL;
  }
  else *ret = (char**)malloc(sizeof(char*) * len);

  PyObject* item;
  const char* str;
  for (unsigned int i = 0; i < len; ++i) {
    item = PyList_GetItem(list, i);

    if (!item || !PyUnicode_Check(item)) {
      error = QObject::tr("No valid string object in PyList in function pyListToArray<char*>");
      for (int k = i -1; k >= 0; --k)
        free((*ret)[k]);
      free(*ret);
      return false;
    }

    str = PyUnicode_AsUTF8(item);
    (*ret[i]) = (char*)malloc(sizeof(char) * (strlen(str) +1));
    strcpy((*ret)[i], str);
  }

  return true;
}

template<>
inline bool pyListToArray<int>(PyObject* list, QString& error, int** ret, bool appendNull) {
  if (!PyList_Check(list)) {
    error = QObject::tr("No PyList given in function pyListToArray<int>");
    return false;
  }

  unsigned int len = PyList_Size(list);
  if (appendNull) {
    *ret = (int*)malloc(sizeof(int) * (len +1));
    (*ret)[len] = 0;
  }
  else *ret = (int*)malloc(sizeof(int) * len);

  PyObject* item;
  for (unsigned int i = 0; i < len; ++i) {
    item = PyList_GetItem(list, i);

    if (!item || !PyLong_Check(item)) {
      error = QObject::tr("No valid long object in PyList in function pyListToArray<int>");
      free(*ret);
      return false;
    }

    (*ret[i]) = (int)PyLong_AsLong(item);
  }

  return true;
}

template<>
inline bool pyListToArray<unsigned int>(PyObject* list, QString& error, unsigned int** ret, bool appendNull) {
  if (!PyList_Check(list)) {
    error = QObject::tr("No PyList given in function pyListToArray<unsigned int>");
    return false;
  }

  unsigned int len = PyList_Size(list);
  if (appendNull) {
    *ret = (unsigned int*)malloc(sizeof(unsigned int) * (len +1));
    (*ret)[len] = 0;
  }
  else *ret = (unsigned int*)malloc(sizeof(unsigned int) * len);

  PyObject* item;
  for (unsigned int i = 0; i < len; ++i) {
    item = PyList_GetItem(list, i);

    if (!item || !PyLong_Check(item)) {
      error = QObject::tr("No valid long object in PyList in function pyListToArray<unsigned int>");
      free(*ret);
      return false;
    }

    (*ret[i]) = (unsigned int)PyLong_AsUnsignedLong(item);
  }

  return true;
}

template<>
inline bool pyListToArray<uint64>(PyObject* list, QString& error, uint64** ret, bool appendNull) {
  if (!PyList_Check(list)) {
    error = QObject::tr("No PyList given in function pyListToArray<uint64>");
    return false;
  }

  unsigned int len = PyList_Size(list);
  if (appendNull) {
    *ret = (uint64*)malloc(sizeof(uint64) * (len +1));
    (*ret)[len] = 0;
  }
  else *ret = (uint64*)malloc(sizeof(uint64) * len);

  PyObject* item;
  for (unsigned int i = 0; i < len; ++i) {
    item = PyList_GetItem(list, i);

    if (!item || !PyLong_Check(item)) {
      error = QObject::tr("No valid long object in PyList in function pyListToArray<uint64>");
      free(*ret);
      return false;
    }

    (*ret[i]) = (uint64)PyLong_AsUnsignedLongLong(item);
  }

  return true;
}

template<>
inline bool pyListToArray<short>(PyObject* list, QString& error, short** ret, bool appendNull) {
  if (!PyList_Check(list)) {
    error = QObject::tr("No PyList given in function pyListToArray<short>");
    return false;
  }

  unsigned int len = PyList_Size(list);
  if (appendNull) {
    *ret = (short*)malloc(sizeof(short) * (len +1));
    (*ret)[len] = 0;
  }
  else *ret = (short*)malloc(sizeof(short) * len);

  PyObject* item;
  for (unsigned int i = 0; i < len; ++i) {
    item = PyList_GetItem(list, i);

    if (!item || !PyLong_Check(item)) {
      error = QObject::tr("No valid long object in PyList in function pyListToArray<short>");
      free(*ret);
      return false;
    }

    (*ret[i]) = (short)PyLong_AsLong(item);
  }

  return true;
}

template<>
inline bool pyListToArray<anyID>(PyObject* list, QString& error, anyID** ret, bool appendNull) {
  if (!PyList_Check(list)) {
    error = QObject::tr("No PyList given in function pyListToArray<anyID>");
    return false;
  }

  unsigned int len = PyList_Size(list);
  if (appendNull) {
    *ret = (anyID*)malloc(sizeof(anyID) * (len +1));
    (*ret)[len] = 0;
  }
  else *ret = (anyID*)malloc(sizeof(anyID) * len);

  PyObject* item;
  for (unsigned int i = 0; i < len; ++i) {
    item = PyList_GetItem(list, i);

    if (!item || !PyLong_Check(item)) {
      error = QObject::tr("No valid long object in PyList in function pyListToArray<anyID>");
      free(*ret);
      return false;
    }

    (*ret[i]) = (anyID)PyLong_AsUnsignedLong(item);
  }

  return true;
}

#endif // PYCONVERSION_H
