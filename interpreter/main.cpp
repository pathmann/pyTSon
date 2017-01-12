#include <Python.h>

#include <locale>
#include <iostream>

#include <array>

#include "pytsonpathfactory.h"

#if defined(_WIN32)
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

void deleteArray(wchar_t** arr, size_t size) {
  for (unsigned int i = 0; i < size; ++i)
    PyMem_RawFree(arr[i]);

  delete[] arr;
}

void printHelp(const char* prog) {
  std::cout << "usage: " << prog << "[-c cmd | -m module | file] [arg] ..." << std::endl;
}

int runModule(std::string modname) {
  PyObject* runpy = PyImport_ImportModule("runpy");
  if (!runpy) {
    std::cerr << "Error importing runpy module" << std::endl;
    return 1;
  }

  PyObject* runmeth = PyObject_GetAttrString(runpy, "_run_module_as_main");
  if (!runmeth) {
    Py_DECREF(runpy);
    std::cerr << "Error getting runmodule method" << std::endl;
    return 1;
  }

  PyObject* args = Py_BuildValue("(si)", modname.c_str(), 1);
  if (!args) {
    Py_DECREF(runpy);
    Py_DECREF(runmeth);
    std::cerr << "Error creating arguments for runmodule method" << std::endl;
    return 1;
  }

  PyObject* res = PyObject_Call(runmeth, args, NULL);
  Py_DECREF(runpy);
  Py_DECREF(runmeth);
  Py_DECREF(args);

  if (res) {
    Py_DECREF(res);
    return 0;
  }
  else return 1;
}

bool setupPrefix(const pytsonpathfactory& fac) {
  const char* prefixes[] = {"prefix", "exec_prefix", "base_prefix", "base_exec_prefix"};
  PyObject* pref = NULL;
  for (unsigned int i = 0; i < std::extent<decltype(prefixes)>::value; ++i) {
    pref = Py_BuildValue("s", fac.basepath().c_str());
    if (!pref) {
      std::cerr << "Error creating prefix string" << std::endl;
      return false;
    }

    if (PySys_SetObject(prefixes[i], pref) != 0) {
      Py_DECREF(pref);
      std::cerr << "Error setting prefix string" << std::endl;
      return false;
    }
  }

  return true;
}

bool setupPath() {
  PyObject* pypath = PySys_GetObject("path");
  if (!pypath) {
    std::cerr << "Error getting sys.path" << std::endl;
    return false;
  }

  PyObject* estring = Py_BuildValue("s", "");
  if (!estring) {
    std::cerr << "Error creating empty python string" << std::endl;
    return false;
  }

  if (PyList_Insert(pypath, 0, estring) != 0) {
    Py_DECREF(estring);
    std::cerr << "Error prepending to sys.path" << std::endl;
    return false;
  }

  return true;
}

template <std::size_t S>
bool isIn(const char* str, std::array<const char*, S> arr) {
  for (auto& e: arr) {
    if (!strcmp(str, e))
      return true;
  }

  return false;
}

