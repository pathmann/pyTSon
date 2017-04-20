#include "dialog.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent): QDialog(parent), m_curpath(QDir::current()) {
  setupUi(this);
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
  QFile file(fileEdit->text());

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::critical(this, tr("Error"), tr("Error opening file"));
    return;
  }

  QString script = file.readAll();
  file.close();

  PyRun_SimpleString(script.toUtf8().constData());
}

void Dialog::on_consoleButton_clicked() {
  PyRun_SimpleString("from pytsonui.console import PythonConsole\nconsole = PythonConsole()\nconsole.show()");
}
