#include "plugin.h"

#include "pythonhost.h"
#include "ts3logdispatcher.h"

#include <QString>

#ifdef PYTSON_PARSER
#define PLUGIN_HOST_CALL (void*)
#define LOG_ADD (void*)
#define QSTR (void*)
#else
#define PLUGIN_HOST_CALL PythonHost::instance()->callMethod
#define LOG_ADD ts3logdispatcher::instance()->add
#define QSTR QString
#endif

/*
# coding: utf-8

import ts3defines


class PluginMount(type):
    def __init__(cls, name, bases, attrs):
        super(PluginMount, cls).__init__(name, bases, attrs)
        
        
class ts3plugin(object):
    __metaclass__ = PluginMount
    """

    """

    requestAutoload = False
    """"""
    name = "__ts3plugin__"
    """"""
    version = "1.0"
    """"""
    apiVersion = 20
    """"""
    author = "Thomas \"PLuS\" Pathmann"
    """"""
    description = "This is the baseclass for all ts3 python plugins"
    """"""
    offersConfigure = False
    """"""
    commandKeyword = "py"
    """"""
    infoTitle = "pyTSon"
    """"""
    menuItems = [(ts3defines.PLUGIN_MENU_TYPE_CLIENT, 0, "text", "icon.png")]
    """"""
    hotkeys = [("keyword", "description")]
    """"""
    
    def __init__(self):
        """
        
        """
    
    def stop(self):
        """
        
        """
    
    def menuCreated(self):
        """

        """

    def configure(self, qParentWidget):
        """
        
        @param qParentWidget:
        @type qParentWidget:
        """
        
    def infoData(self, schid, id, atype):
        """
        @param schid:
        @type schid:
        @param id:
        @type id:
        @param atype:
        @type atype:
        """
        
    def processCommand(self, schid, command):
        """
        @param schid:
        @type schid:
        @param command:
        @type command:
        """
        
    def onServerErrorEvent(self, schid, errorMessage, error, returnCode, extraMessage):
        """

        @param schid:
        @type schid:
        @param errorMessage:
        @type errorMessage:
        @param error:
        @type error:
        @param returnCode:
        @type returnCode:
        @param extraMessage:
        @type extraMessage:
        """
        
    def onTextMessageEvent(self, schid, targetMode, toID, fromID, fromName, fromUniqueIdentifier, message, ffIgnored):
        """

        @param schid:
        @type schid:
        @param targetMode:
        @type targetMode:
        @param toID:
        @type toID:
        @param fromID:
        @type fromID:
        @param fromName:
        @type fromName:
        @param fromUniqueIdentifier:
        @type fromUniqueIdentifier:
        @param message:
        @type message:
        @param ffIgnored:
        @type ffIgnored:
        """

    def onClientPokeEvent(self, schid, fromClientID, pokerName, pokerUniqueIdentity, message, ffIgnored):
        """

        @param schid:
        @type schid:
        @param fromClientID:
        @type fromClientID:
        @param pokerName:
        @type pokerName:
        @param pokerUniqueIdentity:
        @type pokerUniqueIdentity:
        @param message:
        @type message:
        @param ffIgnored:
        @type ffIgnored:
        """
        
    def onServerPermissionErrorEvent(self, schid, errorMessage, error, returnCode, failedPermissionID):
        """

        @param schid:
        @type schid:
        @param errorMessage:
        @type errorMessage:
        @param error:
        @type error:
        @param returnCode:
        @type returnCode:
        @param failedPermissionID:
        @type failedPermissionID:
        """
*/

{% for name, c in callbacks|dictsort %}
{{c.signature}} {
{% if c.body == "" %}
  /*
    def {{name}}(self, {{c.parameterNames | join(", ")}}):
        """
        
        {% for p in c.parameterNames %}
        @param {{p}}:
        @type {{p}}:
        {% endfor %}
        """
  */
    {% if c.parameterNames | length > 0 %}
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(s{{c.formatString}})", "{{name}}", {{c.convertedParameters | join(", ")}})) {
    {% else %}
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(s)", "{{name}}")) {
    {% endif %}
    {% if "serverConnectionHandlerID" in c.parameterNames %}
    LOG_ADD(QSTR("Calling {{name}} failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
    {% else %}
    LOG_ADD(QSTR("Calling {{name}} failed with error \"%1\"").arg(callerror), LogLevel_ERROR);
    {% endif %}
  }
{% else %}
{{c.body}}
{% endif %}
}

{% endfor %}
