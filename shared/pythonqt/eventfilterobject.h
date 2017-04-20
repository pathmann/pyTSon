#ifndef EVENTFILTEROBJECT_H
#define EVENTFILTEROBJECT_H

#include <QObject>

#include <QList>
#include <QEvent>

class EventFilterObject: public QObject {
  Q_OBJECT

  public:
    EventFilterObject(const QList<int>& types = QList<int>(), QObject* parent = 0);

  public slots:
    void setFilterResult(bool res);

    const QList<int>& types() const;
    void addType(int type);
    void removeType(int type);
  signals:
    void eventFiltered(QObject* obj, QEvent* event);

  protected:
    bool eventFilter(QObject* watched, QEvent* event);

  private:
    QList<int> m_types;
    bool m_filterresult;
};

#endif // EVENTFILTEROBJECT_H
