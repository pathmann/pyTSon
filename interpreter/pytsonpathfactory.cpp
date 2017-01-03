#include "pytsonpathfactory.h"

#include <stdexcept>

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

  m_lib = std::string(m_inc);
  m_lib += PATHSEP;
  m_lib += LIBDIR;
  //m_lib = ~/.ts3client/plugins/pyTSon/include/Lib

  m_dynload = std::string(m_lib);
  m_dynload += PATHSEP;
  m_dynload += "lib-dynload";
  //m_dynload = ~/.ts3client/plugins/pyTSon/include/Lib/lib-dynload

  m_site = std::string(m_lib);
  m_site += PATHSEP;
  m_site += "site-packages";
  //m_site = ~/.ts3client/plugins/pyTSon/include/site-packages
}

const std::string pytsonpathfactory::moduleSearchPath() const {
  std::string ret(m_lib);
  ret += PATHDELIM;
  ret += m_dynload;
  ret += PATHDELIM;
  ret += m_site;
  // = ~/.ts3client/plugins/pyTSon/include/Lib:~/.ts3client/plugins/pyTSon/include/Lib/lib-dynload

  return ret;
}
