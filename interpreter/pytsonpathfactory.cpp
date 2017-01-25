#include "pytsonpathfactory.h"

#if defined(_WIN32)
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif

#include <stdexcept>
#include <stdlib.h>

pytsonpathfactory::pytsonpathfactory(const char* execname) {
  std::string tmp(execname);

  char* norm = realpath(tmp.c_str(), NULL);
  m_exec = std::string(norm);
  free(norm);

  //delete binary name
  m_base = m_exec.substr(0, m_exec.find_last_of("\\/"));
  //m_base = ~/.ts3client/plugins/pyTSon

  m_inc = std::string(m_base);
  m_inc += PATHSEP;
  m_inc += "include";
  //m_inc = ~/.ts3client/plugins/pyTSon/include

  m_lib = std::string(m_base);
  m_lib += PATHSEP;
  m_lib += "lib";
#ifndef _WIN32
  m_lib += PATHSEP;
  m_lib += "python3.5";
#endif
  //m_lib = ~/.ts3client/plugins/pyTSon/lib/python3.5

  m_dynload = std::string(m_lib);
  m_dynload += PATHSEP;
  m_dynload += "lib-dynload";
  //m_dynload = ~/.ts3client/plugins/pyTSon/lib/python3.5m/lib-dynload

  m_site = std::string(m_lib);
  m_site += PATHSEP;
  m_site += "site-packages";
  //m_site = ~/.ts3client/plugins/pyTSon/lib/python3.5m/site-packages
}

const std::string pytsonpathfactory::moduleSearchPath() const {
  std::string ret(m_lib);
  ret += PATHDELIM;
  ret += m_dynload;
  ret += PATHDELIM;
  ret += m_site;
  // = ~/.ts3client/plugins/pyTSon/lib/python3.5m/lib:~/.ts3client/plugins/pyTSon/lib/python3.5m/lib-dynload

  return ret;
}
