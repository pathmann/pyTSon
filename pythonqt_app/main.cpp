#include <Python.h>

#include <iostream>

#include <QApplication>
#include <QDir>

#include "pythonqtapphost.h"
#include "dialog.h"

void printHelp(const char* execname) {
  std::cout << execname << " [-h] [pyTSon-dir]" << std::endl;
  std::cout << "-h\tShow this help" << std::endl;
  std::cout << "pyTSon-dir\tThe directory where pyTSon lives in (this might be your TS3 plugin path)" << std::endl;
}

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  QDir dir;

  for (int i = 1; i < app.arguments().count(); ++i) {
    if (app.arguments()[i] == "-h") {
      printHelp(argv[0]);
      return 0;
    }
    else {
      dir = app.arguments()[i];

      if (!dir.exists()) {
        printHelp(argv[0]);
        return 1;
      }
    }
  }

  QString error;
  PythonQtAppHost pythonhost;
  if (pythonhost.init(dir, error)) {
    if (!error.isEmpty())
      std::cout << error.toUtf8().data() << std::endl;

    Dialog* dlg = new Dialog;
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
