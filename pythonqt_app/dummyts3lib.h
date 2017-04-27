#ifndef DUMMYTS3LIB_H
#define DUMMYTS3LIB_H

#include <Python.h>

PyMODINIT_FUNC PyInit_dummyts3lib(void);

PyObject* getConfigPath(PyObject* self, PyObject* args);
PyObject* getResourcesPath(PyObject* self, PyObject* args);
PyObject* logMessage(PyObject* self, PyObject* args);

#endif // DUMMYTS3LIB_H
