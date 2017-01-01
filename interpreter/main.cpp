#include <Python.h>

#include <errno.h>
#include <locale>

#include <iostream>

#include "pytsonpathfactory.h"


void deleteArray(wchar_t** arr, size_t size) {
  for (unsigned int i = 0; i < size; ++i)
    PyMem_RawFree(arr[i]);

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

wchar_t** convertArgs(int start, int last, char** argv, int jumpover = -1) {
  unsigned int len = last - start +2;
  if (jumpover >= 0)
    --len;

  wchar_t** ret = new wchar_t*[len];
  ret[len -1] = NULL;

  unsigned int rid;
  for (int i = start; i <= last; ++i) {
    if (jumpover == i)
      continue;

    if (jumpover == -1 || jumpover > i)
      rid = i - start;
    else
      rid = i - start -1;

    ret[rid] = Py_DecodeLocale(argv[i], NULL);

    if (!ret[rid]) {
      deleteArray(ret, rid);

      std::cerr << "Error decoding argument " << i << ": " << argv[i] << std::endl;
      return NULL;
    }
  }

  return ret;
}

void printHelp(const char* prog) {
  std::cout << "usage: " << prog << "[-c cmd | file] [arg] ..." << std::endl;
}

int main(int argc, char** argv) {
  if (argc == 0) {
    std::cerr << "No executablename in arg[0]" << std::endl;
    return 1;
  }
  else if (argc == 1) {
    std::cout << "Nothing to do here" << std::endl;
    printHelp(argv[0]);
    return 0;
  }

  std::string fname;
  std::string cmd;
  int jmpover = -1;
  unsigned int arglen = argc -1;

  if (!strcmp("--help", argv[1])) {
    printHelp(argv[0]);
    return 0;
  }
  else if (!strcmp("-c", argv[1])) {
    if (argc == 2) {
      std::cerr << "No command given" << std::endl;
      return 1;
    }

    cmd = argv[2];
    jmpover = 2;
    --arglen;
  }
  else fname = argv[1];

  char* tmp = setlocale(LC_ALL, NULL);
  char* loc = new char[strlen(tmp) +1];
  if (!loc) {
    std::cerr << "Out of memory when allocating locale string" << std::endl;
    return 1;
  }
  strncpy(loc, tmp, strlen(tmp));

  setlocale(LC_ALL, "");

  wchar_t** pyargv = convertArgs(1, argc -1, argv, jmpover);
  if (!pyargv) {
    setlocale(LC_ALL, loc);
    delete[]loc;

    return 1;
  }

  pytsonpathfactory fac(argv[0]);

  std::string execpath(fac.executablePath());
  wchar_t* wname = Py_DecodeLocale(execpath.c_str(), NULL);
  Py_SetProgramName(wname);

  setlocale(LC_ALL, loc);
  delete[] loc;

  std::string modpath(fac.moduleSearchPath());
  Py_SetPath(std::wstring(modpath.begin(), modpath.end()).c_str());

  Py_FrozenFlag = 1;
  Py_IgnoreEnvironmentFlag = 1;
  Py_NoUserSiteDirectory = 1;

  Py_InitializeEx(0);

  if (PyErr_Occurred()) {
    PyErr_Print();
    deleteArray(pyargv, arglen);
    PyMem_RawFree(wname);
    return 1;
  }

  PySys_SetArgvEx(arglen, pyargv, 0);

  PyObject* syspath = constructSysPath(fac);
  if (!syspath) {
    deleteArray(pyargv, arglen);
    PyMem_RawFree(wname);
    Py_Finalize();
    return 1;
  }

  if (PySys_SetObject("path", syspath) != 0) {
    Py_DECREF(syspath);
    deleteArray(pyargv, arglen);
    PyMem_RawFree(wname);
    Py_Finalize();

    std::cerr << "Error setting sys.path" << std::endl;
    return 1;
  }

  if (!fname.empty()) {
    FILE* f = fopen(fname.c_str(), "r");
    if (!f) {
      deleteArray(pyargv, arglen);
      PyMem_RawFree(wname);
      Py_Finalize();

      std::cerr << "Error opening script (error: " << errno << ")" << std::endl;
      return 1;
    }

    PyRun_SimpleFileEx(f, argv[1], 1);
  }
  else {
    if (PyRun_SimpleString(cmd.c_str()) != 0)
      std::cerr << "Error executing command" << std::endl;
  }

  Py_Finalize();

  deleteArray(pyargv, arglen);
  PyMem_RawFree(wname);

  return 0;
}
