#include "dummyts3lib.h"

static PyModuleDef mdef = {
  PyModuleDef_HEAD_INIT,
  "ts3lib",
  NULL,
  -1,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC PyInit_dummyts3lib(void) {
 return PyModule_Create(&mdef);
}
