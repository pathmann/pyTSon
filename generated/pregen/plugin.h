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

EXPORT_SYMBOL void ts3plugin_onServerStopEvent(uint64 serverConnectionHandlerID, const char *shutdownMessage);
EXPORT_SYMBOL void ts3plugin_onClientDBIDfromUIDEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, uint64 clientDatabaseID);
EXPORT_SYMBOL void ts3plugin_onBanListEvent(uint64 serverConnectionHandlerID, uint64 banid, const char *ip, const char *name, const char *uid, uint64 creationTime, uint64 durationTime, const char *invokerName, uint64 invokercldbid, const char *invokeruid, const char *reason, int numberOfEnforcements, const char *lastNickName);
EXPORT_SYMBOL void ts3plugin_currentServerConnectionChanged(uint64 serverConnectionHandlerID);
EXPORT_SYMBOL void ts3plugin_onServerConnectionInfoEvent(uint64 serverConnectionHandlerID);
EXPORT_SYMBOL void ts3plugin_onPlaybackShutdownCompleteEvent(uint64 serverConnectionHandlerID);
EXPORT_SYMBOL void ts3plugin_onDelChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier);
EXPORT_SYMBOL void ts3plugin_onServerGroupListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char *name, int type, int iconID, int saveDB);
EXPORT_SYMBOL void ts3plugin_onClientChatClosedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientUniqueIdentity);
EXPORT_SYMBOL void ts3plugin_onClientNeededPermissionsEvent(uint64 serverConnectionHandlerID, unsigned int permissionID, int permissionValue);
EXPORT_SYMBOL void ts3plugin_onServerGroupClientDeletedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientName, const char *clientUniqueIdentity, uint64 serverGroupID, anyID invokerClientID, const char *invokerName, const char *invokerUniqueIdentity);
EXPORT_SYMBOL void ts3plugin_onClientSelfVariableUpdateEvent(uint64 serverConnectionHandlerID, int flag, const char *oldValue, const char *newValue);
EXPORT_SYMBOL void ts3plugin_onClientMoveSubscriptionEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility);
EXPORT_SYMBOL void ts3plugin_onPermissionListGroupEndIDEvent(uint64 serverConnectionHandlerID, unsigned int groupEndID);
EXPORT_SYMBOL void ts3plugin_onChannelGroupPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID);
EXPORT_SYMBOL void ts3plugin_onChannelSubscribeFinishedEvent(uint64 serverConnectionHandlerID);
EXPORT_SYMBOL void ts3plugin_onServerTemporaryPasswordListEvent(uint64 serverConnectionHandlerID, const char *clientNickname, const char *uniqueClientIdentifier, const char *description, const char *password, uint64 timestampStart, uint64 timestampEnd, uint64 targetChannelID, const char *targetChannelPW);
EXPORT_SYMBOL void ts3plugin_onClientNeededPermissionsFinishedEvent(uint64 serverConnectionHandlerID);
EXPORT_SYMBOL void ts3plugin_onServerGroupClientAddedEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientName, const char *clientUniqueIdentity, uint64 serverGroupID, anyID invokerClientID, const char *invokerName, const char *invokerUniqueIdentity);
EXPORT_SYMBOL void ts3plugin_onClientIDsEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, anyID clientID, const char *clientName);
EXPORT_SYMBOL void ts3plugin_onClientMoveMovedEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID moverID, const char *moverName, const char *moverUniqueIdentifier, const char *moveMessage);
EXPORT_SYMBOL void ts3plugin_onChannelGroupPermListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
EXPORT_SYMBOL void ts3plugin_onUpdateChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID);
EXPORT_SYMBOL void ts3plugin_onClientBanFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char *kickerName, const char *kickerUniqueIdentifier, uint64 time, const char *kickMessage);
EXPORT_SYMBOL void ts3plugin_onUpdateClientEvent(uint64 serverConnectionHandlerID, anyID clientID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier);
EXPORT_SYMBOL void ts3plugin_onConnectionInfoEvent(uint64 serverConnectionHandlerID, anyID clientID);
EXPORT_SYMBOL void ts3plugin_onChannelPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID);
EXPORT_SYMBOL void ts3plugin_onAvatarUpdated(uint64 serverConnectionHandlerID, anyID clientID, const char *avatarPath);
EXPORT_SYMBOL void ts3plugin_onClientKickFromChannelEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char *kickerName, const char *kickerUniqueIdentifier, const char *kickMessage);
EXPORT_SYMBOL void ts3plugin_onHotkeyRecordedEvent(const char *keyword, const char *key);
EXPORT_SYMBOL void ts3plugin_onFileListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char *path);
EXPORT_SYMBOL void ts3plugin_onMessageGetEvent(uint64 serverConnectionHandlerID, uint64 messageID, const char *fromClientUniqueIdentity, const char *subject, const char *message, uint64 timestamp);
EXPORT_SYMBOL void ts3plugin_onChannelClientPermListEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 clientDatabaseID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
EXPORT_SYMBOL void ts3plugin_onConnectStatusChangeEvent(uint64 serverConnectionHandlerID, int newStatus, unsigned int errorNumber);
EXPORT_SYMBOL void ts3plugin_onNewChannelEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID);
EXPORT_SYMBOL void ts3plugin_onUpdateChannelEditedEvent(uint64 serverConnectionHandlerID, uint64 channelID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier);
EXPORT_SYMBOL void ts3plugin_onChannelUnsubscribeFinishedEvent(uint64 serverConnectionHandlerID);
EXPORT_SYMBOL void ts3plugin_onClientKickFromServerEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, anyID kickerID, const char *kickerName, const char *kickerUniqueIdentifier, const char *kickMessage);
EXPORT_SYMBOL void ts3plugin_onChannelGroupListFinishedEvent(uint64 serverConnectionHandlerID);
EXPORT_SYMBOL void ts3plugin_onChannelClientPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 clientDatabaseID);
EXPORT_SYMBOL void ts3plugin_onClientDisplayNameChanged(uint64 serverConnectionHandlerID, anyID clientID, const char *displayName, const char *uniqueClientIdentifier);
EXPORT_SYMBOL void ts3plugin_onClientServerQueryLoginPasswordEvent(uint64 serverConnectionHandlerID, const char *loginPassword);
EXPORT_SYMBOL void ts3plugin_onClientNamefromDBIDEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, uint64 clientDatabaseID, const char *clientNickName);
EXPORT_SYMBOL void ts3plugin_onClientChatComposingEvent(uint64 serverConnectionHandlerID, anyID clientID, const char *clientUniqueIdentity);
EXPORT_SYMBOL void ts3plugin_onPluginCommandEvent(uint64 serverConnectionHandlerID, const char *pluginName, const char *pluginCommand);
EXPORT_SYMBOL void ts3plugin_onChannelPermListEvent(uint64 serverConnectionHandlerID, uint64 channelID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
EXPORT_SYMBOL void ts3plugin_onClientMoveTimeoutEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char *timeoutMessage);
EXPORT_SYMBOL void ts3plugin_onChannelMoveEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier);
EXPORT_SYMBOL void ts3plugin_onClientPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID);
EXPORT_SYMBOL void ts3plugin_onServerGroupPermListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
EXPORT_SYMBOL void ts3plugin_onServerGroupListFinishedEvent(uint64 serverConnectionHandlerID);
EXPORT_SYMBOL void ts3plugin_onPermissionOverviewEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, uint64 channelID, int overviewType, uint64 overviewID1, uint64 overviewID2, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
EXPORT_SYMBOL void ts3plugin_onChannelPasswordChangedEvent(uint64 serverConnectionHandlerID, uint64 channelID);
EXPORT_SYMBOL void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char *moveMessage);
EXPORT_SYMBOL void ts3plugin_onPermissionListEvent(uint64 serverConnectionHandlerID, unsigned int permissionID, const char *permissionName, const char *permissionDescription);
EXPORT_SYMBOL void ts3plugin_onPermissionListFinishedEvent(uint64 serverConnectionHandlerID);
EXPORT_SYMBOL void ts3plugin_onClientIDsFinishedEvent(uint64 serverConnectionHandlerID);
EXPORT_SYMBOL void ts3plugin_onClientNamefromUIDEvent(uint64 serverConnectionHandlerID, const char *uniqueClientIdentifier, uint64 clientDatabaseID, const char *clientNickName);
EXPORT_SYMBOL void ts3plugin_onServerEditedEvent(uint64 serverConnectionHandlerID, anyID editerID, const char *editerName, const char *editerUniqueIdentifier);
EXPORT_SYMBOL void ts3plugin_onChannelUnsubscribeEvent(uint64 serverConnectionHandlerID, uint64 channelID);
EXPORT_SYMBOL void ts3plugin_onTalkStatusChangeEvent(uint64 serverConnectionHandlerID, int status, int isReceivedWhisper, anyID clientID);
EXPORT_SYMBOL void ts3plugin_onHotkeyEvent(const char *keyword);
EXPORT_SYMBOL void ts3plugin_onFileInfoEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char *name, uint64 size, uint64 datetime);
EXPORT_SYMBOL void ts3plugin_onNewChannelCreatedEvent(uint64 serverConnectionHandlerID, uint64 channelID, uint64 channelParentID, anyID invokerID, const char *invokerName, const char *invokerUniqueIdentifier);
EXPORT_SYMBOL void ts3plugin_onClientChannelGroupChangedEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, uint64 channelID, anyID clientID, anyID invokerClientID, const char *invokerName, const char *invokerUniqueIdentity);
EXPORT_SYMBOL void ts3plugin_onMenuItemEvent(uint64 serverConnectionHandlerID, enum PluginMenuType type, int menuItemID, uint64 selectedItemID);
EXPORT_SYMBOL void ts3plugin_onServerGroupPermListFinishedEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID);
EXPORT_SYMBOL void ts3plugin_onClientPermListEvent(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, unsigned int permissionID, int permissionValue, int permissionNegated, int permissionSkip);
EXPORT_SYMBOL void ts3plugin_onMessageListEvent(uint64 serverConnectionHandlerID, uint64 messageID, const char *fromClientUniqueIdentity, const char *subject, uint64 timestamp, int flagRead);
EXPORT_SYMBOL void ts3plugin_onSoundDeviceListChangedEvent(const char *modeID, int playOrCap);
EXPORT_SYMBOL void ts3plugin_onServerLogFinishedEvent(uint64 serverConnectionHandlerID, uint64 lastPos, uint64 fileSize);
EXPORT_SYMBOL void ts3plugin_onServerGroupByClientIDEvent(uint64 serverConnectionHandlerID, const char *name, uint64 serverGroupList, uint64 clientDatabaseID);
EXPORT_SYMBOL void ts3plugin_onFileListEvent(uint64 serverConnectionHandlerID, uint64 channelID, const char *path, const char *name, uint64 size, uint64 datetime, int type, uint64 incompletesize, const char *returnCode);
EXPORT_SYMBOL void ts3plugin_onIncomingClientQueryEvent(uint64 serverConnectionHandlerID, const char *commandText);
EXPORT_SYMBOL void ts3plugin_onServerLogEvent(uint64 serverConnectionHandlerID, const char *logMsg);
EXPORT_SYMBOL void ts3plugin_onComplainListEvent(uint64 serverConnectionHandlerID, uint64 targetClientDatabaseID, const char *targetClientNickName, uint64 fromClientDatabaseID, const char *fromClientNickName, const char *complainReason, uint64 timestamp);
EXPORT_SYMBOL void ts3plugin_onServerUpdatedEvent(uint64 serverConnectionHandlerID);
EXPORT_SYMBOL void ts3plugin_onChannelGroupListEvent(uint64 serverConnectionHandlerID, uint64 channelGroupID, const char *name, int type, int iconID, int saveDB);
EXPORT_SYMBOL void ts3plugin_onChannelSubscribeEvent(uint64 serverConnectionHandlerID, uint64 channelID);
EXPORT_SYMBOL void ts3plugin_onChannelDescriptionUpdateEvent(uint64 serverConnectionHandlerID, uint64 channelID);
EXPORT_SYMBOL void ts3plugin_onServerGroupClientListEvent(uint64 serverConnectionHandlerID, uint64 serverGroupID, uint64 clientDatabaseID, const char *clientNameIdentifier, const char *clientUniqueID);
EXPORT_SYMBOL void ts3plugin_onPermissionOverviewFinishedEvent(uint64 serverConnectionHandlerID);

#ifndef PYTSON_PARSER
#ifdef __cplusplus
}
#endif
#endif

#endif
