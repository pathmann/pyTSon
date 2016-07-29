#ifndef TS3LOGDISPATCHER_H
#define TS3LOGDISPATCHER_H

#include "singleton.h"

#include "teamspeak/public_definitions.h"
#include "teamlog/logtypes.h"

#include <QString>

class ts3logdispatcher: public singleton<ts3logdispatcher> {
    friend class singleton<ts3logdispatcher>;
  public:
    void init(const QString& channel);

    void add(const QString &msg, LogLevel level = LogLevel_INFO, uint64 id = 0, bool totab = false);
  private:
    ts3logdispatcher();
    ~ts3logdispatcher();

    QString m_chan;
};

#endif // TS3LOGDISPATCHER_H
