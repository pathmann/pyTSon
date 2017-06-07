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

void ts3plugin_currentServerConnectionChanged(uint64 serverConnectionHandlerID) {
  /*
    def currentServerConnectionChanged(self, serverConnectionHandlerID):
        """
        This is called when the current serverconnection changed (the user switched between tabs)
        @param serverConnectionHandlerID: id of the new serverconnectionhandler
        @type serverConnectionHandlerID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "currentServerConnectionChanged", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling currentServerConnectionChanged failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onAvatarUpdated(uint64 serverConnectionHandlerID, anyID clientID, const char *avatarPath) {
  /*
    def onAvatarUpdated(self, serverConnectionHandlerID, clientID, avatarPath):
        """
        This is called when a client's avatar changed.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the client's id
        @type clientID: int
        @param avatarPath: path to the avatar
        @type avatarPath: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIs)", "onAvatarUpdated", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID,  avatarPath)) {
    LOG_ADD(QSTR("Calling onAvatarUpdated failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onBanListEvent(uint64 serverConnectionHandlerID, uint64 banid, const char *ip, const char *name, const char *uid, uint64 creationTime, uint64 durationTime, const char *invokerName, uint64 invokercldbid, const char *invokeruid, const char *reason, int numberOfEnforcements, const char *lastNickName) {
  /*
    def onBanListEvent(self, serverConnectionHandlerID, banid, ip, name, uid, creationTime, durationTime, invokerName, invokercldbid, invokeruid, reason, numberOfEnforcements, lastNickName):
        """
        This is called for each entry in the server's banlist after it was requested with ts3lib.requestBanList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param banid: id of the ban
        @type banid: int
        @param ip: the banned ip pattern or an empty string
        @type ip: str
        @param name: the banned name pattern or an empty string
        @type name: str
        @param uid: the banned uid or an empty string
        @type uid: str
        @param creationTime: time the ban was created as unix timestamp
        @type creationTime: int
        @param durationTime: duration of the ban in seconds
        @type durationTime: int
        @param invokerName: nick of the creator (at time the ban was created)
        @type invokerName: str
        @param invokercldbid: database id of the creator
        @type invokercldbid: int
        @param invokeruid: uid of the creator
        @type invokeruid: str
        @param reason: reason for ban
        @type reason: str
        @param numberOfEnforcements: number of times, the ban has been enforced since
        @type numberOfEnforcements: int
        @param lastNickName: last nickname of the last enforced client
        @type lastNickName: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsssKKsKssis)", "onBanListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) banid,  ip,  name,  uid, (unsigned long long) creationTime, (unsigned long long) durationTime,  invokerName, (unsigned long long) invokercldbid,  invokeruid,  reason,  numberOfEnforcements,  lastNickName)) {
    LOG_ADD(QSTR("Calling onBanListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelClientPermListEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 clientDatabaseID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onChannelClientPermListEvent(self, serverConnectionHandlerID, channelID, clientDatabaseID, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        This is called for each granted permission of a client in a specific channel requested with ts3lib.requestChannelClientPermList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: id of the channel
        @type channelID: int
        @param clientDatabaseID: the client's database id
        @type clientDatabaseID: int
        @param permissionID: id of the permission
        @type permissionID: int
        @param permissionValue: value of the permission
        @type permissionValue: int
        @param permissionNegated: the negated flag
        @type permissionNegated: int
        @param permissionSkip: the skip flag
        @type permissionSkip: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKIiii)", "onChannelClientPermListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned long long) clientDatabaseID,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onChannelClientPermListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelClientPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 clientDatabaseID) {
  /*
    def onChannelClientPermListFinishedEvent(self, serverConnectionHandlerID, channelID, clientDatabaseID):
        """
        This is called after each permission yielded by onChannelClientPermListEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: id of the channel
        @type channelID: int
        @param clientDatabaseID: the client's database id
        @type clientDatabaseID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKK)", "onChannelClientPermListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned long long) clientDatabaseID)) {
    LOG_ADD(QSTR("Calling onChannelClientPermListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelDescriptionUpdateEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onChannelDescriptionUpdateEvent(self, serverConnectionHandlerID, channelID):
        """
        This is called whenever a channel's description is updated.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: id of the channel
        @type channelID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelDescriptionUpdateEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onChannelDescriptionUpdateEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelGroupListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, const char *name, int type, int iconID, int saveDB) {
  /*
    def onChannelGroupListEvent(self, serverConnectionHandlerID, channelGroupID, name, atype, iconID, saveDB):
        """
        This is called for each channelgroup on the server requested with ts3lib.requestChannelGroupList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelGroupID: id of the channelgroup
        @type channelGroupID: int
        @param name: name of the channelgroup
        @type name: str
        @param atype: defines if the channelgroup is a templategroup (value==0) or a regular one (value==1)
        @type atype: int
        @param iconID: id of the icon displayed for members or 0 if no icon is displayed
        @type iconID: int
        @param saveDB: set to 1 if memberships are saved to the server's database, otherwise set to 0
        @type saveDB: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsiii)", "onChannelGroupListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelGroupID,  name,  type,  iconID,  saveDB)) {
    LOG_ADD(QSTR("Calling onChannelGroupListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelGroupListFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onChannelGroupListFinishedEvent(self, serverConnectionHandlerID):
        """
        This is called after each channelgroup yielded by onChannelGroupListEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onChannelGroupListFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onChannelGroupListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelGroupPermListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onChannelGroupPermListEvent(self, serverConnectionHandlerID, channelGroupID, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        This is called for each granted permission assigned to a channelgroup requested with ts3lib.requestChannelGroupPermList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelGroupID: id of the channelgroup
        @type channelGroupID: int
        @param permissionID: id of the permission
        @type permissionID: int
        @param permissionValue: value of the permission
        @type permissionValue: int
        @param permissionNegated: negated flag of the permission
        @type permissionNegated: int
        @param permissionSkip: skip flag of the permission
        @type permissionSkip: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIiii)", "onChannelGroupPermListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelGroupID,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onChannelGroupPermListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelGroupPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID) {
  /*
    def onChannelGroupPermListFinishedEvent(self, serverConnectionHandlerID, channelGroupID):
        """
        This is called after each permission yielded by onChannelGroupPermListEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelGroupID: id of the channelgroup
        @type channelGroupID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelGroupPermListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelGroupID)) {
    LOG_ADD(QSTR("Calling onChannelGroupPermListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelMoveEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier) {
  /*
    def onChannelMoveEvent(self, serverConnectionHandlerID, channelID, newChannelParentID, invokerID, invokerName, invokerUniqueIdentifier):
        """
        This is called whenever a channel is moved to a new parent. If a channel is moved without changing the parent, onUpdateChannelEditedEvent is called instead.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: id of the moved channel
        @type channelID: int
        @param newChannelParentID: id of the new parent channel
        @type newChannelParentID: int
        @param invokerID: id of the moving client
        @type invokerID: int
        @param invokerName: nick of the moving client
        @type invokerName: str
        @param invokerUniqueIdentifier: uid of the moving client
        @type invokerUniqueIdentifier: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKIss)", "onChannelMoveEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned long long) newChannelParentID, (unsigned int) invokerID,  invokerName,  invokerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onChannelMoveEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelPasswordChangedEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onChannelPasswordChangedEvent(self, serverConnectionHandlerID, channelID):
        """
        This is called whenever a channelpassword is changed.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: if of the channel
        @type channelID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelPasswordChangedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onChannelPasswordChangedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelPermListEvent(uint64 serverConnectionHandlerID, uint64 channelID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onChannelPermListEvent(self, serverConnectionHandlerID, channelID, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        This is called for each granted permission of a channel requested by ts3lib.requestChannelPermList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the id of the channel
        @type channelID: int
        @param permissionID: the id of the permission
        @type permissionID: int
        @param permissionValue: the value of the permission
        @type permissionValue: int
        @param permissionNegated: negated flag of the permission
        @type permissionNegated: int
        @param permissionSkip: skip flag of the permission
        @type permissionSkip: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIiii)", "onChannelPermListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onChannelPermListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onChannelPermListFinishedEvent(self, serverConnectionHandlerID, channelID):
        """
        This is called after each permission yielded by onChannelPermListEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the id of the channel
        @type channelID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelPermListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onChannelPermListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelSubscribeEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onChannelSubscribeEvent(self, serverConnectionHandlerID, channelID):
        """
        This is called whenever a channel was subscribed.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the id of the channel
        @type channelID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelSubscribeEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onChannelSubscribeEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelSubscribeFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onChannelSubscribeFinishedEvent(self, serverConnectionHandlerID):
        """
        This is called after the subscription on a server has finished (either after subscribing one channel, after all subscriptions of a channel family has been yielded by onChannelSubscribeEvent or after all subscriptions had been reset after connecting).
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onChannelSubscribeFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onChannelSubscribeFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelUnsubscribeEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onChannelUnsubscribeEvent(self, serverConnectionHandlerID, channelID):
        """
        This is called whenever a channel was unsubscribed.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the id of the channel
        @type channelID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelUnsubscribeEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onChannelUnsubscribeEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelUnsubscribeFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onChannelUnsubscribeFinishedEvent(self, serverConnectionHandlerID):
        """
        This is called after the subscription on a server has finished (either after unsubscribing one channel or after all unsubscriptions of a channel family has been yielded by onChannelUnsubscribeEvent).
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onChannelUnsubscribeFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onChannelUnsubscribeFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientBanFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char *kickerName, const char *kickerUniqueIdentifier, uint64 time, const char *kickMessage) {
  /*
    def onClientBanFromServerEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, kickerID, kickerName, kickerUniqueIdentifier, time, kickMessage):
        """
        This is called after a client was banned from the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the banned client
        @type clientID: int
        @param oldChannelID: the id of the last channel, the banned client was in
        @type oldChannelID: int
        @param newChannelID: always set to 0
        @type newChannelID: int
        @param visibility: always set to ts3defines.Visibility.LEAVE_VISIBILITY
        @type visibility: int
        @param kickerID: id of the banning client
        @type kickerID: int
        @param kickerName: nick of the banning client
        @type kickerName: str
        @param kickerUniqueIdentifier: uid of the banning client
        @type kickerUniqueIdentifier: str
        @param time: duration of the ban in seconds
        @type time: int
        @param kickMessage: the kick and ban reason
        @type kickMessage: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKiIssKs)", "onClientBanFromServerEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility, (unsigned int) kickerID,  kickerName,  kickerUniqueIdentifier, (unsigned long long) time,  kickMessage)) {
    LOG_ADD(QSTR("Calling onClientBanFromServerEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientChannelGroupChangedEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, uint64 channelID, anyID clientID, anyID invokerClientID, const char *invokerName, const char *invokerUniqueIdentity) {
  /*
    def onClientChannelGroupChangedEvent(self, serverConnectionHandlerID, channelGroupID, channelID, clientID, invokerClientID, invokerName, invokerUniqueIdentity):
        """
        This is called whenever a client is added to a channelgroup in a specific channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelGroupID: the id of the channelgroup
        @type channelGroupID: int
        @param channelID: the id of the channel
        @type channelID: int
        @param clientID: the id of the client
        @type clientID: int
        @param invokerClientID: the id of the client who added the channelgroup or 0 if the server did
        @type invokerClientID: int
        @param invokerName: the nick of the client who added the channelgroup or "Server" if the server did
        @type invokerName: str
        @param invokerUniqueIdentity: uid of the client who added the channelgroup or an empty string if the server did
        @type invokerUniqueIdentity: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKIIss)", "onClientChannelGroupChangedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelGroupID, (unsigned long long) channelID, (unsigned int) clientID, (unsigned int) invokerClientID,  invokerName,  invokerUniqueIdentity)) {
    LOG_ADD(QSTR("Calling onClientChannelGroupChangedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientChatClosedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientUniqueIdentity) {
  /*
    def onClientChatClosedEvent(self, serverConnectionHandlerID, clientID, clientUniqueIdentity):
        """
        This is called after a client closed the chat to this client (but only after the other client has sent at least one message). This is either invoked by the sdk with ts3lib.clientChatClosed or the user has closed the conversation tab).
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the other client
        @type clientID: int
        @param clientUniqueIdentity: the uid of the other client
        @type clientUniqueIdentity: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIs)", "onClientChatClosedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID,  clientUniqueIdentity)) {
    LOG_ADD(QSTR("Calling onClientChatClosedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientChatComposingEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientUniqueIdentity) {
  /*
    def onClientChatComposingEvent(self, serverConnectionHandlerID, clientID, clientUniqueIdentity):
        """
        This is called whenever another client sends the chat composing command (either invoked by the sdk with ts3lib.clientChatComposing or when the user is really writing in the chat).
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID:
        @type clientID:
        @param clientUniqueIdentity:
        @type clientUniqueIdentity:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIs)", "onClientChatComposingEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID,  clientUniqueIdentity)) {
    LOG_ADD(QSTR("Calling onClientChatComposingEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientDBIDfromUIDEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, uint64 clientDatabaseID) {
  /*
    def onClientDBIDfromUIDEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientDatabaseID):
        """
        This is called whenever a database id was requested with ts3lib.requestClientDBIDfromUID.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param uniqueClientIdentifier: the uid of the requested client
        @type uniqueClientIdentifier: str
        @param clientDatabaseID: the resulting id in the database
        @type clientDatabaseID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKsK)", "onClientDBIDfromUIDEvent", (unsigned long long) serverConnectionHandlerID,  uniqueClientIdentifier, (unsigned long long) clientDatabaseID)) {
    LOG_ADD(QSTR("Calling onClientDBIDfromUIDEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientDisplayNameChanged(uint64 serverConnectionHandlerID, anyID clientID, const char *displayName, const char *uniqueClientIdentifier) {
  /*
    def onClientDisplayNameChanged(self, serverConnectionHandlerID, clientID, displayName, uniqueClientIdentifier):
        """
        This is called whenever a client's displayname changed (nickname or friend/foe manager).
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the client
        @type clientID: int
        @param displayName: the new displayname
        @type displayName: str
        @param uniqueClientIdentifier: the uid of the client
        @type uniqueClientIdentifier: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIss)", "onClientDisplayNameChanged", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID,  displayName,  uniqueClientIdentifier)) {
    LOG_ADD(QSTR("Calling onClientDisplayNameChanged failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientIDsEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, anyID clientID, const char *clientName) {
  /*
    def onClientIDsEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientID, clientName):
        """
        This is called for each client matching a specific uid requested by ts3lib.requestClientIDs.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param uniqueClientIdentifier: the uid of the client
        @type uniqueClientIdentifier: str
        @param clientID: the id of a client
        @type clientID: int
        @param clientName: the nick of the client
        @type clientName: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKsIs)", "onClientIDsEvent", (unsigned long long) serverConnectionHandlerID,  uniqueClientIdentifier, (unsigned int) clientID,  clientName)) {
    LOG_ADD(QSTR("Calling onClientIDsEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientIDsFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onClientIDsFinishedEvent(self, serverConnectionHandlerID):
        """
        This is called after each client yielded by onClientIDsEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onClientIDsFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onClientIDsFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientKickFromChannelEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char *kickerName, const char *kickerUniqueIdentifier, const char *kickMessage) {
  /*
    def onClientKickFromChannelEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, kickerID, kickerName, kickerUniqueIdentifier, kickMessage):
        """
        This is called whenever a client is kicked from a channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the kicked client
        @type clientID: int
        @param oldChannelID: the id of the channel the client was kicked from
        @type oldChannelID: int
        @param newChannelID: the id of the channel the client was kicked to (the default channel)
        @type newChannelID: int
        @param visibility: defines the new state of the client in the view (see ts3defines.Visibility)
        @type visibility: int
        @param kickerID: the id of the kicking client
        @type kickerID: int
        @param kickerName: the nick of the kicking client
        @type kickerName: str
        @param kickerUniqueIdentifier: the uid of the kicking client
        @type kickerUniqueIdentifier: str
        @param kickMessage: the kick reason
        @type kickMessage: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKiIsss)", "onClientKickFromChannelEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility, (unsigned int) kickerID,  kickerName,  kickerUniqueIdentifier,  kickMessage)) {
    LOG_ADD(QSTR("Calling onClientKickFromChannelEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientKickFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char *kickerName, const char *kickerUniqueIdentifier, const char *kickMessage) {
  /*
    def onClientKickFromServerEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, kickerID, kickerName, kickerUniqueIdentifier, kickMessage):
        """
        This is called whenever a client is kicked from the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the kicked client
        @type clientID: int
        @param oldChannelID: the id of the channel the client was in
        @type oldChannelID: int
        @param newChannelID: always set to 0
        @type newChannelID: int
        @param visibility: always set to ts3defines.Visibility.LEAVE_VISIBILITY
        @type visibility: int
        @param kickerID: the id of the kicking client
        @type kickerID: int
        @param kickerName: nick of the kicking client
        @type kickerName: str
        @param kickerUniqueIdentifier: uid of the kicking client
        @type kickerUniqueIdentifier: str
        @param kickMessage: the kick reason
        @type kickMessage: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKiIsss)", "onClientKickFromServerEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility, (unsigned int) kickerID,  kickerName,  kickerUniqueIdentifier,  kickMessage)) {
    LOG_ADD(QSTR("Calling onClientKickFromServerEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char *moveMessage) {
  /*
    def onClientMoveEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, moveMessage):
        """
        This is called whenever a client enters a another channel (moving, joining or leaving the server).
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the client
        @type clientID: int
        @param oldChannelID: the id of the former channel or 0 if the client joined the server
        @type oldChannelID: int
        @param newChannelID: the id of the new channel or 0 if the client disconnected
        @type newChannelID: int
        @param visibility: defines the new state of the client in the view (see ts3defines.Visibility)
        @type visibility: int
        @param moveMessage: the disconnect message if the client left the server or an empty string
        @type moveMessage: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKis)", "onClientMoveEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility,  moveMessage)) {
    LOG_ADD(QSTR("Calling onClientMoveEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientMoveMovedEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char *moverName, const char *moverUniqueIdentifier, const char *moveMessage) {
  /*
    def onClientMoveMovedEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, moverID, moverName, moverUniqueIdentifier, moveMessage):
        """
        This is called whenever a client is moved to another channel by another client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the moved client
        @type clientID: int
        @param oldChannelID: the id of the former channel
        @type oldChannelID: int
        @param newChannelID: the id of the new channel
        @type newChannelID: int
        @param visibility: defines the new state of the client in the view (see ts3defines.Visibility)
        @type visibility: int
        @param moverID: the id of the moving client
        @type moverID: int
        @param moverName: nick of the moving client
        @type moverName: str
        @param moverUniqueIdentifier: uid of the moving client
        @type moverUniqueIdentifier: str
        @param moveMessage: always set to an empty string
        @type moveMessage: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKiIsss)", "onClientMoveMovedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility, (unsigned int) moverID,  moverName,  moverUniqueIdentifier,  moveMessage)) {
    LOG_ADD(QSTR("Calling onClientMoveMovedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility) {
  /*
    def onClientMoveSubscriptionEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility):
        """
        This is called whenever a new client enters the view when subscribing a channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the client
        @type clientID: int
        @param oldChannelID: always set to 0
        @type oldChannelID: int
        @param newChannelID: the id of the subscribed channel
        @type newChannelID: int
        @param visibility: always set to ts3defines.Visibility.ENTER_VISIBILITY
        @type visibility: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKi)", "onClientMoveSubscriptionEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility)) {
    LOG_ADD(QSTR("Calling onClientMoveSubscriptionEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char *timeoutMessage) {
  /*
    def onClientMoveTimeoutEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, timeoutMessage):
        """
        This is called when a client timed out.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the client
        @type clientID: int
        @param oldChannelID: the id of the channel the client was in
        @type oldChannelID: int
        @param newChannelID: always set to 0
        @type newChannelID: int
        @param visibility: always set to ts3defines.Visibility.LEAVE_VISIBILITY
        @type visibility: int
        @param timeoutMessage: the timeout message
        @type timeoutMessage: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKis)", "onClientMoveTimeoutEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility,  timeoutMessage)) {
    LOG_ADD(QSTR("Calling onClientMoveTimeoutEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientNamefromDBIDEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, uint64 clientDatabaseID, const char *clientNickName) {
  /*
    def onClientNamefromDBIDEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientDatabaseID, clientNickName):
        """
        This is called to return the last nickname of a client referenced by the database id after it was requested with ts3lib.requestClientNamefromDBID.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param uniqueClientIdentifier: the uid of the client
        @type uniqueClientIdentifier: str
        @param clientDatabaseID: the database id of the client
        @type clientDatabaseID: int
        @param clientNickName: the last nickname of the client
        @type clientNickName: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKsKs)", "onClientNamefromDBIDEvent", (unsigned long long) serverConnectionHandlerID,  uniqueClientIdentifier, (unsigned long long) clientDatabaseID,  clientNickName)) {
    LOG_ADD(QSTR("Calling onClientNamefromDBIDEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientNamefromUIDEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, uint64 clientDatabaseID, const char *clientNickName) {
  /*
    def onClientNamefromUIDEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientDatabaseID, clientNickName):
        """
        This is called to return the last nickname of a client referenced by the uid after it was requested with ts3lib.requestClientNamefromUID.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param uniqueClientIdentifier: the uid of the client
        @type uniqueClientIdentifier: str
        @param clientDatabaseID: the database id of the client
        @type clientDatabaseID: int
        @param clientNickName: the last nickname of the client
        @type clientNickName: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKsKs)", "onClientNamefromUIDEvent", (unsigned long long) serverConnectionHandlerID,  uniqueClientIdentifier, (unsigned long long) clientDatabaseID,  clientNickName)) {
    LOG_ADD(QSTR("Calling onClientNamefromUIDEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientNeededPermissionsEvent(uint64 serverConnectionHandlerID, unsigned int permissionID, int permissionValue) {
  /*
    def onClientNeededPermissionsEvent(self, serverConnectionHandlerID, permissionID, permissionValue):
        """
        This is called whenever a permission the TS3 client needed changes.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param permissionID: the id of the permission
        @type permissionID: int
        @param permissionValue: the value of the permission
        @type permissionValue: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIi)", "onClientNeededPermissionsEvent", (unsigned long long) serverConnectionHandlerID,  permissionID,  permissionValue)) {
    LOG_ADD(QSTR("Calling onClientNeededPermissionsEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientNeededPermissionsFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onClientNeededPermissionsFinishedEvent(self, serverConnectionHandlerID):
        """
        This is called after each permission yielded by onClientNeededPermissionsEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onClientNeededPermissionsFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onClientNeededPermissionsFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientPermListEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onClientPermListEvent(self, serverConnectionHandlerID, clientDatabaseID, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        This is called for each granted permission to a specific client requested with ts3lib.requestClientPermList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientDatabaseID: the database id of the client
        @type clientDatabaseID: int
        @param permissionID: the id of the permission
        @type permissionID: int
        @param permissionValue: the value of the permission
        @type permissionValue: int
        @param permissionNegated: negated flag of the permission
        @type permissionNegated: int
        @param permissionSkip: skip flag of the permission
        @type permissionSkip: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIiii)", "onClientPermListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) clientDatabaseID,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onClientPermListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID) {
  /*
    def onClientPermListFinishedEvent(self, serverConnectionHandlerID, clientDatabaseID):
        """
        This is called after each permission yielded by onClientPermListEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientDatabaseID: the database id of the client
        @type clientDatabaseID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onClientPermListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) clientDatabaseID)) {
    LOG_ADD(QSTR("Calling onClientPermListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char *oldValue, const char *newValue) {
  /*
    def onClientSelfVariableUpdateEvent(self, serverConnectionHandlerID, flag, oldValue, newValue):
        """
        This is called whenever a variable of the own client is changed.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param flag: the changed variable (see ts3defines.ClientProperties and ts3defines.ClientPropertiesRare)
        @type flag: int
        @param oldValue: the former value
        @type oldValue: str
        @param newValue: the new value
        @type newValue: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKiss)", "onClientSelfVariableUpdateEvent", (unsigned long long) serverConnectionHandlerID,  flag,  oldValue,  newValue)) {
    LOG_ADD(QSTR("Calling onClientSelfVariableUpdateEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientServerQueryLoginPasswordEvent(uint64 serverConnectionHandlerID, const char *loginPassword) {
  /*
    def onClientServerQueryLoginPasswordEvent(self, serverConnectionHandlerID, loginPassword):
        """
        This is called when a new query login was requested.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param loginPassword: the new password
        @type loginPassword: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKs)", "onClientServerQueryLoginPasswordEvent", (unsigned long long) serverConnectionHandlerID,  loginPassword)) {
    LOG_ADD(QSTR("Calling onClientServerQueryLoginPasswordEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onComplainListEvent(uint64 serverConnectionHandlerID, uint64 targetClientDatabaseID, const char *targetClientNickName, uint64 fromClientDatabaseID, const char *fromClientNickName, const char *complainReason, uint64 timestamp) {
  /*
    def onComplainListEvent(self, serverConnectionHandlerID, targetClientDatabaseID, targetClientNickName, fromClientDatabaseID, fromClientNickName, complainReason, timestamp):
        """
        This is called for each entry in the complaintslist.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param targetClientDatabaseID: the database id of the complained client
        @type targetClientDatabaseID: int
        @param targetClientNickName: the last nickname of the complained client
        @type targetClientNickName: str
        @param fromClientDatabaseID: the database id of the complaining client
        @type fromClientDatabaseID: int
        @param fromClientNickName: the last nickname of the complaining client
        @type fromClientNickName: str
        @param complainReason: the reason
        @type complainReason: str
        @param timestamp: the time of the complain as unix timestamp
        @type timestamp: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsKssK)", "onComplainListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) targetClientDatabaseID,  targetClientNickName, (unsigned long long) fromClientDatabaseID,  fromClientNickName,  complainReason, (unsigned long long) timestamp)) {
    LOG_ADD(QSTR("Calling onComplainListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onConnectionInfoEvent(uint64 serverConnectionHandlerID, anyID clientID) {
  /*
    def onConnectionInfoEvent(self, serverConnectionHandlerID, clientID):
        """
        This is called when the connection info of a client has been updated requested with ts3lib.requestConnectionInfo.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the client
        @type clientID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKI)", "onConnectionInfoEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID)) {
    LOG_ADD(QSTR("Calling onConnectionInfoEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber) {
  /*
    def onConnectStatusChangeEvent(self, serverConnectionHandlerID, newStatus, errorNumber):
        """
        This is called whenever the status of a serverconnection changed.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param newStatus: the new status (see ts3defines.ConnectStatus)
        @type newStatus: int
        @param errorNumber: the error (see ts3defines.ERROR_*)
        @type errorNumber: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKiI)", "onConnectStatusChangeEvent", (unsigned long long) serverConnectionHandlerID,  newStatus,  errorNumber)) {
    LOG_ADD(QSTR("Calling onConnectStatusChangeEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onDelChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier) {
  /*
    def onDelChannelEvent(self, serverConnectionHandlerID, channelID, invokerID, invokerName, invokerUniqueIdentifier):
        """
        This is called whenever a channel was deleted.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the id of the channel
        @type channelID: int
        @param invokerID: the id of the client who deleted the channel
        @type invokerID: int
        @param invokerName: the nick of the deleting client
        @type invokerName: str
        @param invokerUniqueIdentifier: the uid of the deleting client
        @type invokerUniqueIdentifier: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIss)", "onDelChannelEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned int) invokerID,  invokerName,  invokerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onDelChannelEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onFileInfoEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char *name, uint64 size, uint64 datetime) {
  /*
    def onFileInfoEvent(self, serverConnectionHandlerID, channelID, name, size, datetime):
        """
        This is called with the fileinfo of a remote file requested with ts3lib.requestFileInfo.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the id of the channel the file is in
        @type channelID: int
        @param name: the full path of the file
        @type name: str
        @param size: the filesize in bytes
        @type size: int
        @param datetime: time the file was last changed as unix timestamp
        @type datetime: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsKK)", "onFileInfoEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID,  name, (unsigned long long) size, (unsigned long long) datetime)) {
    LOG_ADD(QSTR("Calling onFileInfoEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onFileListEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char *path, const char *name, uint64 size, uint64 datetime, int type, uint64 incompletesize, const char *returnCode) {
  /*
    def onFileListEvent(self, serverConnectionHandlerID, channelID, path, name, size, datetime, atype, incompletesize, returnCode):
        """
        This is called for each file and directory in path requested with ts3lib.requestFileList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the id of the channel
        @type channelID: int
        @param path: the path
        @type path: str
        @param name: the filename
        @type name: str
        @param size: the filesize in bytes
        @type size: int
        @param datetime: time the file or directory was last changed as unix timestamp
        @type datetime:
        @param atype: set to 1 if it's a directory, otherwise set to 0
        @type atype: int
        @param incompletesize: the complete filesize in bytes or 0 if the file is already complete
        @type incompletesize: int
        @param returnCode: the returncode passed to the request or an empty string
        @type returnCode: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKssKKiKs)", "onFileListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID,  path,  name, (unsigned long long) size, (unsigned long long) datetime,  type, (unsigned long long) incompletesize,  returnCode)) {
    LOG_ADD(QSTR("Calling onFileListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onFileListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char *path) {
  /*
    def onFileListFinishedEvent(self, serverConnectionHandlerID, channelID, path):
        """
        This is called after each file and directory yielded by onFileListEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID:
        @type channelID: int
        @param path:
        @type path:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKs)", "onFileListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID,  path)) {
    LOG_ADD(QSTR("Calling onFileListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onHotkeyEvent(const char *keyword) {
  /*
    def onHotkeyEvent(self, keyword):
        """
        This is called when a plugin's hotkey is triggered.
        @param keyword: the local keyword set in cls.hotkeys
        @type keyword: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(ss)", "onHotkeyEvent",  keyword)) {
    LOG_ADD(QSTR("Calling onHotkeyEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR);
  }
}

void ts3plugin_onHotkeyRecordedEvent(const char *keyword, const char *key) {
  /*
    def onHotkeyRecordedEvent(self, keyword, key):
        """
        This is called when a hotkey was recorded requested by ts3lib.requestHotkeyInputDialog.
        @param keyword: the keyword
        @type keyword: str
        @param key: the hotkey to trigger the keyword
        @type key: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sss)", "onHotkeyRecordedEvent",  keyword,  key)) {
    LOG_ADD(QSTR("Calling onHotkeyRecordedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR);
  }
}

void ts3plugin_onIncomingClientQueryEvent(uint64 serverConnectionHandlerID, const char *commandText) {
  /*
    def onIncomingClientQueryEvent(self, serverConnectionHandlerID, commandText):
        """
        This callback was designed for the clientquery plugin. It combines many callbacks and is called with a representing string.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param commandText: the text of the client query
        @type commandText: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKs)", "onIncomingClientQueryEvent", (unsigned long long) serverConnectionHandlerID,  commandText)) {
    LOG_ADD(QSTR("Calling onIncomingClientQueryEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID) {
  /*
    def onMenuItemEvent(self, serverConnectionHandlerID, atype, menuItemID, selectedItemID):
        """
        This is called when a plugin's menuitem defined in cls.menuItems is clicked.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param atype: type of the menuitem (see ts3defines.PluginMenuType)
        @type atype: int
        @param menuItemID: the local id of the menuitem defines in cls.menuItems
        @type menuItemID: int
        @param selectedItemID: set to the id of the selected channel if atype is ts3defines.PluginMenuType.PLUGIN_MENU_TYPE_CHANNEL, set to the id of the selected client if atype is ts3defines.PluginMenuType.PLUGIN_MENU_TYPE_CLIENT, otherwise always set to 0
        @type selectedItemID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIiK)", "onMenuItemEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) type,  menuItemID, (unsigned long long) selectedItemID)) {
    LOG_ADD(QSTR("Calling onMenuItemEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onMessageGetEvent(uint64 serverConnectionHandlerID, uint64 messageID, const char *fromClientUniqueIdentity, const char *subject, const char *message, uint64 timestamp) {
  /*
    def onMessageGetEvent(self, serverConnectionHandlerID, messageID, fromClientUniqueIdentity, subject, message, timestamp):
        """
        This is called with the information about an offline message requested with ts3lib.requestMessageGet.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param messageID: the id of the message
        @type messageID: int
        @param fromClientUniqueIdentity: the uid of the message's sender
        @type fromClientUniqueIdentity: str
        @param subject: the subject of the message
        @type subject: str
        @param message: the content of the message
        @type message: str
        @param timestamp: time the message was sent as unix timestamp
        @type timestamp: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsssK)", "onMessageGetEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) messageID,  fromClientUniqueIdentity,  subject,  message, (unsigned long long) timestamp)) {
    LOG_ADD(QSTR("Calling onMessageGetEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onMessageListEvent(uint64 serverConnectionHandlerID, uint64 messageID, const char *fromClientUniqueIdentity, const char *subject, uint64 timestamp, int flagRead) {
  /*
    def onMessageListEvent(self, serverConnectionHandlerID, messageID, fromClientUniqueIdentity, subject, timestamp, flagRead):
        """
        This is called for each offline message available on the server requested with ts3lib.requestMessageList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param messageID: the id of the message
        @type messageID: int
        @param fromClientUniqueIdentity: the uid of the message's sender
        @type fromClientUniqueIdentity: str
        @param subject: the subject of the message
        @type subject: str
        @param timestamp: time the message was sent as unix timestamp
        @type timestamp: int
        @param flagRead: defines the read status of the message
        @type flagRead: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKssKi)", "onMessageListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) messageID,  fromClientUniqueIdentity,  subject, (unsigned long long) timestamp,  flagRead)) {
    LOG_ADD(QSTR("Calling onMessageListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onNewChannelCreatedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier) {
  /*
    def onNewChannelCreatedEvent(self, serverConnectionHandlerID, channelID, channelParentID, invokerID, invokerName, invokerUniqueIdentifier):
        """
        This is called whenever a new channel was created.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the id of the new channel
        @type channelID: int
        @param channelParentID: the id of the parent channel
        @type channelParentID: int
        @param invokerID: the id of the creating client
        @type invokerID: int
        @param invokerName: nick of the creating client
        @type invokerName: str
        @param invokerUniqueIdentifier: the uid of the creating client
        @type invokerUniqueIdentifier: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKIss)", "onNewChannelCreatedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned long long) channelParentID, (unsigned int) invokerID,  invokerName,  invokerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onNewChannelCreatedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onNewChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID) {
  /*
    def onNewChannelEvent(self, serverConnectionHandlerID, channelID, channelParentID):
        """
        This is called whenever a new channel enters the view (at connect).
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the id of the channel
        @type channelID: int
        @param channelParentID: the id of the parent channel
        @type channelParentID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKK)", "onNewChannelEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned long long) channelParentID)) {
    LOG_ADD(QSTR("Calling onNewChannelEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPermissionListEvent(uint64 serverConnectionHandlerID, unsigned int permissionID, const char *permissionName, const char *permissionDescription) {
  /*
    def onPermissionListEvent(self, serverConnectionHandlerID, permissionID, permissionName, permissionDescription):
        """
        This is called for each permission on the server requested with ts3lib.requestPermissionList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param permissionID: id of the permission
        @type permissionID: int
        @param permissionName: name of the permission
        @type permissionName: str
        @param permissionDescription: description of the permission
        @type permissionDescription: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIss)", "onPermissionListEvent", (unsigned long long) serverConnectionHandlerID,  permissionID,  permissionName,  permissionDescription)) {
    LOG_ADD(QSTR("Calling onPermissionListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPermissionListFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onPermissionListFinishedEvent(self, serverConnectionHandlerID):
        """
        This is called after each permission yielded by onPermissionListEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onPermissionListFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onPermissionListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPermissionListGroupEndIDEvent(uint64 serverConnectionHandlerID, unsigned int groupEndID) {
  /*
    def onPermissionListGroupEndIDEvent(self, serverConnectionHandlerID, groupEndID):
        """
        This is called for each last permission in the groups of permissions after requesting the permissionlist with ts3lib.requestPermissionList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param groupEndID: id of the last permission in group
        @type groupEndID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKI)", "onPermissionListGroupEndIDEvent", (unsigned long long) serverConnectionHandlerID,  groupEndID)) {
    LOG_ADD(QSTR("Calling onPermissionListGroupEndIDEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPermissionOverviewEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, uint64 channelID, int overviewType, uint64 overviewID1, uint64 overviewID2, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onPermissionOverviewEvent(self, serverConnectionHandlerID, clientDatabaseID, channelID, overviewType, overviewID1, overviewID2, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        This is called for each permission of a pair of client and channel requested with ts3lib.requestPermissionOverview.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientDatabaseID: the database id of the client
        @type clientDatabaseID: int
        @param channelID: the id of the channel
        @type channelID: int
        @param overviewType: defines the type of entry in the overview (0 for servergroup, 1 for client permissions, 2 for needed channel permissions, 3 for channelgroup)
        @type overviewType: int
        @param overviewID1: depending on the overviewType, set to the id of the servergroup, to the client's database id or the id of the channel
        @type overviewID1: int
        @param overviewID2: only used with overviewType=3, then set to the id of the channelgroup; otherwise set to 0
        @type overviewID2: int
        @param permissionID: the id of the permission
        @type permissionID: int
        @param permissionValue: the value of the permission
        @type permissionValue: int
        @param permissionNegated: negated flag of the permission
        @type permissionNegated: int
        @param permissionSkip: skip flag of the permission
        @type permissionSkip: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKiKKIiii)", "onPermissionOverviewEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) clientDatabaseID, (unsigned long long) channelID,  overviewType, (unsigned long long) overviewID1, (unsigned long long) overviewID2,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onPermissionOverviewEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPermissionOverviewFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onPermissionOverviewFinishedEvent(self, serverConnectionHandlerID):
        """
        This is called after each permission yielded by onPermissionOverviewEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        """

  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onPermissionOverviewFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onPermissionOverviewFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPlaybackShutdownCompleteEvent(uint64 serverConnectionHandlerID) {
  /*
    def onPlaybackShutdownCompleteEvent(self, serverConnectionHandlerID):
        """
        This is called when a playback device can be shutdown with ts3lib.closePlaybackDevice after the process was initiated with ts3lib.initiateGracefulPlaybackShutdown.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onPlaybackShutdownCompleteEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onPlaybackShutdownCompleteEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPluginCommandEvent(uint64 serverConnectionHandlerID, const char *pluginName, const char *pluginCommand) {
  /*
    def onPluginCommandEvent(self, serverConnectionHandlerID, sender, pluginCommand):
        """
        This is called whenever pyTSon receives a plugincommand from another client. All pyTSon plugins will receive this callback. pyTSon recommends to prefix plugincommands with the pluginname.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param sender: the id of the sending client
        @type sender: int
        @param pluginCommand: the command
        @type pluginCommand: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKss)", "onPluginCommandEvent", (unsigned long long) serverConnectionHandlerID,  pluginName,  pluginCommand)) {
    LOG_ADD(QSTR("Calling onPluginCommandEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerConnectionInfoEvent(uint64 serverConnectionHandlerID) {
  /*
    def onServerConnectionInfoEvent(self, serverConnectionHandlerID):
        """
        This is called whenever the server's connectioninfo was updated.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onServerConnectionInfoEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onServerConnectionInfoEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerEditedEvent(uint64 serverConnectionHandlerID, anyID editerID, const char *editerName, const char *editerUniqueIdentifier) {
  /*
    def onServerEditedEvent(self, serverConnectionHandlerID, editerID, editerName, editerUniqueIdentifier):
        """
        This is called whenever the server was edited by a client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param editerID: the id of the client
        @type editerID: int
        @param editerName: nick of the client
        @type editerName: int
        @param editerUniqueIdentifier: uid of the client
        @type editerUniqueIdentifier: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIss)", "onServerEditedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) editerID,  editerName,  editerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onServerEditedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupByClientIDEvent(uint64 serverConnectionHandlerID, const char *name, uint64 serverGroupList, uint64 clientDatabaseID) {
  /*
    def onServerGroupByClientIDEvent(self, serverConnectionHandlerID, name, serverGroupList, clientDatabaseID):
        """
        This is called for each servergroup of a client requested with ts3lib.requestServerGroupsByClientID.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param name: name of the servergroup
        @type name:  str
        @param serverGroupList: id of the servergroup
        @type serverGroupList: int
        @param clientDatabaseID: the database id of the client
        @type clientDatabaseID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKsKK)", "onServerGroupByClientIDEvent", (unsigned long long) serverConnectionHandlerID,  name, (unsigned long long) serverGroupList, (unsigned long long) clientDatabaseID)) {
    LOG_ADD(QSTR("Calling onServerGroupByClientIDEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupClientAddedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientName, const char *clientUniqueIdentity, uint64 serverGroupID, anyID invokerClientID, const char *invokerName, const char *invokerUniqueIdentity) {
  /*
    def onServerGroupClientAddedEvent(self, serverConnectionHandlerID, clientID, clientName, clientUniqueIdentity, serverGroupID, invokerClientID, invokerName, invokerUniqueIdentity):
        """
        This is called whenever a client is added to a servergroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the added client
        @type clientID: int
        @param clientName: nick of the added client
        @type clientName: str
        @param clientUniqueIdentity: uid of the added client
        @type clientUniqueIdentity: str
        @param serverGroupID: the id of the servergroup
        @type serverGroupID: int
        @param invokerClientID: the id of the adding client
        @type invokerClientID: int
        @param invokerName: nick of the adding client
        @type invokerName: str
        @param invokerUniqueIdentity: uid of the adding client
        @type invokerUniqueIdentity: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIssKIss)", "onServerGroupClientAddedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID,  clientName,  clientUniqueIdentity, (unsigned long long) serverGroupID, (unsigned int) invokerClientID,  invokerName,  invokerUniqueIdentity)) {
    LOG_ADD(QSTR("Calling onServerGroupClientAddedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupClientDeletedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientName, const char *clientUniqueIdentity, uint64 serverGroupID, anyID invokerClientID, const char *invokerName, const char *invokerUniqueIdentity) {
  /*
    def onServerGroupClientDeletedEvent(self, serverConnectionHandlerID, clientID, clientName, clientUniqueIdentity, serverGroupID, invokerClientID, invokerName, invokerUniqueIdentity):
        """
        This is called whenever a client was removed from a servergroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the removed client
        @type clientID: int
        @param clientName: nick of the removed client
        @type clientName: str
        @param clientUniqueIdentity: uid of the removed client
        @type clientUniqueIdentity: str
        @param serverGroupID: id the servergroup
        @type serverGroupID: int
        @param invokerClientID: the id of the removing client
        @type invokerClientID: int
        @param invokerName: nick of the removing client
        @type invokerName: str
        @param invokerUniqueIdentity: uid of the removing client
        @type invokerUniqueIdentity: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIssKIss)", "onServerGroupClientDeletedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID,  clientName,  clientUniqueIdentity, (unsigned long long) serverGroupID, (unsigned int) invokerClientID,  invokerName,  invokerUniqueIdentity)) {
    LOG_ADD(QSTR("Calling onServerGroupClientDeletedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupClientListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, uint64 clientDatabaseID, const char *clientNameIdentifier, const char *clientUniqueID) {
  /*
    def onServerGroupClientListEvent(self, serverConnectionHandlerID, serverGroupID, clientDatabaseID, clientNameIdentifier, clientUniqueID):
        """
        This is called for each member of a servergroup requested with ts3lib.requestServerGroupClientList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverGroupID: the id of the servergroup
        @type serverGroupID: int
        @param clientDatabaseID: the database id of the member
        @type clientDatabaseID: int
        @param clientNameIdentifier: the last nick of the member or an empty string if withNames was set to False in the request
        @type clientNameIdentifier: str
        @param clientUniqueID: the uid of the member or an empty string if withNames was set to False in the request
        @type clientUniqueID: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKss)", "onServerGroupClientListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) serverGroupID, (unsigned long long) clientDatabaseID,  clientNameIdentifier,  clientUniqueID)) {
    LOG_ADD(QSTR("Calling onServerGroupClientListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char *name, int type, int iconID, int saveDB) {
  /*
    def onServerGroupListEvent(self, serverConnectionHandlerID, serverGroupID, name, atype, iconID, saveDB):
        """
        This is called for each servergroup on the server requested with ts3lib.requestServerGroupList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverGroupID: the id of the servergroup
        @type serverGroupID: int
        @param name: name of the servergroup
        @type name: str
        @param atype: type of the servergroup (0=template, 1=regular, 2=serverquery)
        @type atype: int
        @param iconID: icon id of the servergroup or 0 if no icon in this group
        @type iconID: int
        @param saveDB: set to 1 if memberships are saved to the database, set to 0 otherwise
        @type saveDB: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsiii)", "onServerGroupListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) serverGroupID,  name,  type,  iconID,  saveDB)) {
    LOG_ADD(QSTR("Calling onServerGroupListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupListFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onServerGroupListFinishedEvent(self, serverConnectionHandlerID):
        """
        This is called after each servergroup yielded by onServerGroupListEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onServerGroupListFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onServerGroupListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupPermListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onServerGroupPermListEvent(self, serverConnectionHandlerID, serverGroupID, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        This is called for each granted permission of a servergroup requested with ts3lib.requestServerGroupPermList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverGroupID: the id of the servergroup
        @type serverGroupID: int
        @param permissionID: the id of the permission
        @type permissionID: int
        @param permissionValue: value of the permission
        @type permissionValue: int
        @param permissionNegated: negated flag
        @type permissionNegated: int
        @param permissionSkip: skip flag
        @type permissionSkip: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIiii)", "onServerGroupPermListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) serverGroupID,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onServerGroupPermListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID) {
  /*
    def onServerGroupPermListFinishedEvent(self, serverConnectionHandlerID, serverGroupID):
        """
        This is called after each permission yielded by onServerGroupPermListEvent was triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverGroupID: id of the servergroup
        @type serverGroupID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onServerGroupPermListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) serverGroupID)) {
    LOG_ADD(QSTR("Calling onServerGroupPermListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerLogEvent(uint64 serverConnectionHandlerID, const char *logMsg) {
  /*
    def onServerLogEvent(self, serverConnectionHandlerID, logMsg):
        """
        This is called for each line of the serverlog requested by the TS3 Client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param logMsg: the message
        @type logMsg: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKs)", "onServerLogEvent", (unsigned long long) serverConnectionHandlerID,  logMsg)) {
    LOG_ADD(QSTR("Calling onServerLogEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerLogFinishedEvent(uint64 serverConnectionHandlerID, uint64 lastPos, uint64 fileSize) {
  /*
    def onServerLogFinishedEvent(self, serverConnectionHandlerID, lastPos, fileSize):
        """
        This is called after the requested number of loglines were yielded by onServerLogEvent.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param lastPos:
        @type lastPos:
        @param fileSize:
        @type fileSize:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKK)", "onServerLogFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) lastPos, (unsigned long long) fileSize)) {
    LOG_ADD(QSTR("Calling onServerLogFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerStopEvent(uint64 serverConnectionHandlerID, const char *shutdownMessage) {
  /*
    def onServerStopEvent(self, serverConnectionHandlerID, shutdownMessage):
        """
        This is called when the server was stopped.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param shutdownMessage: if given, the shutdownmessage
        @type shutdownMessage: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKs)", "onServerStopEvent", (unsigned long long) serverConnectionHandlerID,  shutdownMessage)) {
    LOG_ADD(QSTR("Calling onServerStopEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerTemporaryPasswordListEvent(uint64 serverConnectionHandlerID, const char *clientNickname, const char *uniqueClientIdentifier, const char *description, const char *password, uint64 timestampStart, uint64 timestampEnd, uint64 targetChannelID, const char *targetChannelPW) {
  /*
    def onServerTemporaryPasswordListEvent(self, serverConnectionHandlerID, clientNickname, uniqueClientIdentifier, description, password, timestampStart, timestampEnd, targetChannelID, targetChannelPW):
        """
        This is called for each temporary password on the server requested with ts3lib.requestServerTemporaryPasswordList.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientNickname: nick of the creator
        @type clientNickname: str
        @param uniqueClientIdentifier: uid of the creator
        @type uniqueClientIdentifier: str
        @param description: description of the password
        @type description: str
        @param password: the password
        @type password: str
        @param timestampStart: time the password was created as unix timestamp
        @type timestampStart: int
        @param timestampEnd: time the password expires as unix timestamp
        @type timestampEnd: int
        @param targetChannelID: the id of the channel clients join in
        @type targetChannelID: int
        @param targetChannelPW: password to the targetChannel
        @type targetChannelPW: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKssssKKKs)", "onServerTemporaryPasswordListEvent", (unsigned long long) serverConnectionHandlerID,  clientNickname,  uniqueClientIdentifier,  description,  password, (unsigned long long) timestampStart, (unsigned long long) timestampEnd, (unsigned long long) targetChannelID,  targetChannelPW)) {
    LOG_ADD(QSTR("Calling onServerTemporaryPasswordListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerUpdatedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onServerUpdatedEvent(self, serverConnectionHandlerID):
        """
        This is called whenever the server variables were updated.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onServerUpdatedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onServerUpdatedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onSoundDeviceListChangedEvent(const char *modeID, int playOrCap) {
  /*
    def onSoundDeviceListChangedEvent(self, modeID, playOrCap):
        """
        This is called when the list of sounddevices changed.
        @param modeID: defines the playback/capture mode
        @type modeID: int
        @param playOrCap: defines whether the playback- or capturelist changed
        @type playOrCap: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(ssi)", "onSoundDeviceListChangedEvent",  modeID,  playOrCap)) {
    LOG_ADD(QSTR("Calling onSoundDeviceListChangedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR);
  }
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
  /*
    def onTalkStatusChangeEvent(self, serverConnectionHandlerID, status, isReceivedWhisper, clientID):
        """
        This is called whenever a client starts or stops talking.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param status: defines whether the client starts or stops talking (see ts3defines.TalkStatus)
        @type status: int
        @param isReceivedWhisper: set to 1 if the client whispered, set to 0 otherwise
        @type isReceivedWhisper: int
        @param clientID: the id of the client
        @type clientID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKiiI)", "onTalkStatusChangeEvent", (unsigned long long) serverConnectionHandlerID,  status,  isReceivedWhisper, (unsigned int) clientID)) {
    LOG_ADD(QSTR("Calling onTalkStatusChangeEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onUpdateChannelEditedEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier) {
  /*
    def onUpdateChannelEditedEvent(self, serverConnectionHandlerID, channelID, invokerID, invokerName, invokerUniqueIdentifier):
        """
        This is called whenever a channel was edited by a client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the id of the channel
        @type channelID: int
        @param invokerID: the id of the client
        @type invokerID: int
        @param invokerName: nick of the client
        @type invokerName: str
        @param invokerUniqueIdentifier: uid of the client
        @type invokerUniqueIdentifier: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIss)", "onUpdateChannelEditedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned int) invokerID,  invokerName,  invokerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onUpdateChannelEditedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onUpdateChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onUpdateChannelEvent(self, serverConnectionHandlerID, channelID):
        """
        This is called whenever the channel variables of a specific channel are updated.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the id of the channel
        @type channelID: int
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onUpdateChannelEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onUpdateChannelEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onUpdateClientEvent(uint64 serverConnectionHandlerID, anyID clientID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier) {
  /*
    def onUpdateClientEvent(self, serverConnectionHandlerID, clientID, invokerID, invokerName, invokerUniqueIdentifier):
        """
        This is called whenever the client variables of a specific client are updated.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the id of the client
        @type clientID: int
        @param invokerID: id of the client invoking the change or 0 if it was a selfupdate
        @type invokerID: int
        @param invokerName: nick of the invoking client
        @type invokerName: str
        @param invokerUniqueIdentifier: uid of the invoking client
        @type invokerUniqueIdentifier: str
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIIss)", "onUpdateClientEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned int) invokerID,  invokerName,  invokerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onUpdateClientEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

