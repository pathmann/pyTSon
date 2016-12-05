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
        
    def onEditPlaybackVoiceDataEvent(self, schid, clientID, samples, channels):
        """
        
        @param schid:
        @type schid:
        @param samples:
        @type samples:
        @param channels:
        @type channels:
        @return: 
        @rtype: tuple(bool, list(int))
        """
        
    def onEditPostProcessVoiceDataEvent(schid, clientID, samples, channels, channelSpeakerArray, channelFillMask):
        """
        
        @param schid:
        @type schid:
        @param clientID:
        @type clientID:
        @param samples:
        @type samples:
        @param channels:
        @type channels:
        @param channelSpeakerArray:
        @type channelSpeakerArray:
        @param channelFillMask:
        @type channelFillMask:
        @return:
        @rtype: tuple(bool, list(int), int)
        """
        
    def onEditMixedPlaybackVoiceDataEvent(schid, samples, channels, channelSpeakerArray, channelFillMask):
        """
        
        @param schid:
        @type schid:
        @param samples:
        @type samples:
        @param channels:
        @type channels:
        @param channelSpeakerArray:
        @type channelSpeakerArray:
        @param channelFillMask:
        @type channelFillMask:
        @return:
        @rtype: tuple(bool, list(int), int)
        """
        
    def onEditCapturedVoiceDataEvent(schid, samples, channels, edited):
        """
        
        @param schid:
        @type schid:
        @param samples:
        @type samples:
        @param channels:
        @type channels:
        @param edited:
        @type edited:
        @return:
        @rtype: tuple(bool, list(int), int)
        """
        
    def onCustom3dRolloffCalculationClientEvent(schid, clientID, distance, volume):
        """
        
        @param schid:
        @type schid:
        @param clientID:
        @type clientID:
        @param distance:
        @type distance:
        @param volume:
        @type volume:
        @return:
        @rtype: float
        """
        
    def onCustom3dRolloffCalculationWaveEvent(schid, waveHandle, distance, volume):
        """
        
        @param schid:
        @type schid:
        @param waveHandle:
        @type waveHandle:
        @param distance:
        @type distance:
        @param volume:
        @type volume:
        @return:
        @rtype: float
        """
*/

