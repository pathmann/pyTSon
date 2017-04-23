#include "dialog.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

Dialog::Dialog(const QString& script, bool openconsole, QWidget *parent): QDialog(parent), m_curpath(QDir::current()) {
  setupUi(this);

  PyRun_SimpleString("from pytsonui.console import PythonConsole\nconsole = None");

  if (openconsole)
    openConsole();

  if (!script.isEmpty()) {
    fileEdit->setText(script);
    executeScript(script);
  }
}

Dialog::~Dialog() {

}

void Dialog::on_fileButton_clicked() {
  QString filename = QFileDialog::getOpenFileName(this, tr("Open script"), m_curpath.absolutePath(), tr("Python scripts (*.py)"));

  if (!filename.isEmpty()) {
    fileEdit->setText(filename);
    m_curpath = QFileInfo(filename).absoluteDir();
  }
}

void Dialog::on_fileEdit_textChanged(const QString &arg1) {
  executeButton->setEnabled(QFile::exists(arg1));
}

void Dialog::on_executeButton_clicked() {
  executeScript(fileEdit->text());
}

void Dialog::on_consoleButton_clicked() {
  openConsole();
}

void Dialog::executeScript(const QString &filename) {
  FILE* file = fopen(filename.toUtf8().constData(), "r");
  if (!file) {
    QMessageBox::critical(this, tr("Error"), tr("Error opening file"));
    return;
  }

  PyRun_SimpleFileEx(file, filename.toUtf8().constData(), 1);
}

void Dialog::openConsole() {
  PyRun_SimpleString("if not console:\n    console = PythonConsole(catchstd=True)\nconsole.show()");
}
