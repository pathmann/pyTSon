#include <Python.h>

#include <iostream>

#include <QApplication>
#include <QDir>

#include "pythonqtapphost.h"
#include "dialog.h"
#include "global_shared.h"

void printHelp(const char* execname) {
  std::cout << execname << " [-h] [-p pyTSon-dir] [-e script] [-c]" << std::endl;
  std::cout << "-h\tShow this help" << std::endl;
  std::cout << "-p pyTSon-dir\tThe directory where pyTSon lives in (this might be your TS3 plugin path)" << std::endl;
  std::cout << "-e script\tScript to execute on startup" << std::endl;
  std::cout << "-s\tOpen scripting console on startup" << std::endl;
  std::cout << "-r\tResources path of the ts3 client" << std::endl;
  std::cout << "-c\tConfig path of the ts3 client" << std::endl;
}

int main(int argc, char** argv) {
  QApplication app(argc, argv);


  QStringList args = app.arguments();
  if (args.indexOf("-h") != -1) {
    printHelp(argv[0]);
    return 0;
  }

  QDir dir;
  QString script;
  QString respath;
  QString cfgpath;
  bool console = false;
  for (auto it = args.begin() +1; it != args.end(); ++it) {
    if (*it == "-p") {
      ++it;
      if (it == args.end()) {
        printHelp(argv[0]);
        std::cerr << "missing pyTSon-dir argument" << std::endl;
        return 1;
      }

      dir = *it;
      if (!dir.exists()) {
        printHelp(argv[0]);
        std::cerr << "pyTSon-dir does not exist" << std::endl;
        return 1;
      }
    }
    else if (*it == "-e") {
      ++it;
      if (it == args.end()) {
        printHelp(argv[0]);
        std::cerr << "missing script argument" << std::endl;
        return 1;
      }

      script = *it;
      if (!QFile::exists(script)) {
        printHelp(argv[0]);
        std::cerr << "script does not exist" << std::endl;
        return 1;
      }
    }
    else if (*it == "-s")
      console = true;
    else if (*it == "-r") {
      ++it;
      if (it == args.end()) {
        printHelp(argv[0]);
        std::cerr << "missing resources path argument" << std::endl;
        return 1;
      }

      respath = *it;
    }
    else if (*it == "-c") {
      ++it;
      if (it == args.end()) {
        printHelp(argv[0]);
        std::cerr << "missing config path argument" << std::endl;
        return 1;
      }

      cfgpath = *it;
    }
    else {
      printHelp(argv[0]);
      std::cerr << "can't interpret argument: " << (*it).toUtf8().constData() << std::endl;
      return 1;
    }
  }

  QString error;
  PythonQtAppHost pythonhost;
  if (pythonhost.init(dir, error)) {
    if (!error.isEmpty())
      std::cout << error.toUtf8().data() << std::endl;

    if (!respath.isEmpty()) {
      QByteArray bytes = respath.toUtf8();
      resources_path = new char[bytes.length() +1];
      strncpy(resources_path, bytes.constData(), bytes.length());
    }

    if (!cfgpath.isEmpty()) {
      QByteArray bytes = cfgpath.toUtf8();
      config_path = new char[bytes.length() +1];
      strncpy(config_path, bytes.constData(), bytes.length());
    }

    Dialog* dlg = new Dialog(script, console);
    dlg->show();

    app.connect(&app, &QApplication::aboutToQuit, [&]() {
      delete dlg;
      pythonhost.shutdown();

      delete[] config_path;
      delete[] resources_path;
    });

    return app.exec();
  }
  else {
    std::cerr << error.toUtf8().data() << std::endl;
    return 1;
  }
}
