#include "ts3logdispatcher.h"

#include "global_shared.h"


ts3logdispatcher::ts3logdispatcher(): singleton<ts3logdispatcher>() {

}

ts3logdispatcher::~ts3logdispatcher() {

}

void ts3logdispatcher::init(const QString &channel) {
  m_chan = channel;
}

void ts3logdispatcher::add(const QString &msg, LogLevel level, uint64 id, bool totab) {
  ts3_funcs.logMessage(msg.toUtf8().data(), level, m_chan.toUtf8().data(), id);

  if (totab)
    ts3_funcs.printMessageToCurrentTab(msg.toUtf8().data());
}
