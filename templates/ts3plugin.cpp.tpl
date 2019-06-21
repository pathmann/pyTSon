#include "ts3plugin.h"

#include "global_shared.h"
#include "ts3logdispatcher.h"

#include <QString>

#ifdef PYTSON_PARSER
#define PLUGIN_HOST_CALL (void*)
#define LOG_ADD (void*)
#define QSTR (void*)
#else
#define PLUGIN_HOST_CALL pyhost->callMethod
#define LOG_ADD ts3logdispatcher::instance()->add
#define QSTR QString
#endif

/*
# coding: utf-8

import ts3defines
from pluginhost import PluginHost

class PluginMount(type):
    """
    Mountpoint for ts3plugins.
    This class is used as metaclass for ts3plugin-subclasses to autodetect classes and add them to the PluginHost.
    """

    def __init__(cls, name, bases, attrs):
        super(PluginMount, cls).__init__(name, bases, attrs)
        if not hasattr(PluginHost, 'plugins'):
            PluginHost.plugins = {}
            PluginHost.active = {}
        else:
            for a in ['requestAutoload', 'name', 'version', 'apiVersion', 'author', 'description', 'offersConfigure', 'commandKeyword', 'infoTitle', 'menuItems', 'hotkeys']:
                if not hasattr(cls, a):
                    err = ts3lib.logMessage("Plugin %s not loaded, missing required attribute %s" % (name, a), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginMount.init", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Plugin %s not loaded, missing required attribute %s" % (name, a))

                    return

            if not cls.name in PluginHost.plugins:
                PluginHost.plugins[cls.name] = cls
            else:
                err = ts3lib.logMessage("Error loading python plugin %s, already registered or a plugin with that name already exist" % cls.name, ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginMount.init", 0)
                if err != ts3defines.ERROR_ok:
                    print("Error loading python plugin %s, already registered or a plugin with that name already exist" % cls.name)


class ts3plugin(object):
    __metaclass__ = PluginMount
    """
    Base class for all pyTSon plugins. Subclass this to receive events from the client. pyTSon will automatically recognize this class on startup or on reload.
    """

    requestAutoload = False
    """If set to True, the plugin is automatically loaded on startup. This check is only done once per new plugin, after that users can enable/disable the plugin."""
    name = "__ts3plugin__"
    """The name of the plugin. Use meaningful names. It has to be unique in the list of plugins."""
    version = "1.0"
    """Version string of the plugin. pyTSon will use this string to determine, if a new version is available in an online repository."""
    apiVersion = 21
    """apiVersion the plugin was developed for."""
    author = "Thomas \"PLuS\" Pathmann"
    """Let the world know who made the plugin."""
    description = "This is the baseclass for all ts3 python plugins"
    """Explain, what the plugin does."""
    offersConfigure = False
    """Set this to True, if the plugin offers a configuration ui. In this case the method configure is called."""
    commandKeyword = "py"
    """Set this to a keyword (non-empty) your plugin can be called with. Users may type /py <thecommand> [moreargs]. The method processCommand will be called with any additional args."""
    infoTitle = "pyTSon"
    """If set to a string, this title is shown in the info frame of the client on top of the infoData. If set to None, nothing is shown and infoData won't be called."""
    menuItems = [(ts3defines.PluginMenuType.PLUGIN_MENU_TYPE_CLIENT, 0, "text", "icon.png")]
    """List of tuple(int, int, str, str) containing the menuitems. The tuple has to contain the type (see ts3defines.PluginMenuType), an int identifier (unique in this list), the title and the name of the icon. The icon has to be a path relative to pytson.getPluginPath(). Pass an empty string to omit using an icon. The method onMenuItemEvent with menuItemID=identifier is called."""
    hotkeys = [("keyword", "description")]
    """List of tuple(str, str) containing the hotkeys. The tuple has to contain a string identifier (unique in this list) and a description shown in the TS3 Client's hotkey dialog. The method onHotkeyEvent with keyword=identifier is called."""

    def __init__(self):
        """
        Initializes the plugin. This is called if the plugin is started. After this, the plugin's event-methods will be invoked.
        """

    def stop(self):
        """
        This is called, when the plugin is stopped. After this, event-methods won't be invoked any longer.
        """

    def menuCreated(self):
        """
        This is called after the plugin's menuitems are created or the plugin was reactivated. Plugin developers can assume, that when this is called, all menuitems are enabled, disable them with ts3lib.setPluginMenuEnabled if necessary.
        """

    def configure(self, qParentWidget):
        """
        This is called to show the plugin's configuration ui.
        @param qParentWidget: the reference to pyTSon's configdialog
        @type qParentWidget: pytsonui.ConfigurationDialog
        """

    def infoData(self, schid, aid, atype):
        """
        If the classvariable infoTitle is not None, this is called to show information on a treeitem of the TS3 Client.
        @param schid: the ID of the serverconnection
        @type schid: int
        @param aid: the id (channel or client id) of the object represented by the treeitem
        @type aid: int
        @param atype: type of the treeitem (see ts3defines.PluginItemType)
        @type atype: int
        @return: list of strings shown in the client (will be joined by a newline)
        @rtype: list[str]
        """

    def processCommand(self, schid, command):
        """
        If the classvariable commandKeyword is set to a string (non-empty), this is called if the user requests a command by typing /py commandKeyword [args].
        @param schid: the ID of the serverconnection
        @type schid: int
        @param command: the additional arguments passed by the user
        @type command: str
        @return: True, if the plugin handled the command, otherwise the user will receive an error
        @rtype: bool
        """

    def onServerErrorEvent(self, schid, errorMessage, error, returnCode, extraMessage):
        """
        This is the global error event. Independent from the return value, all pyTSon plugins will receive this event.
        @param schid: the ID of the serverconnection
        @type schid: int
        @param errorMessage: the message
        @type errorMessage: str
        @param error: the errorcode (see ts3defines.ERROR_*)
        @type error: int
        @param returnCode: the returnCode of the error passed to the causal method or an empty string, if no returnCode was passed
        @type returnCode: str
        @param extraMessage: additional error information
        @type extraMessage: str
        @return: True, if the plugin handled the command, so the client will ignore it. If no returnCode was passed, this return value will be ignored
        @rtype: bool
        """

    def onTextMessageEvent(self, schid, targetMode, toID, fromID, fromName, fromUniqueIdentifier, message, ffIgnored):
        """
        This is called when the client receives a textmessage from another client. Independent from the return value, all pyTSon plugins will receive this event.
        @param schid: the ID of the serverconnection
        @type schid: int
        @param targetMode: the target of the message (see ts3defines.TextMessageTargetMode)
        @type targetMode: int
        @param toID: the id of the receiver (client or channel)
        @type toID: int
        @param fromID: the client id of the sending client
        @type fromID: int
        @param fromName: the current nick of the sending client
        @type fromName: str
        @param fromUniqueIdentifier: the uid of the sending client
        @type fromUniqueIdentifier: str
        @param message: the message
        @type message: str
        @param ffIgnored: if set to a value != 0, the client will ignore this message independent from the return value (eg. the friend/foe manager kicked in)
        @type ffIgnored: int
        @return: True, if the plugin handled the message, so the client will ignore the message
        @rtype: bool
        """

    def onClientPokeEvent(self, schid, fromClientID, pokerName, pokerUniqueIdentity, message, ffIgnored):
        """
        This is called when the client is poked by another client. Independent from the return value, all pyTSon plugins will receive this event.
        @param schid: the ID of the serverconnection
        @type schid: int
        @param fromClientID: the id of the poking client
        @type fromClientID: int
        @param pokerName: the current nick of the poking client
        @type pokerName: str
        @param pokerUniqueIdentity: the uid of the poking client
        @type pokerUniqueIdentity: str
        @param message: the poke message
        @type message: str
        @param ffIgnored: if set to a value != 0, the client will ignore this message independent from the return value (eg. the friend/foe manager kicked in)
        @type ffIgnored: int
        @return: True, if the plugin handled the poke, so the client will ignore it
        @rtype: bool
        """

    def onServerPermissionErrorEvent(self, schid, errorMessage, error, returnCode, failedPermissionID):
        """
        This is the global error event for permission errors. Independent from the return value, all pyTSon plugins will receive this event.
        @param schid: the ID of the serverconnection
        @type schid: int
        @param errorMessage: the message
        @type errorMessage: str
        @param error: the errorcode (see ts3defines.ERROR_*)
        @type error: int
        @param returnCode: the returnCode of the error passed to the causal method or an empty string, if no returnCode was passed
        @type returnCode: str
        @param failedPermissionID: id of the permission
        @type failedPermissionID: int
        @return: True, if the plugin handled the error, so the client will ignore it. If no returnCode was passed, this return value will be ignored
        @rtype: bool
        """

    def onUserLoggingMessageEvent(self, logMessage, logLevel, logChannel, logID, logTime, completeLogString):
        """
        This is called whenever a message is added to the clientlog. You should not call ts3lib.logMessage in this event to prevent infinite loops. This event can be called asynchronous if called from another thread than the mainthread.
        @param logMessage: the message that has been logged
        @type logMessage: str
        @param logLevel: the level of the message (see ts3defines.LogLevel)
        @type logLevel: int
        @param logChannel: the logchannel of the message
        @type logChannel: str
        @param logID: the id of the server connection handler it the message is connected to one, otherwise set to 0
        @type logID: int
        @param logTime: the time of the message as unix timestamp
        @type logTime: int
        @param completeLogString: all infos concatenated as string
        @type completeLogString: str
        """

    def onFileTransferStatusEvent(self, transferID, status, statusMessage, remotefileSize, schid):
        """
        This is called whenever a filetransfer's status changed. This event is called asynchronous.
        @param transferID: the id of the filetransfer
        @type transferID: int
        @param status: the new status
        @type status: int
        @param statusMessage: a statusmessage
        @type statusMessage: str
        @param remotefileSize: size of the file on the remote site (if uploading, this is the incompletefilesize)
        @type remotefileSize: int
        @param schid: the ID of the serverconnection
        @type schid: int
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
