#include "eventfilterobject.h"

EventFilterObject::EventFilterObject(const QList<int>& types, QObject* parent): QObject(parent), m_types(types) {

}

bool EventFilterObject::eventFilter(QObject* watched, QEvent* event) {
  if (m_types.contains(event->type())) {
    m_filterresult = false;
    emit eventFiltered(watched, event);
    return m_filterresult;
  }

  return QObject::eventFilter(watched, event);
}

void EventFilterObject::setFilterResult(bool res) {
  m_filterresult = res;
}

const QList<int>& EventFilterObject::types() const {
  return m_types;
}

void EventFilterObject::addType(int type) {
  if (!m_types.contains(type))
    m_types.append(type);
}

void EventFilterObject::removeType(int type) {
  int id = m_types.indexOf(type);
  if (id != -1)
    m_types.removeAt(id);
}
