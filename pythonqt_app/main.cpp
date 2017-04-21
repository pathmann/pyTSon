#include <Python.h>

#include <iostream>

#include <QApplication>
#include <QDir>

#include "pythonqtapphost.h"
#include "dialog.h"

void printHelp(const char* execname) {
  std::cout << execname << " [-h] [-p pyTSon-dir] [-e script]" << std::endl;
  std::cout << "-h\tShow this help" << std::endl;
  std::cout << "-p pyTSon-dir\tThe directory where pyTSon lives in (this might be your TS3 plugin path)" << std::endl;
  std::cout << "-e script\tScript to execute on startup" << std::endl;
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

    Dialog* dlg = new Dialog(script);
    dlg->show();

    app.connect(&app, &QApplication::aboutToQuit, [&]() {
      delete dlg;
      pythonhost.shutdown();
    });

    return app.exec();
  }
  else {
    std::cerr << error.toUtf8().data() << std::endl;
    return 1;
  }
}
