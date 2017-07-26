#include "dummyts3lib.h"

#include "global_shared_app.h"

static PyMethodDef ts3moddummyfuncs[] = {
  {"getConfigPath", getConfigPath, METH_VARARGS, "fake config path function"},
  {"getResourcesPath", getResourcesPath, METH_VARARGS, "fake resources path function"},
  {"logMessage", logMessage, METH_VARARGS, "fake logMessage function"},
  {NULL, NULL, 0, NULL}
};

static PyModuleDef mdef = {
  PyModuleDef_HEAD_INIT,
  "ts3lib",
  NULL,
  -1,
  ts3moddummyfuncs,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC PyInit_dummyts3lib(void) {
 return PyModule_Create(&mdef);
}

PyObject* getConfigPath(PyObject* /*self*/, PyObject* args) {
  unsigned int maxLen = 256;

  if (!PyArg_ParseTuple(args, "|I", &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, "maxLen must be greater than zero");
    return NULL;
  }

  PyObject* ret;
  if (config_path) {
    char* path = new char[maxLen];
    strncpy(path, config_path, maxLen -1);

    ret = Py_BuildValue("s", path);
    delete[] path;
  }
  else ret = Py_BuildValue("s", "no path given");

  return ret;
}

PyObject* getResourcesPath(PyObject* /*self*/, PyObject* args) {
  unsigned int maxLen = 256;

  if (!PyArg_ParseTuple(args, "|I", &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, "maxLen must be greater than zero");
    return NULL;
  }

  PyObject* ret;
  if (resources_path) {
    char* path = new char[maxLen];
    strncpy(path, resources_path, maxLen -1);

    ret = Py_BuildValue("s", path);
    delete[] path;
  }
  else ret = Py_BuildValue("s", "no path given");

  return ret;
}


PyObject* logMessage(PyObject* /*self*/, PyObject* args) {
  char* msg;
  int severity;
  char* channel;
  unsigned long long logID;

  if (!PyArg_ParseTuple(args, "sisK", &msg, &severity, &channel, &logID))
    return NULL;

  printf("logMessage: %s, (%d, %s, %llu)\n", msg, severity, channel, logID);

  return Py_BuildValue("I", 0);
}
