#include "ts3callbackarguments.h"

#include <QObject>

ts3callbackarguments::ts3callbackarguments() {

}

bool ts3callbackarguments::init(QString& error, const char* format, va_list vl) {
  /* minimum is "(x)" */
  if (strlen(format) < 3) {
    error = QObject::tr("Format string is too short");
    return false;
  }

  if (format[0] != '(' || format[strlen(format) -1] != ')') {
    error = QObject::tr("Format string is no tuple");
    return false;
  }

  for (unsigned int i = 1; i < strlen(format) -1; ++i) {
    switch (format[i]) {
      case 's':
        addString(va_arg(vl, char*));
        break;
      case 'i':
        addInt(va_arg(vl, int));
        break;
      case 'I':
        addUnsignedInt(va_arg(vl, unsigned int));
        break;
      case 'K':
        addUnsignedLongLong(va_arg(vl, unsigned long long));
        break;
      default:
        error = QObject::tr("Unrecognized format character \"%1\"").arg(format[i]);
        return false;
    }
  }

  return true;
}

void ts3callbackarguments::addInt(int val) {
  m_formats.append('i');
  m_values.append(val);
}

void ts3callbackarguments::addUnsignedInt(unsigned int val) {
  m_formats.append('I');
  m_values.append(val);
}

void ts3callbackarguments::addUnsignedLongLong(unsigned long long val) {
  m_formats.append('K');
  m_values.append(val);
}

void ts3callbackarguments::addString(char *val) {
  m_formats.append('s');
  m_values.append(QString(val));
}

PyObject* ts3callbackarguments::toPythonTuple(QString& error) const {
  if (m_formats.size() != m_values.size()) {
    error = QObject::tr("Internal error: vector sizes do not match");
    return NULL;
  }

  PyObject* ret = PyTuple_New(m_formats.size());

  if (!ret) {
    error = QObject::tr("Memory error");
    return NULL;
  }

  PyObject* it;
  for (int i = 0; i < m_formats.size(); ++i) {
    switch (m_formats[i]) {
      case 's':
        it = PyUnicode_FromString(m_values[i].toString().toUtf8().data());
        if (!it) {
          error = QObject::tr("Memory error (2)");
          Py_DECREF(ret);
          return NULL;
        }

        if (PyTuple_SetItem(ret, i, it) != 0) {
          error = QObject::tr("Error adding string to tuple");
          Py_DECREF(it);
          Py_DECREF(ret);
          return NULL;
        }
        break;
      case 'i':
        it = PyLong_FromLong(m_values[i].toInt());
        if (!it) {
          error = QObject::tr("Memory error (3)");
          Py_DECREF(ret);
          return NULL;
        }

        if (PyTuple_SetItem(ret, i, it) != 0) {
          error = QObject::tr("Error adding int to tuple");
          Py_DECREF(it);
          Py_DECREF(ret);
          return NULL;
        }
        break;
      case 'I':
        it = PyLong_FromUnsignedLong(m_values[i].toUInt());
        if (!it) {
          error = QObject::tr("Memory error (4)");
          Py_DECREF(ret);
          return NULL;
        }

        if (PyTuple_SetItem(ret, i, it) != 0) {
          error = QObject::tr("Error adding unsigned int to tuple");
          Py_DECREF(it);
          Py_DECREF(ret);
          return NULL;
        }
        break;
      case 'K':
        it = PyLong_FromUnsignedLongLong(m_values[i].toULongLong());
        if (!it) {
          error = QObject::tr("Memory error (5)");
          Py_DECREF(ret);
          return NULL;
        }

        if (PyTuple_SetItem(ret, i, it) != 0) {
          error = QObject::tr("Error adding unsigned long long to tuple");
          Py_DECREF(it);
          Py_DECREF(ret);
          return NULL;
        }
        break;
      default:
        error = QObject::tr("Internal error: unrecognized format character \"%1\"").arg(m_formats[i]);
        break;
    }
  }

  return ret;
}
