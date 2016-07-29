#ifndef TS3MODULE_H__
#define TS3MODULE_H__

#include <Python.h>

#ifndef PYTSON_PARSER
PyMODINIT_FUNC PyInit_ts3(void);
#endif

{% for name, f in functions.items() %}
PyObject* {{f.name}}(PyObject* self, PyObject* args);
{% endfor %}

#endif

