#ifndef PYTHONQTPYTSON_H
#define PYTHONQTPYTSON_H

#include <QObject>

#include "eventfilterobject.h"


class pytsondecorator: public QObject {
  Q_OBJECT

  public slots:
    EventFilterObject* new_EventFilterObject(const QList<int>& types = QList<int>(), QObject* parent = 0) { return new EventFilterObject(types, parent); }
    void delete_EventFilterObject(EventFilterObject* obj) { delete obj; }
};

#endif // PYTHONQTPYTSON_H
