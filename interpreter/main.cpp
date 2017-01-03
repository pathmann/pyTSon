#include <Python.h>

#include <locale>
#include <iostream>

#include "pytsonpathfactory.h"

void deleteArray(wchar_t** arr, size_t size) {
  for (unsigned int i = 0; i < size; ++i)
    PyMem_RawFree(arr[i]);

  delete[] arr;
}

int main(int argc, char** argv) {
  wchar_t** pyargv = new wchar_t*[argc +1];
  wchar_t** pyargv2 = new wchar_t*[argc +1];
  pyargv[argc] = NULL;
  pyargv2[argc] = NULL;

  char* tmp = setlocale(LC_ALL, NULL);
  char* loc = new char[strlen(tmp) +1];
  if (!loc) {
    std::cerr << "Out of memory when allocating locale string" << std::endl;
    return 1;
  }
  strncpy(loc, tmp, strlen(tmp));

  setlocale(LC_ALL, "");

  for (int i = 0; i < argc; ++i) {
    pyargv[i] = Py_DecodeLocale(argv[i], NULL);

    if (!pyargv[i]) {
      deleteArray(pyargv, i -1);
      std::cerr << "Error decoding argument " << i << ": " << argv[i] << std::endl;
      return 1;
    }

    pyargv2[i] = pyargv[i];
  }

  setlocale(LC_ALL, loc);
  delete[] loc;

  pytsonpathfactory fac(argv[0]);

  std::string modpath(fac.moduleSearchPath());
  Py_SetPath(std::wstring(modpath.begin(), modpath.end()).c_str());

  int res = Py_Main(argc, pyargv);

  deleteArray(pyargv2, argc);
  delete[] pyargv;

  return res;
}
