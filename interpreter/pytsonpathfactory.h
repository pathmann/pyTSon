#ifndef PYTSONPATHFACTORY_H
#define PYTSONPATHFACTORY_H

#include <string>

#if defined(__gnu_linux__)
  #define PATHSEP "/"
  #define PATHDELIM ":"
#elif defined(_WIN32)
  #define PATHSEP "\\"
  #define PATHDELIM ";"
#else
  //mac
  #define PATHSEP "/"
  #define PATHDELIM ":"
#endif

class pytsonpathfactory {
  public:
    pytsonpathfactory(const char* execname);

    const std::string basepath() const { return m_base; }

    const std::string includePath() const { return m_inc; }
    const std::string libPath() const { return m_lib; }
    const std::string dynloadPath() const { return m_dynload; }
    const std::string sitepackPath() const { return m_site; }

    const std::string executablePath() const { return m_exec; }

    const std::string moduleSearchPath() const;
  private:
    std::string m_base;
    std::string m_exec;
    std::string m_inc;
    std::string m_lib;
    std::string m_dynload;
    std::string m_site;
};

#endif // PYTSONPATHFACTORY_H
