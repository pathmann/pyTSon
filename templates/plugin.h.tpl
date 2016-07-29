#ifndef PLUGIN_H__
#define PLUGIN_H__

#ifdef WIN32
#define EXPORT_SYMBOL __declspec(dllexport)
#else
#define EXPORT_SYMBOL __attribute__ ((visibility("default")))
#endif


#include "teamspeak/public_definitions.h"
#include "plugin_definitions.h"

#ifndef PYTSON_PARSER
#ifdef __cplusplus
extern "C" {
#endif
#endif

{% for name, c in callbacks.items() %}
EXPORT_SYMBOL {{c.signature}};
{% endfor %}

#ifndef PYTSON_PARSER
#ifdef __cplusplus
}
#endif
#endif

#endif

