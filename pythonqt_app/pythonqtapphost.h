#ifndef PYTHONQTAPPHOST_H
#define PYTHONQTAPPHOST_H

#include "pythonhost.h"

#include "dummyts3lib.h"

class PythonQtAppHost: public PythonHost {
  public:
    PythonQtAppHost(): PythonHost() {
      m_inittabs.append({"ts3lib", PyInit_dummyts3lib});
    }
};

#endif // PYTHONQTAPPHOST_H
