#include <Python.h>

#include <errno.h>
#include <locale>

#include <iostream>

#include "pytsonpathfactory.h"


void deleteArray(wchar_t** arr, size_t size) {
  for (unsigned int i = 0; i < size; ++i)
    delete[] arr[i];

  delete[] arr;
}

bool PyList_SetString(PyObject* list, Py_ssize_t pos, const std::string str) {
  PyObject* pypath = Py_BuildValue("s", str.c_str());
  if (!pypath) {
    std::cerr << "Out of memory when allocating pythonpath for " << str << std::endl;
    return false;
  }

  if (PyList_SetItem(list, pos, pypath) != 0) {
    Py_DECREF(pypath);

    std::cerr << "Error adding " << str << " to list" << std::endl;
    return false;
  }

  return true;
}

PyObject* constructSysPath(const pytsonpathfactory fac) {
  PyObject* syspath = PyList_New(4);
  if (!syspath) {
    std::cerr << "Out of memory when allocating sys.path" << std::endl;
    return NULL;
  }

  if (!PyList_SetString(syspath, 0,  fac.includePath())) {
    Py_DECREF(syspath);
    return NULL;
  }

  if (!PyList_SetString(syspath, 1, fac.libPath())) {
    Py_DECREF(syspath);
    return NULL;
  }

  if (!PyList_SetString(syspath, 2, fac.dynloadPath())) {
    Py_DECREF(syspath);
    return NULL;
  }

  if (!PyList_SetString(syspath, 3, fac.sitepackPath())) {
    Py_DECREF(syspath);
    return NULL;
  }

  return syspath;
}

wchar_t** convertArgs(unsigned int start, unsigned int count, char** argv) {
  const unsigned int len = count - start +1;
  wchar_t** ret = new wchar_t*[count - start +1];
  ret[len -1] = NULL;

  for (unsigned int i = start; i < count; ++i) {
    ret[i -1] = Py_DecodeLocale(argv[i], NULL);

    if (!ret[i -1]) {
      deleteArray(ret, i -1);

      std::cerr << "Error decoding argument " << i << ": " << argv[i] << std::endl;
      return NULL;
    }
  }

  return ret;
}

int main(int argc, char** argv) {
  if (argc == 0) {
    std::cerr << "No executablename in arg[0]" << std::endl;
    return 1;
  }
  else if (argc == 1) {
    std::cout << "No script to execute" << std::endl;
    return 0;
  }

  char* tmp = setlocale(LC_ALL, NULL);
  char* loc = new char[strlen(tmp) +1];
  if (!loc) {
    std::cerr << "Out of memory when allocating locale string" << std::endl;
    return 1;
  }
  strncpy(loc, tmp, strlen(tmp));

  setlocale(LC_ALL, "");

  wchar_t** pyargv = convertArgs(1, argc, argv);
  if (!pyargv) {
    setlocale(LC_ALL, loc);
    delete[]loc;

    return 1;
  }

  setlocale(LC_ALL, loc);
  delete[] loc;

  pytsonpathfactory fac(argv[0]);

  Py_FrozenFlag = 1;
  Py_IgnoreEnvironmentFlag = 1;
  Py_NoUserSiteDirectory = 1;

  std::string execpath(fac.executablePath());
  wchar_t* wname = Py_DecodeLocale(execpath.c_str(), NULL);
  Py_SetProgramName(wname);

  std::string modpath(fac.moduleSearchPath());
  Py_SetPath(std::wstring(modpath.begin(), modpath.end()).c_str());

  Py_InitializeEx(0);

  if (PyErr_Occurred()) {
    PyErr_Print();
    deleteArray(pyargv, argc -1);
    free(wname);
    return 1;
  }

  PySys_SetArgvEx(argc -1, pyargv, 0);

  PyObject* syspath = constructSysPath(fac);
  if (!syspath) {
    deleteArray(pyargv, argc -1);
    free(wname);
    Py_Finalize();
    return 1;
  }

  if (PySys_SetObject("path", syspath) != 0) {
    Py_DECREF(syspath);
    deleteArray(pyargv, argc -1);
    free(wname);

    std::cerr << "Error setting sys.path" << std::endl;
    return 1;
  }

  FILE* f = fopen(argv[1], "r");
  if (!f) {
    deleteArray(pyargv, argc -1);
    free(wname);

    std::cerr << "Error opening script (error: " << errno << ")" << std::endl;
    return 1;
  }

  PyRun_SimpleFileEx(f, argv[1], 1);

  Py_Finalize();

  deleteArray(pyargv, argc -1);
  free(wname);

  return 0;
}
