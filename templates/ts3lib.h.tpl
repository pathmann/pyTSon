#ifndef TS3LIB_H__
#define TS3LIB_H__

#include <Python.h>

#ifndef PYTSON_PARSER
PyMODINIT_FUNC PyInit_ts3lib(void);
#endif

PyObject* getPluginID(PyObject* self, PyObject* args);
{% for name, f in functions|dictsort %}
PyObject* {{f.name}}(PyObject* self, PyObject* args);
{% endfor %}

#endif

