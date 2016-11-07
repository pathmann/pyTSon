#include "ts3module.h"

#include <QObject>
#include <QString>

#include "teamspeak/public_definitions.h"
#include "teamspeak/public_errors.h"

#include "global_shared.h"
#include "pyconversion.h"

#include "PythonQtInstanceWrapper.h"

//this macros makes the code "slightly" ugly, but as long as we use a strict C-Parser, it's necessary
#ifdef PYTSON_PARSER
#define TRANS (void*)
#define PYLIST_TO_ARRAY(type, list, error, ret, appendNull) (void*)(list, error, ret, appendNull)
#define ARRAY_TO_PYLIST(type, array, formatChar, error, ret, len) (void*)(array, formatChar, error, ret, len)
#define VECTOR(x, y, z) (void*)(x, y, z)
#define CPPALLOC(type, size) (void*)(type, size)
#define delete
#define CPPDELARR(arr) (void*)(arr)
#else
#define TRANS QObject::tr
#define PYLIST_TO_ARRAY(type, list, error, ret, appendNull) pyListToArray<type>(list, error, ret, appendNull)
#define ARRAY_TO_PYLIST(type, array, formatChar, error, ret, len) arrayToPyList<type>(array, formatChar, error, ret, len)
#define VECTOR(x, y, z) {x, y, z}
#define CPPALLOC(type, size) new type[size]
#define CPPDELARR(arr) delete[] arr
#endif

/*
# coding: utf-8

class ts3:
    """

    """

*/

#ifndef PYTSON_PARSER
static PyMethodDef ts3modfuncs[] = {
  {"getPluginID", getPluginID, METH_VARARGS, "Get pyTSon's pluginID"},
{% for name in functions %}
  {"{{name}}", {{name}}, METH_VARARGS, "Call {{name}} of the ts3 client plugin sdk"},
{% endfor %}
  {NULL, NULL, 0, NULL}
};

static PyModuleDef mdef = {
  PyModuleDef_HEAD_INIT,
  "ts3",
  NULL,
  -1,
  ts3modfuncs,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC PyInit_ts3(void) {
 return PyModule_Create(&mdef);
}
#endif

PyObject* getPluginID(PyObject* /*self*/, PyObject* args) {
  /*
    @staticmethod
    def getPluginID():
        """
        Returns pyTSon's plugin id
        @return: the plugin id
        @rtype: string
        """
  */
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  return Py_BuildValue("s", ts3_pluginid);
}

{% for name, f in functions.items() %}
PyObject* {{f.name}}(PyObject* /*self*/, PyObject* args) {
{% if f.body == "" %}
  //{{f.signature}}
  /*
    @staticmethod
    def {{f.name}}({{f.parameterNames | join(", ")}}):
        """
        
        {% for p in f.parameterNames %}
        @param {{p}}:
        @type {{p}}:
        {% endfor %}
        """

  */

{% else %}
{{f.body}}
{% endif %}
}

{% endfor %}