void ts3plugin_onServerStopEvent(uint64 serverConnectionHandlerID, const char *shutdownMessage) {
  /*
    def onServerStopEvent(self, serverConnectionHandlerID, shutdownMessage):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param shutdownMessage:
        @type shutdownMessage:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKs)", "onServerStopEvent", (unsigned long long) serverConnectionHandlerID,  shutdownMessage)) {
    LOG_ADD(QSTR("Calling onServerStopEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientDBIDfromUIDEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, uint64 clientDatabaseID) {
  /*
    def onClientDBIDfromUIDEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientDatabaseID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param uniqueClientIdentifier:
        @type uniqueClientIdentifier:
        @param clientDatabaseID:
        @type clientDatabaseID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKsK)", "onClientDBIDfromUIDEvent", (unsigned long long) serverConnectionHandlerID,  uniqueClientIdentifier, (unsigned long long) clientDatabaseID)) {
    LOG_ADD(QSTR("Calling onClientDBIDfromUIDEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onBanListEvent(uint64 serverConnectionHandlerID, uint64 banid, const char *ip, const char *name, const char *uid, uint64 creationTime, uint64 durationTime, const char *invokerName, uint64 invokercldbid, const char *invokeruid, const char *reason, int numberOfEnforcements, const char *lastNickName) {
  /*
    def onBanListEvent(self, serverConnectionHandlerID, banid, ip, name, uid, creationTime, durationTime, invokerName, invokercldbid, invokeruid, reason, numberOfEnforcements, lastNickName):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param banid:
        @type banid:
        @param ip:
        @type ip:
        @param name:
        @type name:
        @param uid:
        @type uid:
        @param creationTime:
        @type creationTime:
        @param durationTime:
        @type durationTime:
        @param invokerName:
        @type invokerName:
        @param invokercldbid:
        @type invokercldbid:
        @param invokeruid:
        @type invokeruid:
        @param reason:
        @type reason:
        @param numberOfEnforcements:
        @type numberOfEnforcements:
        @param lastNickName:
        @type lastNickName:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsssKKsKssis)", "onBanListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) banid,  ip,  name,  uid, (unsigned long long) creationTime, (unsigned long long) durationTime,  invokerName, (unsigned long long) invokercldbid,  invokeruid,  reason,  numberOfEnforcements,  lastNickName)) {
    LOG_ADD(QSTR("Calling onBanListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_currentServerConnectionChanged(uint64 serverConnectionHandlerID) {
  /*
    def currentServerConnectionChanged(self, serverConnectionHandlerID):
        """
        
        @param serverConnectionHandlerID: id of the new serverconnectionhandler
        @type serverConnectionHandlerID: long
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "currentServerConnectionChanged", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling currentServerConnectionChanged failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerConnectionInfoEvent(uint64 serverConnectionHandlerID) {
  /*
    def onServerConnectionInfoEvent(self, serverConnectionHandlerID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onServerConnectionInfoEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onServerConnectionInfoEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPlaybackShutdownCompleteEvent(uint64 serverConnectionHandlerID) {
  /*
    def onPlaybackShutdownCompleteEvent(self, serverConnectionHandlerID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onPlaybackShutdownCompleteEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onPlaybackShutdownCompleteEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onDelChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier) {
  /*
    def onDelChannelEvent(self, serverConnectionHandlerID, channelID, invokerID, invokerName, invokerUniqueIdentifier):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        @param invokerID:
        @type invokerID:
        @param invokerName:
        @type invokerName:
        @param invokerUniqueIdentifier:
        @type invokerUniqueIdentifier:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIss)", "onDelChannelEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned int) invokerID,  invokerName,  invokerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onDelChannelEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char *name, int type, int iconID, int saveDB) {
  /*
    def onServerGroupListEvent(self, serverConnectionHandlerID, serverGroupID, name, atype, iconID, saveDB):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param serverGroupID:
        @type serverGroupID:
        @param name:
        @type name:
        @param atype:
        @type atype:
        @param iconID:
        @type iconID:
        @param saveDB:
        @type saveDB:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsiii)", "onServerGroupListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) serverGroupID,  name,  type,  iconID,  saveDB)) {
    LOG_ADD(QSTR("Calling onServerGroupListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientChatClosedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientUniqueIdentity) {
  /*
    def onClientChatClosedEvent(self, serverConnectionHandlerID, clientID, clientUniqueIdentity):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param clientUniqueIdentity:
        @type clientUniqueIdentity:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIs)", "onClientChatClosedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID,  clientUniqueIdentity)) {
    LOG_ADD(QSTR("Calling onClientChatClosedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientNeededPermissionsEvent(uint64 serverConnectionHandlerID, unsigned int permissionID, int permissionValue) {
  /*
    def onClientNeededPermissionsEvent(self, serverConnectionHandlerID, permissionID, permissionValue):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param permissionID:
        @type permissionID:
        @param permissionValue:
        @type permissionValue:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIi)", "onClientNeededPermissionsEvent", (unsigned long long) serverConnectionHandlerID,  permissionID,  permissionValue)) {
    LOG_ADD(QSTR("Calling onClientNeededPermissionsEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupClientDeletedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientName, const char *clientUniqueIdentity, uint64 serverGroupID, anyID invokerClientID, const char *invokerName, const char *invokerUniqueIdentity) {
  /*
    def onServerGroupClientDeletedEvent(self, serverConnectionHandlerID, clientID, clientName, clientUniqueIdentity, serverGroupID, invokerClientID, invokerName, invokerUniqueIdentity):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param clientName:
        @type clientName:
        @param clientUniqueIdentity:
        @type clientUniqueIdentity:
        @param serverGroupID:
        @type serverGroupID:
        @param invokerClientID:
        @type invokerClientID:
        @param invokerName:
        @type invokerName:
        @param invokerUniqueIdentity:
        @type invokerUniqueIdentity:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIssKIss)", "onServerGroupClientDeletedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID,  clientName,  clientUniqueIdentity, (unsigned long long) serverGroupID, (unsigned int) invokerClientID,  invokerName,  invokerUniqueIdentity)) {
    LOG_ADD(QSTR("Calling onServerGroupClientDeletedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char *oldValue, const char *newValue) {
  /*
    def onClientSelfVariableUpdateEvent(self, serverConnectionHandlerID, flag, oldValue, newValue):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param flag:
        @type flag:
        @param oldValue:
        @type oldValue:
        @param newValue:
        @type newValue:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKiss)", "onClientSelfVariableUpdateEvent", (unsigned long long) serverConnectionHandlerID,  flag,  oldValue,  newValue)) {
    LOG_ADD(QSTR("Calling onClientSelfVariableUpdateEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility) {
  /*
    def onClientMoveSubscriptionEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param oldChannelID:
        @type oldChannelID:
        @param newChannelID:
        @type newChannelID:
        @param visibility:
        @type visibility:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKi)", "onClientMoveSubscriptionEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility)) {
    LOG_ADD(QSTR("Calling onClientMoveSubscriptionEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPermissionListGroupEndIDEvent(uint64 serverConnectionHandlerID, unsigned int groupEndID) {
  /*
    def onPermissionListGroupEndIDEvent(self, serverConnectionHandlerID, groupEndID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param groupEndID:
        @type groupEndID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKI)", "onPermissionListGroupEndIDEvent", (unsigned long long) serverConnectionHandlerID,  groupEndID)) {
    LOG_ADD(QSTR("Calling onPermissionListGroupEndIDEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelGroupPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID) {
  /*
    def onChannelGroupPermListFinishedEvent(self, serverConnectionHandlerID, channelGroupID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelGroupID:
        @type channelGroupID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelGroupPermListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelGroupID)) {
    LOG_ADD(QSTR("Calling onChannelGroupPermListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelSubscribeFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onChannelSubscribeFinishedEvent(self, serverConnectionHandlerID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onChannelSubscribeFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onChannelSubscribeFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerTemporaryPasswordListEvent(uint64 serverConnectionHandlerID, const char *clientNickname, const char *uniqueClientIdentifier, const char *description, const char *password, uint64 timestampStart, uint64 timestampEnd, uint64 targetChannelID, const char *targetChannelPW) {
  /*
    def onServerTemporaryPasswordListEvent(self, serverConnectionHandlerID, clientNickname, uniqueClientIdentifier, description, password, timestampStart, timestampEnd, targetChannelID, targetChannelPW):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientNickname:
        @type clientNickname:
        @param uniqueClientIdentifier:
        @type uniqueClientIdentifier:
        @param description:
        @type description:
        @param password:
        @type password:
        @param timestampStart:
        @type timestampStart:
        @param timestampEnd:
        @type timestampEnd:
        @param targetChannelID:
        @type targetChannelID:
        @param targetChannelPW:
        @type targetChannelPW:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKssssKKKs)", "onServerTemporaryPasswordListEvent", (unsigned long long) serverConnectionHandlerID,  clientNickname,  uniqueClientIdentifier,  description,  password, (unsigned long long) timestampStart, (unsigned long long) timestampEnd, (unsigned long long) targetChannelID,  targetChannelPW)) {
    LOG_ADD(QSTR("Calling onServerTemporaryPasswordListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientNeededPermissionsFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onClientNeededPermissionsFinishedEvent(self, serverConnectionHandlerID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onClientNeededPermissionsFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onClientNeededPermissionsFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupClientAddedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientName, const char *clientUniqueIdentity, uint64 serverGroupID, anyID invokerClientID, const char *invokerName, const char *invokerUniqueIdentity) {
  /*
    def onServerGroupClientAddedEvent(self, serverConnectionHandlerID, clientID, clientName, clientUniqueIdentity, serverGroupID, invokerClientID, invokerName, invokerUniqueIdentity):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param clientName:
        @type clientName:
        @param clientUniqueIdentity:
        @type clientUniqueIdentity:
        @param serverGroupID:
        @type serverGroupID:
        @param invokerClientID:
        @type invokerClientID:
        @param invokerName:
        @type invokerName:
        @param invokerUniqueIdentity:
        @type invokerUniqueIdentity:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIssKIss)", "onServerGroupClientAddedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID,  clientName,  clientUniqueIdentity, (unsigned long long) serverGroupID, (unsigned int) invokerClientID,  invokerName,  invokerUniqueIdentity)) {
    LOG_ADD(QSTR("Calling onServerGroupClientAddedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientIDsEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, anyID clientID, const char *clientName) {
  /*
    def onClientIDsEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientID, clientName):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param uniqueClientIdentifier:
        @type uniqueClientIdentifier:
        @param clientID:
        @type clientID:
        @param clientName:
        @type clientName:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKsIs)", "onClientIDsEvent", (unsigned long long) serverConnectionHandlerID,  uniqueClientIdentifier, (unsigned int) clientID,  clientName)) {
    LOG_ADD(QSTR("Calling onClientIDsEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientMoveMovedEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char *moverName, const char *moverUniqueIdentifier, const char *moveMessage) {
  /*
    def onClientMoveMovedEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, moverID, moverName, moverUniqueIdentifier, moveMessage):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param oldChannelID:
        @type oldChannelID:
        @param newChannelID:
        @type newChannelID:
        @param visibility:
        @type visibility:
        @param moverID:
        @type moverID:
        @param moverName:
        @type moverName:
        @param moverUniqueIdentifier:
        @type moverUniqueIdentifier:
        @param moveMessage:
        @type moveMessage:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKiIsss)", "onClientMoveMovedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility, (unsigned int) moverID,  moverName,  moverUniqueIdentifier,  moveMessage)) {
    LOG_ADD(QSTR("Calling onClientMoveMovedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelGroupPermListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onChannelGroupPermListEvent(self, serverConnectionHandlerID, channelGroupID, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelGroupID:
        @type channelGroupID:
        @param permissionID:
        @type permissionID:
        @param permissionValue:
        @type permissionValue:
        @param permissionNegated:
        @type permissionNegated:
        @param permissionSkip:
        @type permissionSkip:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIiii)", "onChannelGroupPermListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelGroupID,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onChannelGroupPermListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onUpdateChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onUpdateChannelEvent(self, serverConnectionHandlerID, channelID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onUpdateChannelEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onUpdateChannelEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientBanFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char *kickerName, const char *kickerUniqueIdentifier, uint64 time, const char *kickMessage) {
  /*
    def onClientBanFromServerEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, kickerID, kickerName, kickerUniqueIdentifier, time, kickMessage):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param oldChannelID:
        @type oldChannelID:
        @param newChannelID:
        @type newChannelID:
        @param visibility:
        @type visibility:
        @param kickerID:
        @type kickerID:
        @param kickerName:
        @type kickerName:
        @param kickerUniqueIdentifier:
        @type kickerUniqueIdentifier:
        @param time:
        @type time:
        @param kickMessage:
        @type kickMessage:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKiIssKs)", "onClientBanFromServerEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility, (unsigned int) kickerID,  kickerName,  kickerUniqueIdentifier, (unsigned long long) time,  kickMessage)) {
    LOG_ADD(QSTR("Calling onClientBanFromServerEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onUpdateClientEvent(uint64 serverConnectionHandlerID, anyID clientID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier) {
  /*
    def onUpdateClientEvent(self, serverConnectionHandlerID, clientID, invokerID, invokerName, invokerUniqueIdentifier):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param invokerID:
        @type invokerID:
        @param invokerName:
        @type invokerName:
        @param invokerUniqueIdentifier:
        @type invokerUniqueIdentifier:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIIss)", "onUpdateClientEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned int) invokerID,  invokerName,  invokerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onUpdateClientEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onConnectionInfoEvent(uint64 serverConnectionHandlerID, anyID clientID) {
  /*
    def onConnectionInfoEvent(self, serverConnectionHandlerID, clientID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKI)", "onConnectionInfoEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID)) {
    LOG_ADD(QSTR("Calling onConnectionInfoEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onChannelPermListFinishedEvent(self, serverConnectionHandlerID, channelID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelPermListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onChannelPermListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onAvatarUpdated(uint64 serverConnectionHandlerID, anyID clientID, const char *avatarPath) {
  /*
    def onAvatarUpdated(self, serverConnectionHandlerID, clientID, avatarPath):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param avatarPath:
        @type avatarPath:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIs)", "onAvatarUpdated", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID,  avatarPath)) {
    LOG_ADD(QSTR("Calling onAvatarUpdated failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientKickFromChannelEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char *kickerName, const char *kickerUniqueIdentifier, const char *kickMessage) {
  /*
    def onClientKickFromChannelEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, kickerID, kickerName, kickerUniqueIdentifier, kickMessage):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param oldChannelID:
        @type oldChannelID:
        @param newChannelID:
        @type newChannelID:
        @param visibility:
        @type visibility:
        @param kickerID:
        @type kickerID:
        @param kickerName:
        @type kickerName:
        @param kickerUniqueIdentifier:
        @type kickerUniqueIdentifier:
        @param kickMessage:
        @type kickMessage:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKiIsss)", "onClientKickFromChannelEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility, (unsigned int) kickerID,  kickerName,  kickerUniqueIdentifier,  kickMessage)) {
    LOG_ADD(QSTR("Calling onClientKickFromChannelEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onHotkeyRecordedEvent(const char *keyword, const char *key) {
  /*
    def onHotkeyRecordedEvent(self, keyword, key):
        """
        
        @param keyword:
        @type keyword:
        @param key:
        @type key:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sss)", "onHotkeyRecordedEvent",  keyword,  key)) {
    LOG_ADD(QSTR("Calling onHotkeyRecordedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR);
  }
}

void ts3plugin_onFileListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char *path) {
  /*
    def onFileListFinishedEvent(self, serverConnectionHandlerID, channelID, path):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        @param path:
        @type path:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKs)", "onFileListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID,  path)) {
    LOG_ADD(QSTR("Calling onFileListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onMessageGetEvent(uint64 serverConnectionHandlerID, uint64 messageID, const char *fromClientUniqueIdentity, const char *subject, const char *message, uint64 timestamp) {
  /*
    def onMessageGetEvent(self, serverConnectionHandlerID, messageID, fromClientUniqueIdentity, subject, message, timestamp):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param messageID:
        @type messageID:
        @param fromClientUniqueIdentity:
        @type fromClientUniqueIdentity:
        @param subject:
        @type subject:
        @param message:
        @type message:
        @param timestamp:
        @type timestamp:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsssK)", "onMessageGetEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) messageID,  fromClientUniqueIdentity,  subject,  message, (unsigned long long) timestamp)) {
    LOG_ADD(QSTR("Calling onMessageGetEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelClientPermListEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 clientDatabaseID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onChannelClientPermListEvent(self, serverConnectionHandlerID, channelID, clientDatabaseID, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        @param clientDatabaseID:
        @type clientDatabaseID:
        @param permissionID:
        @type permissionID:
        @param permissionValue:
        @type permissionValue:
        @param permissionNegated:
        @type permissionNegated:
        @param permissionSkip:
        @type permissionSkip:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKIiii)", "onChannelClientPermListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned long long) clientDatabaseID,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onChannelClientPermListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber) {
  /*
    def onConnectStatusChangeEvent(self, serverConnectionHandlerID, newStatus, errorNumber):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param newStatus:
        @type newStatus:
        @param errorNumber:
        @type errorNumber:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKiI)", "onConnectStatusChangeEvent", (unsigned long long) serverConnectionHandlerID,  newStatus,  errorNumber)) {
    LOG_ADD(QSTR("Calling onConnectStatusChangeEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onNewChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID) {
  /*
    def onNewChannelEvent(self, serverConnectionHandlerID, channelID, channelParentID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        @param channelParentID:
        @type channelParentID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKK)", "onNewChannelEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned long long) channelParentID)) {
    LOG_ADD(QSTR("Calling onNewChannelEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onUpdateChannelEditedEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier) {
  /*
    def onUpdateChannelEditedEvent(self, serverConnectionHandlerID, channelID, invokerID, invokerName, invokerUniqueIdentifier):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        @param invokerID:
        @type invokerID:
        @param invokerName:
        @type invokerName:
        @param invokerUniqueIdentifier:
        @type invokerUniqueIdentifier:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIss)", "onUpdateChannelEditedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned int) invokerID,  invokerName,  invokerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onUpdateChannelEditedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelUnsubscribeFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onChannelUnsubscribeFinishedEvent(self, serverConnectionHandlerID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onChannelUnsubscribeFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onChannelUnsubscribeFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientKickFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char *kickerName, const char *kickerUniqueIdentifier, const char *kickMessage) {
  /*
    def onClientKickFromServerEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, kickerID, kickerName, kickerUniqueIdentifier, kickMessage):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param oldChannelID:
        @type oldChannelID:
        @param newChannelID:
        @type newChannelID:
        @param visibility:
        @type visibility:
        @param kickerID:
        @type kickerID:
        @param kickerName:
        @type kickerName:
        @param kickerUniqueIdentifier:
        @type kickerUniqueIdentifier:
        @param kickMessage:
        @type kickMessage:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKiIsss)", "onClientKickFromServerEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility, (unsigned int) kickerID,  kickerName,  kickerUniqueIdentifier,  kickMessage)) {
    LOG_ADD(QSTR("Calling onClientKickFromServerEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelGroupListFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onChannelGroupListFinishedEvent(self, serverConnectionHandlerID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onChannelGroupListFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onChannelGroupListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelClientPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 clientDatabaseID) {
  /*
    def onChannelClientPermListFinishedEvent(self, serverConnectionHandlerID, channelID, clientDatabaseID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        @param clientDatabaseID:
        @type clientDatabaseID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKK)", "onChannelClientPermListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned long long) clientDatabaseID)) {
    LOG_ADD(QSTR("Calling onChannelClientPermListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientDisplayNameChanged(uint64 serverConnectionHandlerID, anyID clientID, const char *displayName, const char *uniqueClientIdentifier) {
  /*
    def onClientDisplayNameChanged(self, serverConnectionHandlerID, clientID, displayName, uniqueClientIdentifier):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param displayName:
        @type displayName:
        @param uniqueClientIdentifier:
        @type uniqueClientIdentifier:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIss)", "onClientDisplayNameChanged", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID,  displayName,  uniqueClientIdentifier)) {
    LOG_ADD(QSTR("Calling onClientDisplayNameChanged failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientServerQueryLoginPasswordEvent(uint64 serverConnectionHandlerID, const char *loginPassword) {
  /*
    def onClientServerQueryLoginPasswordEvent(self, serverConnectionHandlerID, loginPassword):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param loginPassword:
        @type loginPassword:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKs)", "onClientServerQueryLoginPasswordEvent", (unsigned long long) serverConnectionHandlerID,  loginPassword)) {
    LOG_ADD(QSTR("Calling onClientServerQueryLoginPasswordEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientNamefromDBIDEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, uint64 clientDatabaseID, const char *clientNickName) {
  /*
    def onClientNamefromDBIDEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientDatabaseID, clientNickName):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param uniqueClientIdentifier:
        @type uniqueClientIdentifier:
        @param clientDatabaseID:
        @type clientDatabaseID:
        @param clientNickName:
        @type clientNickName:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKsKs)", "onClientNamefromDBIDEvent", (unsigned long long) serverConnectionHandlerID,  uniqueClientIdentifier, (unsigned long long) clientDatabaseID,  clientNickName)) {
    LOG_ADD(QSTR("Calling onClientNamefromDBIDEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientChatComposingEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientUniqueIdentity) {
  /*
    def onClientChatComposingEvent(self, serverConnectionHandlerID, clientID, clientUniqueIdentity):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
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

void ts3plugin_onPluginCommandEvent(uint64 serverConnectionHandlerID, const char *pluginName, const char *pluginCommand) {
  /*
    def onPluginCommandEvent(self, serverConnectionHandlerID, pluginName, pluginCommand):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param pluginName:
        @type pluginName:
        @param pluginCommand:
        @type pluginCommand:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKss)", "onPluginCommandEvent", (unsigned long long) serverConnectionHandlerID,  pluginName,  pluginCommand)) {
    LOG_ADD(QSTR("Calling onPluginCommandEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelPermListEvent(uint64 serverConnectionHandlerID, uint64 channelID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onChannelPermListEvent(self, serverConnectionHandlerID, channelID, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        @param permissionID:
        @type permissionID:
        @param permissionValue:
        @type permissionValue:
        @param permissionNegated:
        @type permissionNegated:
        @param permissionSkip:
        @type permissionSkip:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIiii)", "onChannelPermListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onChannelPermListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char *timeoutMessage) {
  /*
    def onClientMoveTimeoutEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, timeoutMessage):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param oldChannelID:
        @type oldChannelID:
        @param newChannelID:
        @type newChannelID:
        @param visibility:
        @type visibility:
        @param timeoutMessage:
        @type timeoutMessage:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKis)", "onClientMoveTimeoutEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility,  timeoutMessage)) {
    LOG_ADD(QSTR("Calling onClientMoveTimeoutEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelMoveEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier) {
  /*
    def onChannelMoveEvent(self, serverConnectionHandlerID, channelID, newChannelParentID, invokerID, invokerName, invokerUniqueIdentifier):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        @param newChannelParentID:
        @type newChannelParentID:
        @param invokerID:
        @type invokerID:
        @param invokerName:
        @type invokerName:
        @param invokerUniqueIdentifier:
        @type invokerUniqueIdentifier:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKIss)", "onChannelMoveEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned long long) newChannelParentID, (unsigned int) invokerID,  invokerName,  invokerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onChannelMoveEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID) {
  /*
    def onClientPermListFinishedEvent(self, serverConnectionHandlerID, clientDatabaseID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientDatabaseID:
        @type clientDatabaseID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onClientPermListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) clientDatabaseID)) {
    LOG_ADD(QSTR("Calling onClientPermListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupPermListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onServerGroupPermListEvent(self, serverConnectionHandlerID, serverGroupID, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param serverGroupID:
        @type serverGroupID:
        @param permissionID:
        @type permissionID:
        @param permissionValue:
        @type permissionValue:
        @param permissionNegated:
        @type permissionNegated:
        @param permissionSkip:
        @type permissionSkip:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIiii)", "onServerGroupPermListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) serverGroupID,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onServerGroupPermListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupListFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onServerGroupListFinishedEvent(self, serverConnectionHandlerID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onServerGroupListFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onServerGroupListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPermissionOverviewEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, uint64 channelID, int overviewType, uint64 overviewID1, uint64 overviewID2, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onPermissionOverviewEvent(self, serverConnectionHandlerID, clientDatabaseID, channelID, overviewType, overviewID1, overviewID2, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientDatabaseID:
        @type clientDatabaseID:
        @param channelID:
        @type channelID:
        @param overviewType:
        @type overviewType:
        @param overviewID1:
        @type overviewID1:
        @param overviewID2:
        @type overviewID2:
        @param permissionID:
        @type permissionID:
        @param permissionValue:
        @type permissionValue:
        @param permissionNegated:
        @type permissionNegated:
        @param permissionSkip:
        @type permissionSkip:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKiKKIiii)", "onPermissionOverviewEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) clientDatabaseID, (unsigned long long) channelID,  overviewType, (unsigned long long) overviewID1, (unsigned long long) overviewID2,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onPermissionOverviewEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelPasswordChangedEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onChannelPasswordChangedEvent(self, serverConnectionHandlerID, channelID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelPasswordChangedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onChannelPasswordChangedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char *moveMessage) {
  /*
    def onClientMoveEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, moveMessage):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientID:
        @type clientID:
        @param oldChannelID:
        @type oldChannelID:
        @param newChannelID:
        @type newChannelID:
        @param visibility:
        @type visibility:
        @param moveMessage:
        @type moveMessage:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIKKis)", "onClientMoveEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) clientID, (unsigned long long) oldChannelID, (unsigned long long) newChannelID,  visibility,  moveMessage)) {
    LOG_ADD(QSTR("Calling onClientMoveEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPermissionListEvent(uint64 serverConnectionHandlerID, unsigned int permissionID, const char *permissionName, const char *permissionDescription) {
  /*
    def onPermissionListEvent(self, serverConnectionHandlerID, permissionID, permissionName, permissionDescription):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param permissionID:
        @type permissionID:
        @param permissionName:
        @type permissionName:
        @param permissionDescription:
        @type permissionDescription:
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
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onPermissionListFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onPermissionListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientIDsFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onClientIDsFinishedEvent(self, serverConnectionHandlerID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onClientIDsFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onClientIDsFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientNamefromUIDEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, uint64 clientDatabaseID, const char *clientNickName) {
  /*
    def onClientNamefromUIDEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientDatabaseID, clientNickName):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param uniqueClientIdentifier:
        @type uniqueClientIdentifier:
        @param clientDatabaseID:
        @type clientDatabaseID:
        @param clientNickName:
        @type clientNickName:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKsKs)", "onClientNamefromUIDEvent", (unsigned long long) serverConnectionHandlerID,  uniqueClientIdentifier, (unsigned long long) clientDatabaseID,  clientNickName)) {
    LOG_ADD(QSTR("Calling onClientNamefromUIDEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerEditedEvent(uint64 serverConnectionHandlerID, anyID editerID, const char *editerName, const char *editerUniqueIdentifier) {
  /*
    def onServerEditedEvent(self, serverConnectionHandlerID, editerID, editerName, editerUniqueIdentifier):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param editerID:
        @type editerID:
        @param editerName:
        @type editerName:
        @param editerUniqueIdentifier:
        @type editerUniqueIdentifier:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIss)", "onServerEditedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) editerID,  editerName,  editerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onServerEditedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelUnsubscribeEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onChannelUnsubscribeEvent(self, serverConnectionHandlerID, channelID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelUnsubscribeEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onChannelUnsubscribeEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID) {
  /*
    def onTalkStatusChangeEvent(self, serverConnectionHandlerID, status, isReceivedWhisper, clientID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param status:
        @type status:
        @param isReceivedWhisper:
        @type isReceivedWhisper:
        @param clientID:
        @type clientID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKiiI)", "onTalkStatusChangeEvent", (unsigned long long) serverConnectionHandlerID,  status,  isReceivedWhisper, (unsigned int) clientID)) {
    LOG_ADD(QSTR("Calling onTalkStatusChangeEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onHotkeyEvent(const char *keyword) {
  /*
    def onHotkeyEvent(self, keyword):
        """
        
        @param keyword:
        @type keyword:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(ss)", "onHotkeyEvent",  keyword)) {
    LOG_ADD(QSTR("Calling onHotkeyEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR);
  }
}

void ts3plugin_onFileInfoEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char *name, uint64 size, uint64 datetime) {
  /*
    def onFileInfoEvent(self, serverConnectionHandlerID, channelID, name, size, datetime):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        @param name:
        @type name:
        @param size:
        @type size:
        @param datetime:
        @type datetime:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsKK)", "onFileInfoEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID,  name, (unsigned long long) size, (unsigned long long) datetime)) {
    LOG_ADD(QSTR("Calling onFileInfoEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onNewChannelCreatedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier) {
  /*
    def onNewChannelCreatedEvent(self, serverConnectionHandlerID, channelID, channelParentID, invokerID, invokerName, invokerUniqueIdentifier):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        @param channelParentID:
        @type channelParentID:
        @param invokerID:
        @type invokerID:
        @param invokerName:
        @type invokerName:
        @param invokerUniqueIdentifier:
        @type invokerUniqueIdentifier:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKIss)", "onNewChannelCreatedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID, (unsigned long long) channelParentID, (unsigned int) invokerID,  invokerName,  invokerUniqueIdentifier)) {
    LOG_ADD(QSTR("Calling onNewChannelCreatedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientChannelGroupChangedEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, uint64 channelID, anyID clientID, anyID invokerClientID, const char *invokerName, const char *invokerUniqueIdentity) {
  /*
    def onClientChannelGroupChangedEvent(self, serverConnectionHandlerID, channelGroupID, channelID, clientID, invokerClientID, invokerName, invokerUniqueIdentity):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelGroupID:
        @type channelGroupID:
        @param channelID:
        @type channelID:
        @param clientID:
        @type clientID:
        @param invokerClientID:
        @type invokerClientID:
        @param invokerName:
        @type invokerName:
        @param invokerUniqueIdentity:
        @type invokerUniqueIdentity:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKIIss)", "onClientChannelGroupChangedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelGroupID, (unsigned long long) channelID, (unsigned int) clientID, (unsigned int) invokerClientID,  invokerName,  invokerUniqueIdentity)) {
    LOG_ADD(QSTR("Calling onClientChannelGroupChangedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID) {
  /*
    def onMenuItemEvent(self, serverConnectionHandlerID, atype, menuItemID, selectedItemID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param atype:
        @type atype:
        @param menuItemID:
        @type menuItemID:
        @param selectedItemID:
        @type selectedItemID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKIiK)", "onMenuItemEvent", (unsigned long long) serverConnectionHandlerID, (unsigned int) type,  menuItemID, (unsigned long long) selectedItemID)) {
    LOG_ADD(QSTR("Calling onMenuItemEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID) {
  /*
    def onServerGroupPermListFinishedEvent(self, serverConnectionHandlerID, serverGroupID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param serverGroupID:
        @type serverGroupID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onServerGroupPermListFinishedEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) serverGroupID)) {
    LOG_ADD(QSTR("Calling onServerGroupPermListFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onClientPermListEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip) {
  /*
    def onClientPermListEvent(self, serverConnectionHandlerID, clientDatabaseID, permissionID, permissionValue, permissionNegated, permissionSkip):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param clientDatabaseID:
        @type clientDatabaseID:
        @param permissionID:
        @type permissionID:
        @param permissionValue:
        @type permissionValue:
        @param permissionNegated:
        @type permissionNegated:
        @param permissionSkip:
        @type permissionSkip:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKIiii)", "onClientPermListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) clientDatabaseID,  permissionID,  permissionValue,  permissionNegated,  permissionSkip)) {
    LOG_ADD(QSTR("Calling onClientPermListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onMessageListEvent(uint64 serverConnectionHandlerID, uint64 messageID, const char *fromClientUniqueIdentity, const char *subject, uint64 timestamp, int flagRead) {
  /*
    def onMessageListEvent(self, serverConnectionHandlerID, messageID, fromClientUniqueIdentity, subject, timestamp, flagRead):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param messageID:
        @type messageID:
        @param fromClientUniqueIdentity:
        @type fromClientUniqueIdentity:
        @param subject:
        @type subject:
        @param timestamp:
        @type timestamp:
        @param flagRead:
        @type flagRead:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKssKi)", "onMessageListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) messageID,  fromClientUniqueIdentity,  subject, (unsigned long long) timestamp,  flagRead)) {
    LOG_ADD(QSTR("Calling onMessageListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onSoundDeviceListChangedEvent(const char *modeID, int playOrCap) {
  /*
    def onSoundDeviceListChangedEvent(self, modeID, playOrCap):
        """
        
        @param modeID:
        @type modeID:
        @param playOrCap:
        @type playOrCap:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(ssi)", "onSoundDeviceListChangedEvent",  modeID,  playOrCap)) {
    LOG_ADD(QSTR("Calling onSoundDeviceListChangedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR);
  }
}

void ts3plugin_onServerLogFinishedEvent(uint64 serverConnectionHandlerID, uint64 lastPos, uint64 fileSize) {
  /*
    def onServerLogFinishedEvent(self, serverConnectionHandlerID, lastPos, fileSize):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
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

void ts3plugin_onServerGroupByClientIDEvent(uint64 serverConnectionHandlerID, const char *name, uint64 serverGroupList, uint64 clientDatabaseID) {
  /*
    def onServerGroupByClientIDEvent(self, serverConnectionHandlerID, name, serverGroupList, clientDatabaseID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param name:
        @type name:
        @param serverGroupList:
        @type serverGroupList:
        @param clientDatabaseID:
        @type clientDatabaseID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKsKK)", "onServerGroupByClientIDEvent", (unsigned long long) serverConnectionHandlerID,  name, (unsigned long long) serverGroupList, (unsigned long long) clientDatabaseID)) {
    LOG_ADD(QSTR("Calling onServerGroupByClientIDEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onFileListEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char *path, const char *name, uint64 size, uint64 datetime, int type, uint64 incompletesize, const char *returnCode) {
  /*
    def onFileListEvent(self, serverConnectionHandlerID, channelID, path, name, size, datetime, atype, incompletesize, returnCode):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        @param path:
        @type path:
        @param name:
        @type name:
        @param size:
        @type size:
        @param datetime:
        @type datetime:
        @param atype:
        @type atype:
        @param incompletesize:
        @type incompletesize:
        @param returnCode:
        @type returnCode:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKssKKiKs)", "onFileListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID,  path,  name, (unsigned long long) size, (unsigned long long) datetime,  type, (unsigned long long) incompletesize,  returnCode)) {
    LOG_ADD(QSTR("Calling onFileListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onIncomingClientQueryEvent(uint64 serverConnectionHandlerID, const char *commandText) {
  /*
    def onIncomingClientQueryEvent(self, serverConnectionHandlerID, commandText):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param commandText:
        @type commandText:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKs)", "onIncomingClientQueryEvent", (unsigned long long) serverConnectionHandlerID,  commandText)) {
    LOG_ADD(QSTR("Calling onIncomingClientQueryEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerLogEvent(uint64 serverConnectionHandlerID, const char *logMsg) {
  /*
    def onServerLogEvent(self, serverConnectionHandlerID, logMsg):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param logMsg:
        @type logMsg:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKs)", "onServerLogEvent", (unsigned long long) serverConnectionHandlerID,  logMsg)) {
    LOG_ADD(QSTR("Calling onServerLogEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onComplainListEvent(uint64 serverConnectionHandlerID, uint64 targetClientDatabaseID, const char *targetClientNickName, uint64 fromClientDatabaseID, const char *fromClientNickName, const char *complainReason, uint64 timestamp) {
  /*
    def onComplainListEvent(self, serverConnectionHandlerID, targetClientDatabaseID, targetClientNickName, fromClientDatabaseID, fromClientNickName, complainReason, timestamp):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param targetClientDatabaseID:
        @type targetClientDatabaseID:
        @param targetClientNickName:
        @type targetClientNickName:
        @param fromClientDatabaseID:
        @type fromClientDatabaseID:
        @param fromClientNickName:
        @type fromClientNickName:
        @param complainReason:
        @type complainReason:
        @param timestamp:
        @type timestamp:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsKssK)", "onComplainListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) targetClientDatabaseID,  targetClientNickName, (unsigned long long) fromClientDatabaseID,  fromClientNickName,  complainReason, (unsigned long long) timestamp)) {
    LOG_ADD(QSTR("Calling onComplainListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerUpdatedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onServerUpdatedEvent(self, serverConnectionHandlerID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onServerUpdatedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onServerUpdatedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelGroupListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, const char *name, int type, int iconID, int saveDB) {
  /*
    def onChannelGroupListEvent(self, serverConnectionHandlerID, channelGroupID, name, atype, iconID, saveDB):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelGroupID:
        @type channelGroupID:
        @param name:
        @type name:
        @param atype:
        @type atype:
        @param iconID:
        @type iconID:
        @param saveDB:
        @type saveDB:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKsiii)", "onChannelGroupListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelGroupID,  name,  type,  iconID,  saveDB)) {
    LOG_ADD(QSTR("Calling onChannelGroupListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelSubscribeEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onChannelSubscribeEvent(self, serverConnectionHandlerID, channelID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelSubscribeEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onChannelSubscribeEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onChannelDescriptionUpdateEvent(uint64 serverConnectionHandlerID, uint64 channelID) {
  /*
    def onChannelDescriptionUpdateEvent(self, serverConnectionHandlerID, channelID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param channelID:
        @type channelID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKK)", "onChannelDescriptionUpdateEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) channelID)) {
    LOG_ADD(QSTR("Calling onChannelDescriptionUpdateEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onServerGroupClientListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, uint64 clientDatabaseID, const char *clientNameIdentifier, const char *clientUniqueID) {
  /*
    def onServerGroupClientListEvent(self, serverConnectionHandlerID, serverGroupID, clientDatabaseID, clientNameIdentifier, clientUniqueID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        @param serverGroupID:
        @type serverGroupID:
        @param clientDatabaseID:
        @type clientDatabaseID:
        @param clientNameIdentifier:
        @type clientNameIdentifier:
        @param clientUniqueID:
        @type clientUniqueID:
        """
  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sKKKss)", "onServerGroupClientListEvent", (unsigned long long) serverConnectionHandlerID, (unsigned long long) serverGroupID, (unsigned long long) clientDatabaseID,  clientNameIdentifier,  clientUniqueID)) {
    LOG_ADD(QSTR("Calling onServerGroupClientListEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

void ts3plugin_onPermissionOverviewFinishedEvent(uint64 serverConnectionHandlerID) {
  /*
    def onPermissionOverviewFinishedEvent(self, serverConnectionHandlerID):
        """
        
        @param serverConnectionHandlerID:
        @type serverConnectionHandlerID:
        """

  */
  QString callerror;
  if (!PLUGIN_HOST_CALL(NULL, callerror, "(sK)", "onPermissionOverviewFinishedEvent", (unsigned long long) serverConnectionHandlerID)) {
    LOG_ADD(QSTR("Calling onPermissionOverviewFinishedEvent failed with error \"%1\"").arg(callerror), LogLevel_ERROR, serverConnectionHandlerID);
  }
}

