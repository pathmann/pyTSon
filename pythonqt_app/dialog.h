#ifndef DIALOG_H
#define DIALOG_H

#include <Python.h>

#include <QDir>

#include "ui_dialog.h"

class Dialog : public QDialog, private Ui::pythonqtapp {
    Q_OBJECT

  public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
  private slots:
    void on_fileButton_clicked();
    void on_fileEdit_textChanged(const QString &arg1);
    void on_consoleButton_clicked();
    void on_executeButton_clicked();
  private:
    QDir m_curpath;
};

#endif // DIALOG_H