int main(int argc, char** argv) {
  if (argc == 0) {
    std::cerr << "Internal error (no executablename in argv[0])" << std::endl;
    return 1;
  }

  std::string fname, cmd, mod;
  //argv[0] is never in sys.argv
  unsigned int pyargc = argc -1;
  int cmdarg = -1;
  int argstart = -1;
  std::array<const char*, 17> iparams = {"-b", "-bb", "-B", "-d", "-E", "-i", "-I", "-O", "-OO", "-q", "-s", "-S", "-u", "-v", "-V", "--version", "-x"};
  std::array<const char*, 2> iargparams = {"-W", "-X"};

  for (int i = 1; i < argc; ++i) {
    if (!strcmp("--help", argv[i]) || !strcmp("-h", argv[i])) {
      printHelp(argv[0]);
      return 0;
    }
    else if (!strcmp("-c", argv[i])) {
      if (i == argc -1) {
        std::cerr << "No command given" << std::endl;
        return 1;
      }

      cmd = argv[i +1];
      argstart = i +2;
      cmdarg = i;
      --pyargc;
      ++i;
      break;
    }
    else if (!strcmp("-m", argv[i])) {
      if (i == argc -1) {
        std::cerr << "No module given" << std::endl;
        return 1;
      }

      mod = argv[i +1];
      argstart = i +2;
      cmdarg = i;
      --pyargc;
      ++i;
      break;
    }
    else if (isIn(argv[i], iparams)) {
      /* things, we ignore without second argument */
      --pyargc;
    }
    else if (isIn(argv[i], iargparams)) {
      /* things, we ignore with a second argument */
      if (i == argc -1) {
        std::cerr << "Switch " << argv[i] << "implies a second argument" << std::endl;
        return 1;
      }

      ++i;
      pyargc -= 2;
    }
    else if (fname.empty() && cmd.empty() && mod.empty()) {
      fname = argv[i];
      argstart = i;
      break;
    }
  }

  if ((fname.empty() && cmd.empty() && mod.empty()) || (fname.empty() && cmdarg == -1)) {
    std::cerr << "Nothing to do here" << std::endl;
    printHelp(argv[0]);
    return 1;
  }

  if (argstart == -1) {
    /* this should not happen, the above if-case has handled this */
    std::cerr << "Internal error (argstart == -1)" << std::endl;
    return 1;
  }

  wchar_t** pyargv = new wchar_t*[pyargc +1];
  wchar_t** pyargv2 = new wchar_t*[pyargc +1];
  pyargv[pyargc] = NULL;
  pyargv2[pyargc] = NULL;

  char* tmp = setlocale(LC_ALL, NULL);
  char* loc = new char[strlen(tmp) +1];
  if (!loc) {
    std::cerr << "Out of memory when allocating locale string" << std::endl;
    return 1;
  }
  strncpy(loc, tmp, strlen(tmp));

  setlocale(LC_ALL, "");

  unsigned int rid = 0;
  if (cmdarg != -1) {
    pyargv[0] = Py_DecodeLocale(argv[cmdarg], NULL);

    if (!pyargv[0]) {
      delete[] pyargv;
      delete[] pyargv2;
      setlocale(LC_ALL, loc);
      delete[] loc;
      std::cerr << "Error decoding argument " << argv[cmdarg] << std::endl;
      return 1;
    }

    pyargv2[0] = pyargv[0];
    ++rid;
  }

  for (int i = argstart; i < argc; ++i) {
    pyargv[rid] = Py_DecodeLocale(argv[i], NULL);

    if (!pyargv[rid]) {
      deleteArray(pyargv2, rid -1);
      delete[] pyargv;
      setlocale(LC_ALL, loc);
      delete[] loc;
      std::cerr << "Error decoding argument " << argv[i] << std::endl;
      return 1;
    }

    pyargv2[rid] = pyargv[rid];
    ++rid;
  }

  setlocale(LC_ALL, loc);
  delete[] loc;

  pytsonpathfactory fac(argv[0]);

  std::string execpath = fac.executablePath();
  std::wstring wexecpath(execpath.begin(), execpath.end());
  Py_SetProgramName(const_cast<wchar_t*>(wexecpath.c_str()));

  std::string modpath(fac.moduleSearchPath());
  Py_SetPath(std::wstring(modpath.begin(), modpath.end()).c_str());

  Py_IsolatedFlag = 1;
  Py_FrozenFlag = 1;
  Py_IgnoreEnvironmentFlag = 1;
  Py_NoUserSiteDirectory = 1;

  Py_InitializeEx(0);

  PySys_SetArgvEx(pyargc, pyargv, 0);

  if (!setupPath()) {
    deleteArray(pyargv2, pyargc +1);
    delete[] pyargv;

    Py_Finalize();
    return 1;
  }

  std::string home(fac.basepath());
  Py_SetPythonHome(const_cast<wchar_t*>(std::wstring(home.begin(), home.end()).c_str()));

  if (!setupPrefix(fac)) {
    deleteArray(pyargv2, pyargc +1);
    delete[] pyargv;

    Py_Finalize();
    return 1;
  }

  int res = 0;
  if (!fname.empty()) {
    FILE* f = fopen(fname.c_str(), "r");
    if (!f) {
      deleteArray(pyargv2, pyargc +1);
      delete[] pyargv;
      Py_Finalize();

      std::cerr << "Error opening script (error: " << errno << ")" << std::endl;
      return 1;
    }

    res = PyRun_SimpleFileEx(f, argv[1], 1);
  }
  else if (mod.empty())
    res = PyRun_SimpleString(cmd.c_str());
  else res = runModule(mod);

  if (res != 0)
    PyErr_Print();

  Py_Finalize();

  deleteArray(pyargv2, pyargc +1);
  delete[] pyargv;

  return res;
}
