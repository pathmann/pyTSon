#ifndef PYTHONQTPYTSON_H
#define PYTHONQTPYTSON_H

#include <QObject>

#include <QVariantList>

#include "eventfilterobject.h"

class QLayout;

class pytsondecorator: public QObject {
  Q_OBJECT

  public slots:
    EventFilterObject* new_EventFilterObject(const QList<int>& types = QList<int>(), QObject* parent = 0) { return new EventFilterObject(types, parent); }
    void delete_EventFilterObject(EventFilterObject* obj) { delete obj; }

    const QVariantList getContentsMargins(QLayout* layout);
};

#endif // PYTHONQTPYTSON_H
