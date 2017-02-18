#ifndef TS3CALLBACKARGUMENTS_H
#define TS3CALLBACKARGUMENTS_H

#include <Python.h>

#include <QString>
#include <QVector>
#include <QVariant>

class ts3callbackarguments {
  public:
    ts3callbackarguments();

    bool init(QString& error, const char* format, va_list vl);

    void addInt(int val);
    void addUnsignedInt(unsigned int val);
    void addUnsignedLongLong(unsigned long long val);
    void addString(char* val);

    PyObject* toPythonTuple(QString& error) const;

  private:
    QVector<char> m_formats;
    QVector<QVariant> m_values;
};

#endif // TS3CALLBACKARGUMENTS_H
