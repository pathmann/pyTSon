#include "ts3module.h"

#include <QObject>
#include <QString>

#include "teamspeak/public_definitions.h"
#include "teamspeak/public_errors.h"

#include "global_shared.h"
#include "pyconversion.h"

#include "PythonQtInstanceWrapper.h"

//this macros makes the code "slightly" ugly, but as long as we use a strict C-Parser, it's necessary
#ifdef PYTSON_PARSER
#define TRANS (void*)
#define PYLIST_TO_ARRAY(type, list, error, ret, appendNull) (void*)(list, error, ret, appendNull)
#define ARRAY_TO_PYLIST(type, array, formatChar, error, ret, len) (void*)(array, formatChar, error, ret, len)
#define VECTOR(x, y, z) (void*)(x, y, z)
#define CPPALLOC(type, size) (void*)(size)
#define delete
#define CPPDELARR(arr) (void*)(arr)
#else
#define TRANS QObject::tr
#define PYLIST_TO_ARRAY(type, list, error, ret, appendNull) pyListToArray<type>(list, error, ret, appendNull)
#define ARRAY_TO_PYLIST(type, array, formatChar, error, ret, len) arrayToPyList<type>(array, formatChar, error, ret, len)
#define VECTOR(x, y, z) {x, y, z}
#define CPPALLOC(type, size) new type[size]
#define CPPDELARR(arr) delete[] arr
#endif

void freeBookmarkList(struct PluginBookmarkList* list) {
  for (int i = 0; i < list->itemcount; ++i) {
    ts3_funcs.freeMemory(list->items[i].name);

    if (list->items[i].isFolder)
      freeBookmarkList(list->items[i].folder);
    else ts3_funcs.freeMemory(list->items[i].uuid);
  }

  ts3_funcs.freeMemory(list);
}

/*
# coding: utf-8

class ts3:
    """

    """

*/

#ifndef PYTSON_PARSER
static PyMethodDef ts3modfuncs[] = {
  {"getPluginID", getPluginID, METH_VARARGS, "Get pyTSon's pluginID"},
  {"acquireCustomPlaybackData", acquireCustomPlaybackData, METH_VARARGS, "Call acquireCustomPlaybackData of the ts3 client plugin sdk"},
  {"activateCaptureDevice", activateCaptureDevice, METH_VARARGS, "Call activateCaptureDevice of the ts3 client plugin sdk"},
  {"banadd", banadd, METH_VARARGS, "Call banadd of the ts3 client plugin sdk"},
  {"banclient", banclient, METH_VARARGS, "Call banclient of the ts3 client plugin sdk"},
  {"banclientdbid", banclientdbid, METH_VARARGS, "Call banclientdbid of the ts3 client plugin sdk"},
  {"bandel", bandel, METH_VARARGS, "Call bandel of the ts3 client plugin sdk"},
  {"bandelall", bandelall, METH_VARARGS, "Call bandelall of the ts3 client plugin sdk"},
  {"channelPropertyStringToFlag", channelPropertyStringToFlag, METH_VARARGS, "Call channelPropertyStringToFlag of the ts3 client plugin sdk"},
  {"channelset3DAttributes", channelset3DAttributes, METH_VARARGS, "Call channelset3DAttributes of the ts3 client plugin sdk"},
  {"cleanUpConnectionInfo", cleanUpConnectionInfo, METH_VARARGS, "Call cleanUpConnectionInfo of the ts3 client plugin sdk"},
  {"clientChatClosed", clientChatClosed, METH_VARARGS, "Call clientChatClosed of the ts3 client plugin sdk"},
  {"clientChatComposing", clientChatComposing, METH_VARARGS, "Call clientChatComposing of the ts3 client plugin sdk"},
  {"clientPropertyStringToFlag", clientPropertyStringToFlag, METH_VARARGS, "Call clientPropertyStringToFlag of the ts3 client plugin sdk"},
  {"closeCaptureDevice", closeCaptureDevice, METH_VARARGS, "Call closeCaptureDevice of the ts3 client plugin sdk"},
  {"closePlaybackDevice", closePlaybackDevice, METH_VARARGS, "Call closePlaybackDevice of the ts3 client plugin sdk"},
  {"closeWaveFileHandle", closeWaveFileHandle, METH_VARARGS, "Call closeWaveFileHandle of the ts3 client plugin sdk"},
  {"createBookmark", createBookmark, METH_VARARGS, "Call createBookmark of the ts3 client plugin sdk"},
  {"createReturnCode", createReturnCode, METH_VARARGS, "Call createReturnCode of the ts3 client plugin sdk"},
  {"destroyServerConnectionHandler", destroyServerConnectionHandler, METH_VARARGS, "Call destroyServerConnectionHandler of the ts3 client plugin sdk"},
  {"flushChannelCreation", flushChannelCreation, METH_VARARGS, "Call flushChannelCreation of the ts3 client plugin sdk"},
  {"flushChannelUpdates", flushChannelUpdates, METH_VARARGS, "Call flushChannelUpdates of the ts3 client plugin sdk"},
  {"flushClientSelfUpdates", flushClientSelfUpdates, METH_VARARGS, "Call flushClientSelfUpdates of the ts3 client plugin sdk"},
  {"getAppPath", getAppPath, METH_VARARGS, "Call getAppPath of the ts3 client plugin sdk"},
  {"getAvatar", getAvatar, METH_VARARGS, "Call getAvatar of the ts3 client plugin sdk"},
  {"getAverageTransferSpeed", getAverageTransferSpeed, METH_VARARGS, "Call getAverageTransferSpeed of the ts3 client plugin sdk"},
  {"getBookmarkList", getBookmarkList, METH_VARARGS, "Call getBookmarkList of the ts3 client plugin sdk"},
  {"getCaptureDeviceList", getCaptureDeviceList, METH_VARARGS, "Call getCaptureDeviceList of the ts3 client plugin sdk"},
  {"getCaptureModeList", getCaptureModeList, METH_VARARGS, "Call getCaptureModeList of the ts3 client plugin sdk"},
  {"getChannelClientList", getChannelClientList, METH_VARARGS, "Call getChannelClientList of the ts3 client plugin sdk"},
  {"getChannelConnectInfo", getChannelConnectInfo, METH_VARARGS, "Call getChannelConnectInfo of the ts3 client plugin sdk"},
  {"getChannelIDFromChannelNames", getChannelIDFromChannelNames, METH_VARARGS, "Call getChannelIDFromChannelNames of the ts3 client plugin sdk"},
  {"getChannelList", getChannelList, METH_VARARGS, "Call getChannelList of the ts3 client plugin sdk"},
  {"getChannelOfClient", getChannelOfClient, METH_VARARGS, "Call getChannelOfClient of the ts3 client plugin sdk"},
  {"getChannelVariableAsInt", getChannelVariableAsInt, METH_VARARGS, "Call getChannelVariableAsInt of the ts3 client plugin sdk"},
  {"getChannelVariableAsString", getChannelVariableAsString, METH_VARARGS, "Call getChannelVariableAsString of the ts3 client plugin sdk"},
  {"getChannelVariableAsUInt64", getChannelVariableAsUInt64, METH_VARARGS, "Call getChannelVariableAsUInt64 of the ts3 client plugin sdk"},
  {"getClientDisplayName", getClientDisplayName, METH_VARARGS, "Call getClientDisplayName of the ts3 client plugin sdk"},
  {"getClientID", getClientID, METH_VARARGS, "Call getClientID of the ts3 client plugin sdk"},
  {"getClientLibVersion", getClientLibVersion, METH_VARARGS, "Call getClientLibVersion of the ts3 client plugin sdk"},
  {"getClientLibVersionNumber", getClientLibVersionNumber, METH_VARARGS, "Call getClientLibVersionNumber of the ts3 client plugin sdk"},
  {"getClientList", getClientList, METH_VARARGS, "Call getClientList of the ts3 client plugin sdk"},
  {"getClientNeededPermission", getClientNeededPermission, METH_VARARGS, "Call getClientNeededPermission of the ts3 client plugin sdk"},
  {"getClientSelfVariableAsInt", getClientSelfVariableAsInt, METH_VARARGS, "Call getClientSelfVariableAsInt of the ts3 client plugin sdk"},
  {"getClientSelfVariableAsString", getClientSelfVariableAsString, METH_VARARGS, "Call getClientSelfVariableAsString of the ts3 client plugin sdk"},
  {"getClientVariableAsInt", getClientVariableAsInt, METH_VARARGS, "Call getClientVariableAsInt of the ts3 client plugin sdk"},
  {"getClientVariableAsString", getClientVariableAsString, METH_VARARGS, "Call getClientVariableAsString of the ts3 client plugin sdk"},
  {"getClientVariableAsUInt64", getClientVariableAsUInt64, METH_VARARGS, "Call getClientVariableAsUInt64 of the ts3 client plugin sdk"},
  {"getConfigPath", getConfigPath, METH_VARARGS, "Call getConfigPath of the ts3 client plugin sdk"},
  {"getConnectionStatus", getConnectionStatus, METH_VARARGS, "Call getConnectionStatus of the ts3 client plugin sdk"},
  {"getConnectionVariableAsDouble", getConnectionVariableAsDouble, METH_VARARGS, "Call getConnectionVariableAsDouble of the ts3 client plugin sdk"},
  {"getConnectionVariableAsString", getConnectionVariableAsString, METH_VARARGS, "Call getConnectionVariableAsString of the ts3 client plugin sdk"},
  {"getConnectionVariableAsUInt64", getConnectionVariableAsUInt64, METH_VARARGS, "Call getConnectionVariableAsUInt64 of the ts3 client plugin sdk"},
  {"getCurrentCaptureDeviceName", getCurrentCaptureDeviceName, METH_VARARGS, "Call getCurrentCaptureDeviceName of the ts3 client plugin sdk"},
  {"getCurrentCaptureMode", getCurrentCaptureMode, METH_VARARGS, "Call getCurrentCaptureMode of the ts3 client plugin sdk"},
  {"getCurrentPlaybackDeviceName", getCurrentPlaybackDeviceName, METH_VARARGS, "Call getCurrentPlaybackDeviceName of the ts3 client plugin sdk"},
  {"getCurrentPlayBackMode", getCurrentPlayBackMode, METH_VARARGS, "Call getCurrentPlayBackMode of the ts3 client plugin sdk"},
  {"getCurrentServerConnectionHandlerID", getCurrentServerConnectionHandlerID, METH_VARARGS, "Call getCurrentServerConnectionHandlerID of the ts3 client plugin sdk"},
  {"getCurrentTransferSpeed", getCurrentTransferSpeed, METH_VARARGS, "Call getCurrentTransferSpeed of the ts3 client plugin sdk"},
  {"getDefaultCaptureDevice", getDefaultCaptureDevice, METH_VARARGS, "Call getDefaultCaptureDevice of the ts3 client plugin sdk"},
  {"getDefaultCaptureMode", getDefaultCaptureMode, METH_VARARGS, "Call getDefaultCaptureMode of the ts3 client plugin sdk"},
  {"getDefaultPlaybackDevice", getDefaultPlaybackDevice, METH_VARARGS, "Call getDefaultPlaybackDevice of the ts3 client plugin sdk"},
  {"getDefaultPlayBackMode", getDefaultPlayBackMode, METH_VARARGS, "Call getDefaultPlayBackMode of the ts3 client plugin sdk"},
  {"getDirectories", getDirectories, METH_VARARGS, "Call getDirectories of the ts3 client plugin sdk"},
  {"getEncodeConfigValue", getEncodeConfigValue, METH_VARARGS, "Call getEncodeConfigValue of the ts3 client plugin sdk"},
  {"getErrorMessage", getErrorMessage, METH_VARARGS, "Call getErrorMessage of the ts3 client plugin sdk"},
  {"getHotkeyFromKeyword", getHotkeyFromKeyword, METH_VARARGS, "Call getHotkeyFromKeyword of the ts3 client plugin sdk"},
  {"getParentChannelOfChannel", getParentChannelOfChannel, METH_VARARGS, "Call getParentChannelOfChannel of the ts3 client plugin sdk"},
  {"getPermissionIDByName", getPermissionIDByName, METH_VARARGS, "Call getPermissionIDByName of the ts3 client plugin sdk"},
  {"getPlaybackConfigValueAsFloat", getPlaybackConfigValueAsFloat, METH_VARARGS, "Call getPlaybackConfigValueAsFloat of the ts3 client plugin sdk"},
  {"getPlaybackDeviceList", getPlaybackDeviceList, METH_VARARGS, "Call getPlaybackDeviceList of the ts3 client plugin sdk"},
  {"getPlaybackModeList", getPlaybackModeList, METH_VARARGS, "Call getPlaybackModeList of the ts3 client plugin sdk"},
  {"getPluginPath", getPluginPath, METH_VARARGS, "Call getPluginPath of the ts3 client plugin sdk"},
  {"getPreProcessorConfigValue", getPreProcessorConfigValue, METH_VARARGS, "Call getPreProcessorConfigValue of the ts3 client plugin sdk"},
  {"getPreProcessorInfoValueFloat", getPreProcessorInfoValueFloat, METH_VARARGS, "Call getPreProcessorInfoValueFloat of the ts3 client plugin sdk"},
  {"getProfileList", getProfileList, METH_VARARGS, "Call getProfileList of the ts3 client plugin sdk"},
  {"getResourcesPath", getResourcesPath, METH_VARARGS, "Call getResourcesPath of the ts3 client plugin sdk"},
  {"getServerConnectInfo", getServerConnectInfo, METH_VARARGS, "Call getServerConnectInfo of the ts3 client plugin sdk"},
  {"getServerConnectionHandlerList", getServerConnectionHandlerList, METH_VARARGS, "Call getServerConnectionHandlerList of the ts3 client plugin sdk"},
  {"getServerVariableAsInt", getServerVariableAsInt, METH_VARARGS, "Call getServerVariableAsInt of the ts3 client plugin sdk"},
  {"getServerVariableAsString", getServerVariableAsString, METH_VARARGS, "Call getServerVariableAsString of the ts3 client plugin sdk"},
  {"getServerVariableAsUInt64", getServerVariableAsUInt64, METH_VARARGS, "Call getServerVariableAsUInt64 of the ts3 client plugin sdk"},
  {"getServerVersion", getServerVersion, METH_VARARGS, "Call getServerVersion of the ts3 client plugin sdk"},
  {"getTransferFileName", getTransferFileName, METH_VARARGS, "Call getTransferFileName of the ts3 client plugin sdk"},
  {"getTransferFilePath", getTransferFilePath, METH_VARARGS, "Call getTransferFilePath of the ts3 client plugin sdk"},
  {"getTransferFileSize", getTransferFileSize, METH_VARARGS, "Call getTransferFileSize of the ts3 client plugin sdk"},
  {"getTransferFileSizeDone", getTransferFileSizeDone, METH_VARARGS, "Call getTransferFileSizeDone of the ts3 client plugin sdk"},
  {"getTransferRunTime", getTransferRunTime, METH_VARARGS, "Call getTransferRunTime of the ts3 client plugin sdk"},
  {"getTransferStatus", getTransferStatus, METH_VARARGS, "Call getTransferStatus of the ts3 client plugin sdk"},
  {"guiConnect", guiConnect, METH_VARARGS, "Call guiConnect of the ts3 client plugin sdk"},
  {"guiConnectBookmark", guiConnectBookmark, METH_VARARGS, "Call guiConnectBookmark of the ts3 client plugin sdk"},
  {"haltTransfer", haltTransfer, METH_VARARGS, "Call haltTransfer of the ts3 client plugin sdk"},
  {"initiateGracefulPlaybackShutdown", initiateGracefulPlaybackShutdown, METH_VARARGS, "Call initiateGracefulPlaybackShutdown of the ts3 client plugin sdk"},
  {"isReceivingWhisper", isReceivingWhisper, METH_VARARGS, "Call isReceivingWhisper of the ts3 client plugin sdk"},
  {"isTransferSender", isTransferSender, METH_VARARGS, "Call isTransferSender of the ts3 client plugin sdk"},
  {"isWhispering", isWhispering, METH_VARARGS, "Call isWhispering of the ts3 client plugin sdk"},
  {"logMessage", logMessage, METH_VARARGS, "Call logMessage of the ts3 client plugin sdk"},
  {"openCaptureDevice", openCaptureDevice, METH_VARARGS, "Call openCaptureDevice of the ts3 client plugin sdk"},
  {"openPlaybackDevice", openPlaybackDevice, METH_VARARGS, "Call openPlaybackDevice of the ts3 client plugin sdk"},
  {"pauseWaveFileHandle", pauseWaveFileHandle, METH_VARARGS, "Call pauseWaveFileHandle of the ts3 client plugin sdk"},
  {"playWaveFile", playWaveFile, METH_VARARGS, "Call playWaveFile of the ts3 client plugin sdk"},
  {"playWaveFileHandle", playWaveFileHandle, METH_VARARGS, "Call playWaveFileHandle of the ts3 client plugin sdk"},
  {"printMessage", printMessage, METH_VARARGS, "Call printMessage of the ts3 client plugin sdk"},
  {"printMessageToCurrentTab", printMessageToCurrentTab, METH_VARARGS, "Call printMessageToCurrentTab of the ts3 client plugin sdk"},
  {"privilegeKeyUse", privilegeKeyUse, METH_VARARGS, "Call privilegeKeyUse of the ts3 client plugin sdk"},
  {"processCustomCaptureData", processCustomCaptureData, METH_VARARGS, "Call processCustomCaptureData of the ts3 client plugin sdk"},
  {"registerCustomDevice", registerCustomDevice, METH_VARARGS, "Call registerCustomDevice of the ts3 client plugin sdk"},
  {"requestBanList", requestBanList, METH_VARARGS, "Call requestBanList of the ts3 client plugin sdk"},
  {"requestChannelAddPerm", requestChannelAddPerm, METH_VARARGS, "Call requestChannelAddPerm of the ts3 client plugin sdk"},
  {"requestChannelClientAddPerm", requestChannelClientAddPerm, METH_VARARGS, "Call requestChannelClientAddPerm of the ts3 client plugin sdk"},
  {"requestChannelClientDelPerm", requestChannelClientDelPerm, METH_VARARGS, "Call requestChannelClientDelPerm of the ts3 client plugin sdk"},
  {"requestChannelClientPermList", requestChannelClientPermList, METH_VARARGS, "Call requestChannelClientPermList of the ts3 client plugin sdk"},
  {"requestChannelDelete", requestChannelDelete, METH_VARARGS, "Call requestChannelDelete of the ts3 client plugin sdk"},
  {"requestChannelDelPerm", requestChannelDelPerm, METH_VARARGS, "Call requestChannelDelPerm of the ts3 client plugin sdk"},
  {"requestChannelDescription", requestChannelDescription, METH_VARARGS, "Call requestChannelDescription of the ts3 client plugin sdk"},
  {"requestChannelGroupAdd", requestChannelGroupAdd, METH_VARARGS, "Call requestChannelGroupAdd of the ts3 client plugin sdk"},
  {"requestChannelGroupAddPerm", requestChannelGroupAddPerm, METH_VARARGS, "Call requestChannelGroupAddPerm of the ts3 client plugin sdk"},
  {"requestChannelGroupDel", requestChannelGroupDel, METH_VARARGS, "Call requestChannelGroupDel of the ts3 client plugin sdk"},
  {"requestChannelGroupDelPerm", requestChannelGroupDelPerm, METH_VARARGS, "Call requestChannelGroupDelPerm of the ts3 client plugin sdk"},
  {"requestChannelGroupList", requestChannelGroupList, METH_VARARGS, "Call requestChannelGroupList of the ts3 client plugin sdk"},
  {"requestChannelGroupPermList", requestChannelGroupPermList, METH_VARARGS, "Call requestChannelGroupPermList of the ts3 client plugin sdk"},
  {"requestChannelMove", requestChannelMove, METH_VARARGS, "Call requestChannelMove of the ts3 client plugin sdk"},
  {"requestChannelPermList", requestChannelPermList, METH_VARARGS, "Call requestChannelPermList of the ts3 client plugin sdk"},
  {"requestChannelSubscribe", requestChannelSubscribe, METH_VARARGS, "Call requestChannelSubscribe of the ts3 client plugin sdk"},
  {"requestChannelSubscribeAll", requestChannelSubscribeAll, METH_VARARGS, "Call requestChannelSubscribeAll of the ts3 client plugin sdk"},
  {"requestChannelUnsubscribe", requestChannelUnsubscribe, METH_VARARGS, "Call requestChannelUnsubscribe of the ts3 client plugin sdk"},
  {"requestChannelUnsubscribeAll", requestChannelUnsubscribeAll, METH_VARARGS, "Call requestChannelUnsubscribeAll of the ts3 client plugin sdk"},
  {"requestClientAddPerm", requestClientAddPerm, METH_VARARGS, "Call requestClientAddPerm of the ts3 client plugin sdk"},
  {"requestClientDBIDfromUID", requestClientDBIDfromUID, METH_VARARGS, "Call requestClientDBIDfromUID of the ts3 client plugin sdk"},
  {"requestClientDelPerm", requestClientDelPerm, METH_VARARGS, "Call requestClientDelPerm of the ts3 client plugin sdk"},
  {"requestClientEditDescription", requestClientEditDescription, METH_VARARGS, "Call requestClientEditDescription of the ts3 client plugin sdk"},
  {"requestClientIDs", requestClientIDs, METH_VARARGS, "Call requestClientIDs of the ts3 client plugin sdk"},
  {"requestClientKickFromChannel", requestClientKickFromChannel, METH_VARARGS, "Call requestClientKickFromChannel of the ts3 client plugin sdk"},
  {"requestClientKickFromServer", requestClientKickFromServer, METH_VARARGS, "Call requestClientKickFromServer of the ts3 client plugin sdk"},
  {"requestClientMove", requestClientMove, METH_VARARGS, "Call requestClientMove of the ts3 client plugin sdk"},
  {"requestClientNamefromDBID", requestClientNamefromDBID, METH_VARARGS, "Call requestClientNamefromDBID of the ts3 client plugin sdk"},
  {"requestClientNamefromUID", requestClientNamefromUID, METH_VARARGS, "Call requestClientNamefromUID of the ts3 client plugin sdk"},
  {"requestClientPermList", requestClientPermList, METH_VARARGS, "Call requestClientPermList of the ts3 client plugin sdk"},
  {"requestClientPoke", requestClientPoke, METH_VARARGS, "Call requestClientPoke of the ts3 client plugin sdk"},
  {"requestClientSetIsTalker", requestClientSetIsTalker, METH_VARARGS, "Call requestClientSetIsTalker of the ts3 client plugin sdk"},
  {"requestClientSetWhisperList", requestClientSetWhisperList, METH_VARARGS, "Call requestClientSetWhisperList of the ts3 client plugin sdk"},
  {"requestClientVariables", requestClientVariables, METH_VARARGS, "Call requestClientVariables of the ts3 client plugin sdk"},
  {"requestComplainAdd", requestComplainAdd, METH_VARARGS, "Call requestComplainAdd of the ts3 client plugin sdk"},
  {"requestComplainDel", requestComplainDel, METH_VARARGS, "Call requestComplainDel of the ts3 client plugin sdk"},
  {"requestComplainDelAll", requestComplainDelAll, METH_VARARGS, "Call requestComplainDelAll of the ts3 client plugin sdk"},
  {"requestComplainList", requestComplainList, METH_VARARGS, "Call requestComplainList of the ts3 client plugin sdk"},
  {"requestConnectionInfo", requestConnectionInfo, METH_VARARGS, "Call requestConnectionInfo of the ts3 client plugin sdk"},
  {"requestCreateDirectory", requestCreateDirectory, METH_VARARGS, "Call requestCreateDirectory of the ts3 client plugin sdk"},
  {"requestDeleteFile", requestDeleteFile, METH_VARARGS, "Call requestDeleteFile of the ts3 client plugin sdk"},
  {"requestFile", requestFile, METH_VARARGS, "Call requestFile of the ts3 client plugin sdk"},
  {"requestFileInfo", requestFileInfo, METH_VARARGS, "Call requestFileInfo of the ts3 client plugin sdk"},
  {"requestFileList", requestFileList, METH_VARARGS, "Call requestFileList of the ts3 client plugin sdk"},
  {"requestHotkeyInputDialog", requestHotkeyInputDialog, METH_VARARGS, "Call requestHotkeyInputDialog of the ts3 client plugin sdk"},
  {"requestInfoUpdate", requestInfoUpdate, METH_VARARGS, "Call requestInfoUpdate of the ts3 client plugin sdk"},
  {"requestIsTalker", requestIsTalker, METH_VARARGS, "Call requestIsTalker of the ts3 client plugin sdk"},
  {"requestMessageAdd", requestMessageAdd, METH_VARARGS, "Call requestMessageAdd of the ts3 client plugin sdk"},
  {"requestMessageDel", requestMessageDel, METH_VARARGS, "Call requestMessageDel of the ts3 client plugin sdk"},
  {"requestMessageGet", requestMessageGet, METH_VARARGS, "Call requestMessageGet of the ts3 client plugin sdk"},
  {"requestMessageList", requestMessageList, METH_VARARGS, "Call requestMessageList of the ts3 client plugin sdk"},
  {"requestMessageUpdateFlag", requestMessageUpdateFlag, METH_VARARGS, "Call requestMessageUpdateFlag of the ts3 client plugin sdk"},
  {"requestMuteClients", requestMuteClients, METH_VARARGS, "Call requestMuteClients of the ts3 client plugin sdk"},
  {"requestPermissionList", requestPermissionList, METH_VARARGS, "Call requestPermissionList of the ts3 client plugin sdk"},
  {"requestPermissionOverview", requestPermissionOverview, METH_VARARGS, "Call requestPermissionOverview of the ts3 client plugin sdk"},
  {"requestRenameFile", requestRenameFile, METH_VARARGS, "Call requestRenameFile of the ts3 client plugin sdk"},
  {"requestSendChannelTextMsg", requestSendChannelTextMsg, METH_VARARGS, "Call requestSendChannelTextMsg of the ts3 client plugin sdk"},
  {"requestSendClientQueryCommand", requestSendClientQueryCommand, METH_VARARGS, "Call requestSendClientQueryCommand of the ts3 client plugin sdk"},
  {"requestSendPrivateTextMsg", requestSendPrivateTextMsg, METH_VARARGS, "Call requestSendPrivateTextMsg of the ts3 client plugin sdk"},
  {"requestSendServerTextMsg", requestSendServerTextMsg, METH_VARARGS, "Call requestSendServerTextMsg of the ts3 client plugin sdk"},
  {"requestServerGroupAdd", requestServerGroupAdd, METH_VARARGS, "Call requestServerGroupAdd of the ts3 client plugin sdk"},
  {"requestServerGroupAddClient", requestServerGroupAddClient, METH_VARARGS, "Call requestServerGroupAddClient of the ts3 client plugin sdk"},
  {"requestServerGroupAddPerm", requestServerGroupAddPerm, METH_VARARGS, "Call requestServerGroupAddPerm of the ts3 client plugin sdk"},
  {"requestServerGroupClientList", requestServerGroupClientList, METH_VARARGS, "Call requestServerGroupClientList of the ts3 client plugin sdk"},
  {"requestServerGroupDel", requestServerGroupDel, METH_VARARGS, "Call requestServerGroupDel of the ts3 client plugin sdk"},
  {"requestServerGroupDelClient", requestServerGroupDelClient, METH_VARARGS, "Call requestServerGroupDelClient of the ts3 client plugin sdk"},
  {"requestServerGroupDelPerm", requestServerGroupDelPerm, METH_VARARGS, "Call requestServerGroupDelPerm of the ts3 client plugin sdk"},
  {"requestServerGroupList", requestServerGroupList, METH_VARARGS, "Call requestServerGroupList of the ts3 client plugin sdk"},
  {"requestServerGroupPermList", requestServerGroupPermList, METH_VARARGS, "Call requestServerGroupPermList of the ts3 client plugin sdk"},
  {"requestServerGroupsByClientID", requestServerGroupsByClientID, METH_VARARGS, "Call requestServerGroupsByClientID of the ts3 client plugin sdk"},
  {"requestServerTemporaryPasswordAdd", requestServerTemporaryPasswordAdd, METH_VARARGS, "Call requestServerTemporaryPasswordAdd of the ts3 client plugin sdk"},
  {"requestServerTemporaryPasswordDel", requestServerTemporaryPasswordDel, METH_VARARGS, "Call requestServerTemporaryPasswordDel of the ts3 client plugin sdk"},
  {"requestServerTemporaryPasswordList", requestServerTemporaryPasswordList, METH_VARARGS, "Call requestServerTemporaryPasswordList of the ts3 client plugin sdk"},
  {"requestServerVariables", requestServerVariables, METH_VARARGS, "Call requestServerVariables of the ts3 client plugin sdk"},
  {"requestSetClientChannelGroup", requestSetClientChannelGroup, METH_VARARGS, "Call requestSetClientChannelGroup of the ts3 client plugin sdk"},
  {"requestUnmuteClients", requestUnmuteClients, METH_VARARGS, "Call requestUnmuteClients of the ts3 client plugin sdk"},
  {"sendFile", sendFile, METH_VARARGS, "Call sendFile of the ts3 client plugin sdk"},
  {"sendPluginCommand", sendPluginCommand, METH_VARARGS, "Call sendPluginCommand of the ts3 client plugin sdk"},
  {"serverPropertyStringToFlag", serverPropertyStringToFlag, METH_VARARGS, "Call serverPropertyStringToFlag of the ts3 client plugin sdk"},
  {"set3DWaveAttributes", set3DWaveAttributes, METH_VARARGS, "Call set3DWaveAttributes of the ts3 client plugin sdk"},
  {"setChannelVariableAsInt", setChannelVariableAsInt, METH_VARARGS, "Call setChannelVariableAsInt of the ts3 client plugin sdk"},
  {"setChannelVariableAsString", setChannelVariableAsString, METH_VARARGS, "Call setChannelVariableAsString of the ts3 client plugin sdk"},
  {"setChannelVariableAsUInt64", setChannelVariableAsUInt64, METH_VARARGS, "Call setChannelVariableAsUInt64 of the ts3 client plugin sdk"},
  {"setClientSelfVariableAsInt", setClientSelfVariableAsInt, METH_VARARGS, "Call setClientSelfVariableAsInt of the ts3 client plugin sdk"},
  {"setClientSelfVariableAsString", setClientSelfVariableAsString, METH_VARARGS, "Call setClientSelfVariableAsString of the ts3 client plugin sdk"},
  {"setClientVolumeModifier", setClientVolumeModifier, METH_VARARGS, "Call setClientVolumeModifier of the ts3 client plugin sdk"},
  {"setPlaybackConfigValue", setPlaybackConfigValue, METH_VARARGS, "Call setPlaybackConfigValue of the ts3 client plugin sdk"},
  {"setPluginMenuEnabled", setPluginMenuEnabled, METH_VARARGS, "Call setPluginMenuEnabled of the ts3 client plugin sdk"},
  {"setPreProcessorConfigValue", setPreProcessorConfigValue, METH_VARARGS, "Call setPreProcessorConfigValue of the ts3 client plugin sdk"},
  {"showHotkeySetup", showHotkeySetup, METH_VARARGS, "Call showHotkeySetup of the ts3 client plugin sdk"},
  {"spawnNewServerConnectionHandler", spawnNewServerConnectionHandler, METH_VARARGS, "Call spawnNewServerConnectionHandler of the ts3 client plugin sdk"},
  {"startConnection", startConnection, METH_VARARGS, "Call startConnection of the ts3 client plugin sdk"},
  {"startVoiceRecording", startVoiceRecording, METH_VARARGS, "Call startVoiceRecording of the ts3 client plugin sdk"},
  {"stopConnection", stopConnection, METH_VARARGS, "Call stopConnection of the ts3 client plugin sdk"},
  {"stopVoiceRecording", stopVoiceRecording, METH_VARARGS, "Call stopVoiceRecording of the ts3 client plugin sdk"},
  {"systemset3DListenerAttributes", systemset3DListenerAttributes, METH_VARARGS, "Call systemset3DListenerAttributes of the ts3 client plugin sdk"},
  {"systemset3DSettings", systemset3DSettings, METH_VARARGS, "Call systemset3DSettings of the ts3 client plugin sdk"},
  {"unregisterCustomDevice", unregisterCustomDevice, METH_VARARGS, "Call unregisterCustomDevice of the ts3 client plugin sdk"},
  {"urlsToBB", urlsToBB, METH_VARARGS, "Call urlsToBB of the ts3 client plugin sdk"},
  {"verifyChannelPassword", verifyChannelPassword, METH_VARARGS, "Call verifyChannelPassword of the ts3 client plugin sdk"},
  {"verifyServerPassword", verifyServerPassword, METH_VARARGS, "Call verifyServerPassword of the ts3 client plugin sdk"},
  {NULL, NULL, 0, NULL}
};

static PyModuleDef mdef = {
  PyModuleDef_HEAD_INIT,
  "ts3lib",
  NULL,
  -1,
  ts3modfuncs,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC PyInit_ts3lib(void) {
 return PyModule_Create(&mdef);
}
#endif

PyObject* getPluginID(PyObject* /*self*/, PyObject* args) {
  /*
    @staticmethod
    def getPluginID():
        """
        Returns pyTSon's plugin id
        @return: the plugin id
        @rtype: string
        """
  */
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  return Py_BuildValue("s", ts3_pluginid);
}

PyObject* acquireCustomPlaybackData(PyObject* /*self*/, PyObject* args) {
  //unsigned int acquireCustomPlaybackData(const char *deviceName, short *buffer, int samples)
  /*
    @staticmethod
    def acquireCustomPlaybackData(deviceName, samples):
        """
        Retrieves playback data from the clientlib
        @param deviceName: the name of the playback device previously registered with registerCustomDevice
        @type deviceName: string
        @param samples: specifies how long the resultbuffer should be, which is passed to the clientlib
        @type samples: int
        @return: the errorcode
        @rtype: int
        """
  */
  char* deviceName;
  int samples;

  if (!PyArg_ParseTuple(args, "si", &deviceName, &samples))
    return NULL;

  short* buffer = CPPALLOC(short, samples);
  unsigned int res = ts3_funcs.acquireCustomPlaybackData(deviceName, buffer, samples);

  PyObject* pyret;
  if (res == ERROR_ok) {
    QString error;
    PyObject* pybuffer;

    if (!ARRAY_TO_PYLIST(short, buffer, "h", error, &pybuffer, samples)) {
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with \"%1\"").arg(error).toUtf8().data());
      return NULL;
    }
    pyret = Py_BuildValue("(IO)", res, pybuffer);
    Py_DECREF(pybuffer);
  }
  else pyret = Py_BuildValue("(Is)", res, NULL);
  CPPDELARR(buffer);

  return pyret;
}

PyObject* activateCaptureDevice(PyObject* /*self*/, PyObject* args) {
  //unsigned int activateCaptureDevice(uint64 serverConnectionHandlerID)
  /*
    @staticmethod
    def activateCaptureDevice(serverConnectionHandlerID):
        """
        Activates the capture device on a serverconnection.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  unsigned int res = ts3_funcs.activateCaptureDevice(schid);

  return Py_BuildValue("I", res);
}

PyObject* banadd(PyObject* /*self*/, PyObject* args) {
  //unsigned int banadd(uint64 serverConnectionHandlerID, const char *ipRegExp, const char *nameRegexp, const char *uniqueIdentity, uint64 timeInSeconds, const char *banReason, const char *returnCode)
  /*
    @staticmethod
    def banadd(serverConnectionHandlerID, ipRegExp, nameRegexp, uniqueIdentity, timeInSeconds, banReason, returnCode):
        """
        Adds a new ban.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param ipRegExp: regular expression to match IPs, pass an empty string to ignore IPs
        @type ipRegExp: string
        @param nameRegexp: regular expression to match client nicknames, pass an empty string to ignore nicknames
        @type nameRegexp: string
        @param uniqueIdentity: client UID to ban, pass an empty string to ignore UIDs
        @type uniqueIdentity: string
        @param timeInSeconds: the time, the client should be banned for, pass 0 to add a permanent ban
        @type timeInSeconds: int
        @param banReason: the reason for the ban
        @type banReason: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* ipRegExp;
  char* nameRegexp;
  char* uniqueIdentity;
  unsigned long long timeInSeconds;
  char* banReason;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KsssKs|s", &schid, &ipRegExp, &nameRegexp, &uniqueIdentity, &timeInSeconds, &banReason, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.banadd((uint64)schid, ipRegExp, nameRegexp, uniqueIdentity, (uint64)timeInSeconds, banReason, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* banclient(PyObject* /*self*/, PyObject* args) {
  //unsigned int banclient(uint64 serverConnectionHandlerID, anyID clientID, uint64 timeInSeconds, const char *banReason, const char *returnCode)
  /*
    @staticmethod
    def banclient(serverConnectionHandlerID, clientID, timeInSeconds, banReason, returnCode):
        """
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @param timeInSeconds: the time, the client should be banned for, pass 0 to add a permanent ban
        @type timeInSeconds: int
        @param banReason: the reason for the ban
        @type banReason: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  unsigned long long timeInSeconds;
  char* banReason;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KIKs|s", &schid, &clientID, &timeInSeconds, &banReason, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.banclient((uint64)schid, (anyID)clientID, (uint64)timeInSeconds, banReason, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* banclientdbid(PyObject* /*self*/, PyObject* args) {
  //unsigned int banclientdbid(uint64 serverConnectionHandlerID, uint64 clientDBID, uint64 timeInSeconds, const char *banReason, const char *returnCode)
  /*
    @staticmethod
    def banclientdbid(serverConnectionHandlerID, clientDBID, timeInSeconds, banReason, returnCode):
        """
        Bans a user defined by his database ID.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientDBID: the database ID of the user
        @type clientDBID: int
        @param timeInSeconds: the time, the client should be banned for, pass 0 to add a permanent ban
        @type timeInSeconds: int
        @param banReason: the reason for the ban
        @type banReason: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long clientDBID;
  unsigned long long timeInSeconds;
  char* banReason;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKKs|s", &schid, &clientDBID, &timeInSeconds, &banReason, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.banclientdbid((uint64)schid, (uint64)clientDBID, (uint64)timeInSeconds, banReason, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* bandel(PyObject* /*self*/, PyObject* args) {
  //unsigned int bandel(uint64 serverConnectionHandlerID, uint64 banID, const char *returnCode)
  /*
    @staticmethod
    def bandel(serverConnectionHandlerID, banID, returnCode):
        """
        Deletes a ban.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param banID: the ID of the ban
        @type banID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long banID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &banID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.bandel((uint64)schid, (uint64)banID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* bandelall(PyObject* /*self*/, PyObject* args) {
  //unsigned int bandelall(uint64 serverConnectionHandlerID, const char *returnCode)
  /*
    @staticmethod
    def bandelall(serverConnectionHandlerID, returnCode):
        """
        Deletes all bans on a server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "K|s", &schid, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.bandelall((uint64)schid, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* channelPropertyStringToFlag(PyObject* /*self*/, PyObject* args) {
  //unsigned int channelPropertyStringToFlag(const char *channelPropertyString, size_t *resultFlag)
  /*
    @staticmethod
    def channelPropertyStringToFlag(channelPropertyString):
        """
        Converts a channel property name used in strings (eg the serverquery) to the corresponding flag.
        @param channelPropertyString:
        @type channelPropertyString: string
        @return: a tuple, containing the errorcode and the flag (see ts3defines.ChannelProperties and ts3defines.ChannelPropertiesRare)
        @rtype: tuple (int, int)
        """
  */
  char* channelPropertyString;

  if (!PyArg_ParseTuple(args, "s", &channelPropertyString))
    return NULL;

  size_t resultFlag;
  unsigned int res = ts3_funcs.channelPropertyStringToFlag(channelPropertyString, &resultFlag);

  return Py_BuildValue("(II)", res, (unsigned int)resultFlag);
}

PyObject* channelset3DAttributes(PyObject* /*self*/, PyObject* args) {
  //unsigned int channelset3DAttributes(uint64 serverConnectionHandlerID, anyID clientID, const TS3_VECTOR *position)
  /*
    @staticmethod
    def channelset3DAttributes(serverConnectionHandlerID, clientID, position):
        """
        Adjusts a clients position and velocity in 3D space.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client to adjust
        @type clientID: int
        @param position: a tuple defining the position of the clientID
        @type position: tuple (float, float, float)
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  float x, y, z;

  if (!PyArg_ParseTuple(args, "KI(fff)", &schid, &clientID, &x, &y, &z))
    return NULL;

  TS3_VECTOR position = VECTOR(x, y, z);
  unsigned int res = ts3_funcs.channelset3DAttributes((uint64)schid, (anyID)clientID, &position);

  return Py_BuildValue("I", res);
}

PyObject* cleanUpConnectionInfo(PyObject* /*self*/, PyObject* args) {
  //unsigned int cleanUpConnectionInfo(uint64 serverConnectionHandlerID, anyID clientID)
  /*
    @staticmethod
    def cleanUpConnectionInfo(serverConnectionHandlerID, clientID):
        """
        //FIXME:
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;

  if (!PyArg_ParseTuple(args, "KI", &schid, &clientID))
    return NULL;

  unsigned int res = ts3_funcs.cleanUpConnectionInfo((uint64)schid, (anyID)clientID);

  return Py_BuildValue("I", res);
}

PyObject* clientChatClosed(PyObject* /*self*/, PyObject* args) {
  //unsigned int clientChatClosed(uint64 serverConnectionHandlerID, const char *clientUniqueIdentifier, anyID clientID, const char *returnCode)
  /*
    @staticmethod
    def clientChatClosed(serverConnectionHandlerID, clientUniqueIdentifier, clientID, returnCode):
        """
        Sends the client chat closed command to a client the own client is currently chatting with.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientUniqueIdentifier: the uid of the own chatting client
        @type clientUniqueIdentifier: string
        @param clientID: the ID of the client, the own client is chatting with
        @type clientID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* clientUniqueIdentifier;
  unsigned int clientID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KsI|s", &schid, &clientUniqueIdentifier, &clientID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.clientChatClosed((uint64)schid, clientUniqueIdentifier, (anyID)clientID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* clientChatComposing(PyObject* /*self*/, PyObject* args) {
  //unsigned int clientChatComposing(uint64 serverConnectionHandlerID, anyID clientID, const char *returnCode)
  /*
    @staticmethod
    def clientChatComposing(serverConnectionHandlerID, clientID, returnCode):
        """
        Sends the client chat composing command to a client the own client is currently chatting with.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client, the own client is chatting with
        @type clientID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KI|s", &schid, &clientID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.clientChatComposing((uint64)schid, (anyID)clientID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* clientPropertyStringToFlag(PyObject* /*self*/, PyObject* args) {
  //unsigned int clientPropertyStringToFlag(const char *clientPropertyString, size_t *resultFlag)
  /*
    @staticmethod
    def clientPropertyStringToFlag(clientPropertyString):
        """
        Converts a client property name used in strings (eg the serverquery) to the corresponding flag.
        @param clientPropertyString:
        @type clientPropertyString: string
        @return: a tuple, containing the errorcode and the flag (see ts3defines.ClientProperties and ts3defines.ClientPropertiesRare)
        @rtype: tuple (int, int)
        """
  */
  char* clientPropertyString;

  if (!PyArg_ParseTuple(args, "s", &clientPropertyString))
    return NULL;

  size_t resultFlag;
  unsigned int res = ts3_funcs.clientPropertyStringToFlag(clientPropertyString, &resultFlag);

  return Py_BuildValue("(II)", res, (unsigned int)resultFlag);
}

PyObject* closeCaptureDevice(PyObject* /*self*/, PyObject* args) {
  //unsigned int closeCaptureDevice(uint64 serverConnectionHandlerID)
  /*
    @staticmethod
    def closeCaptureDevice(serverConnectionHandlerID):
        """
        Closes a capture device on a serverconnection.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  unsigned int res = ts3_funcs.closeCaptureDevice(schid);

  return Py_BuildValue("I", res);
}

PyObject* closePlaybackDevice(PyObject* /*self*/, PyObject* args) {
  //unsigned int closePlaybackDevice(uint64 serverConnectionHandlerID)
  /*
    @staticmethod
    def closePlaybackDevice(serverConnectionHandlerID):
        """
        Closes a playback device on a serverconnection.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  unsigned int res = ts3_funcs.closePlaybackDevice(schid);

  return Py_BuildValue("I", res);
}

PyObject* closeWaveFileHandle(PyObject* /*self*/, PyObject* args) {
  //unsigned int closeWaveFileHandle(uint64 serverConnectionHandlerID, uint64 waveHandle)
  /*
    @staticmethod
    def closeWaveFileHandle(serverConnectionHandlerID, waveHandle):
        """
        Closes a wavefile sound handle previously returned by playWaveFileHandle.
        @param serverConnectionHandlerID: the ID of the serverConnection the sound was played on
        @type serverConnectionHandlerID: int
        @param waveHandle: the handle returned by playWaveFileHandle
        @type waveHandle: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long waveHandle;

  if (!PyArg_ParseTuple(args, "KK", &schid, &waveHandle))
    return NULL;

  unsigned int res = ts3_funcs.closeWaveFileHandle((uint64)schid, (uint64)waveHandle);

  return Py_BuildValue("I", res);
}

PyObject* createBookmark(PyObject* /*self*/, PyObject* args) {
  //unsigned int createBookmark(const char *bookmarkuuid, const char *serverLabel, const char *serverAddress, const char *serverPassword, const char *nickname, const char *channel, const char *channelPassword, const char *captureProfile, const char *playbackProfile, const char *hotkeyProfile, const char *soundProfile, const char *uniqueUserId, const char *oneTimeKey, const char *phoneticName)
  /*
    @staticmethod
    def createBookmark(bookmarkuuid, serverLabel, serverAddress, serverPassword, nickname, channel, channelPassword, captureProfile, playbackProfile, hotkeyProfile, soundProfile, uniqueUserId, oneTimeKey, phoneticName):
        """
        Creates a new bookmark.
        @param bookmarkuuid: //FIXME: parent?
        @type bookmarkuuid: string
        @param serverLabel: the label of the connection
        @type serverLabel: string
        @param serverAddress: host or ip address
        @type serverAddress: string
        @param serverPassword: password to the server, pass an empty string if the server is not password protected
        @type serverPassword: string
        @param nickname: the user's nickname
        @type nickname: string
        @param channel: complete path to the channel to connect to
        @type channel: string
        @param channelPassword: password to the channel, pass an empty string if the channel is not password protected
        @type channelPassword: string
        @param captureProfile: the name of the capture profile to use; pass an empty string to always use the default one
        @type captureProfile: string
        @param playbackProfile: the name of the playback profile to use; pass an empty string to always use the default one
        @type playbackProfile: string
        @param hotkeyProfile: the name of the hotkey profile to use; pass an empty string to always use the default one
        @type hotkeyProfile: string
        @param soundProfile: the name of the sound profile to use; pass an empty string to always use the default one
        @type soundProfile: string
        @param uniqueUserId: identity (name) to use; pass an empty string to always use the default one
        @type uniqueUserId: string
        @param oneTimeKey: privilege key to use on connect
        @type oneTimeKey: string
        @param phoneticName: phonetic nickname
        @type phoneticName: string
        @return: the errorcode
        @rtype: int
        """
  */
  char* bookmarkuuid;
  char* serverLabel;
  char* serverAddress;
  char* serverPassword;
  char* nickname;
  char* channel;
  char* channelPassword;
  char* captureProfile;
  char* playbackProfile;
  char* hotkeyProfile;
  char* soundProfile;
  char* uniqueUserId;
  char* oneTimeKey;
  char* phoneticName;

  if (!PyArg_ParseTuple(args, "ssssssssssssss", &bookmarkuuid, &serverLabel, &serverAddress, &serverPassword, &nickname, &channel, &channelPassword, &captureProfile, &playbackProfile, &hotkeyProfile, &soundProfile, &uniqueUserId, &oneTimeKey, &phoneticName))
    return NULL;

  unsigned int res = ts3_funcs.createBookmark(bookmarkuuid, serverLabel, serverAddress, serverPassword, nickname, channel, channelPassword, captureProfile, playbackProfile, hotkeyProfile, soundProfile, uniqueUserId, oneTimeKey, phoneticName);

  return Py_BuildValue("I", res);
}

PyObject* createReturnCode(PyObject* /*self*/, PyObject* args) {
  //void createReturnCode(const char *pluginID, char *returnCode, size_t maxLen)
  /*
    @staticmethod
    def createReturnCode(maxLen = 128):
        """
        Creates a returnCode which can be passed to the other functions and will be passed to the event onServerErrorEvent.
        @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
        @type maxLen: int
        @return: the created returnCode
        @rtype: string
        """
  */
  unsigned int maxLen = 128;

  if (!PyArg_ParseTuple(args, "|I", &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, TRANS("maxLen must be greater than zero").toUtf8().data());
    return NULL;
  }

  char* returnCode = CPPALLOC(char, maxLen);
  ts3_funcs.createReturnCode(ts3_pluginid, returnCode, (size_t)maxLen);

  PyObject* ret = Py_BuildValue("s", returnCode);
  CPPDELARR(returnCode);

  return ret;
}

PyObject* destroyServerConnectionHandler(PyObject* /*self*/, PyObject* args) {
  //unsigned int destroyServerConnectionHandler(uint64 serverConnectionHandlerID)
  /*
    @staticmethod
    def destroyServerConnectionHandler(serverConnectionHandlerID):
        """
        Destroys a server connection handler.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: The errorcode
        @rtype: int
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  unsigned int res = ts3_funcs.destroyServerConnectionHandler((uint64)schid);

  return Py_BuildValue("I", res);
}

PyObject* flushChannelCreation(PyObject* /*self*/, PyObject* args) {
  //unsigned int flushChannelCreation(uint64 serverConnectionHandlerID, uint64 channelParentID, const char *returnCode)
  /*
    @staticmethod
    def flushChannelCreation(serverConnectionHandlerID, channelParentID, returnCode):
        """
        Flushes the channel creation made by the setChannelVariable-functions to the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelParentID: the ID of the parent channel of the new channel
        @type channelParentID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelParentID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &channelParentID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.flushChannelCreation((uint64)schid, (uint64)channelParentID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* flushChannelUpdates(PyObject* /*self*/, PyObject* args) {
  //unsigned int flushChannelUpdates(uint64 serverConnectionHandlerID, uint64 channelID, const char *returnCode)
  /*
    @staticmethod
    def flushChannelUpdates(serverConnectionHandlerID, channelID, returnCode):
        """
        Flushes the changes made by the setChannelVariable-functions to the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channelID
        @type channelID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &channelID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.flushChannelUpdates((uint64)schid, (uint64)channelID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* flushClientSelfUpdates(PyObject* /*self*/, PyObject* args) {
  //unsigned int flushClientSelfUpdates(uint64 serverConnectionHandlerID, const char *returnCode)
  /*
    @staticmethod
    def flushClientSelfUpdates(serverConnectionHandlerID, returnCode):
        """
        Flushes the changes made by the setClientSelfVariable-functions to the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "K|s", &schid, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.flushClientSelfUpdates((uint64)schid, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* getAppPath(PyObject* /*self*/, PyObject* args) {
  //void getAppPath(char *path, size_t maxLen)
  /*
    @staticmethod
    def getAppPath(maxLen = 256):
        """
        Returns the ts3 application path.
        @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
        @type maxLen: int
        @return: the application path
        @rtype: string
        """
  */
  unsigned int maxLen = 256;

  if (!PyArg_ParseTuple(args, "|I", &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, TRANS("maxLen must be greater than zero").toUtf8().data());
    return NULL;
  }

  char* path = CPPALLOC(char, maxLen);
  ts3_funcs.getAppPath(path, (size_t)maxLen);

  PyObject* ret = Py_BuildValue("s", path);
  CPPDELARR(path);

  return ret;
}

PyObject* getAvatar(PyObject* /*self*/, PyObject* args) {
  //unsigned int getAvatar(uint64 scHandlerID, anyID clientID, char *result, size_t maxLen)
  /*
    @staticmethod
    def getAvatar(serverConnectionHandlerID, clientID, maxLen = 256):
        """
        Returns the path on the system to the avatar image file of a client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
        @type maxLen: int
        @return: a tuple, containing the errorcode and the path to the avatar
        @rtype: tuple (int, string)
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  unsigned int maxLen = 256;

  if (!PyArg_ParseTuple(args, "KI|I", &schid, &clientID, &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, TRANS("maxLen must be greater than zero").toUtf8().data());
    return NULL;
  }

  char* result = CPPALLOC(char, maxLen);
  unsigned int res = ts3_funcs.getAvatar((uint64)schid, (anyID)clientID, result, (size_t)maxLen);

  PyObject* ret = Py_BuildValue("(Is)", res, result);
  CPPDELARR(result);

  return ret;
}

PyObject* getAverageTransferSpeed(PyObject* /*self*/, PyObject* args) {
  //unsigned int getAverageTransferSpeed(anyID transferID, float *result)
  /*
    @staticmethod
    def getAverageTransferSpeed(transferID):
        """
        Returns the average transfer speed of a filetransfer.
        @param transferID: the ID of the filetransfer
        @type transferID: int
        @return: a tuple, containing the errorcode and the speed
        @rtype: tuple (int, float)
        """
  */
  unsigned int transferID;

  if (!PyArg_ParseTuple(args, "I", &transferID))
    return NULL;

  float result = 0;
  unsigned int res = ts3_funcs.getAverageTransferSpeed((anyID)transferID, &result);

  return Py_BuildValue("(If)", res, result);

}

PyObject* getBookmarkList(PyObject* /*self*/, PyObject* args) {
  //unsigned int getBookmarkList(struct PluginBookmarkList **list)
  /*
    @staticmethod
    def getBookmarkList():
        """
        Returns the list of bookmarks.
        @return: a tuple, containing the errorcode and a list of tuples (name, isFolder, uid, childs)
        @rtype: tuple (int, [tuple (string, int or bool, string or None, [childs])])
        """
  */
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  struct PluginBookmarkList* list;
  unsigned int res = ts3_funcs.getBookmarkList(&list);

  PyObject* pyret;
  if (res == ERROR_ok) {
    QString error;
    PyObject* pylist;
    if (!bookmarksToPyList(list, error, &pylist)) {
      freeBookmarkList(list);
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
      return NULL;
    }

    freeBookmarkList(list);
    pyret = Py_BuildValue("(IO)", res, pylist);
    Py_DECREF(pylist);
  }
  else pyret = Py_BuildValue("(Is)", res, NULL);

  return pyret;
}

PyObject* getCaptureDeviceList(PyObject* /*self*/, PyObject* args) {
  //unsigned int getCaptureDeviceList(const char *modeID, char ****result)
  /*
    @staticmethod
    def getCaptureDeviceList(modeID):
        """
        Queries all available capture devices.
        @param modeID: Defines the capture mode to use.
        @type modeID: string
        @return: A tuple, containing the errorcode and the list of capture devices as tuple (devicename, deviceid)
        @rtype: tuple (int, [(string, string)])
        """
  */
  char* modeID;

  if (!PyArg_ParseTuple(args, "s", &modeID))
    return NULL;

  char*** result = NULL;
  unsigned int res = ts3_funcs.getCaptureDeviceList(modeID, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    QString error;
    PyObject* pyresult;
    if (!deviceArrayToPyList(result, error, &pyresult)) {
      for (int i = 0; result[i] != NULL; ++i) {
        ts3_funcs.freeMemory(result[i][0]);
        ts3_funcs.freeMemory(result[i][1]);
        ts3_funcs.freeMemory(result[i]);
      }
      ts3_funcs.freeMemory(result);
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
      return NULL;
    }

    for (int i = 0; result[i] != NULL; ++i) {
      ts3_funcs.freeMemory(result[i][0]);
      ts3_funcs.freeMemory(result[i][1]);
      ts3_funcs.freeMemory(result[i]);
    }
    ts3_funcs.freeMemory(result);

    pyret = Py_BuildValue("(IO)", res, pyresult);
    Py_DECREF(pyresult);
  }
  else pyret = Py_BuildValue("(Is)", res, NULL);

  return pyret;
}

PyObject* getCaptureModeList(PyObject* /*self*/, PyObject* args) {
  //unsigned int getCaptureModeList(char ***result)
  /*
    @staticmethod
    def getCaptureModeList():
        """
        Queries all available capture modes.
        @return: A tuple, containing the errorcode and the list of capture modes
        @rtype: tuple (int, [string])
        """
  */
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  char** result;
  unsigned int res = ts3_funcs.getCaptureModeList(&result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    QString error;
    PyObject* pyresult;
    if (!ARRAY_TO_PYLIST(char*, result, "s", error, &pyresult, 0)) {
      for (int i = 0; result[i] != NULL; ++i)
        ts3_funcs.freeMemory(result[i]);
      ts3_funcs.freeMemory(result);
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
      return NULL;
    }

    for (int i = 0; result[i] != NULL; ++i)
      ts3_funcs.freeMemory(result[i]);
    ts3_funcs.freeMemory(result);

    pyret = Py_BuildValue("(IO)", res, pyresult);
    Py_DECREF(pyresult);
  }
  else pyret = Py_BuildValue("(Is)", res, NULL);

  return pyret;
}

PyObject* getChannelClientList(PyObject* /*self*/, PyObject* args) {
  //unsigned int getChannelClientList(uint64 serverConnectionHandlerID, uint64 channelID, anyID **result)
  /*
    @staticmethod
    def getChannelClientList(serverConnectionHandlerID, channelID):
        """
        Returns all clients in a specified channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @return: a tuple, containing the errorcode and a list of client IDs or None if the call failed
        @rtype: tuple (int, [int]) or tuple(int, None)
        """
  */
  unsigned long long schid;
  unsigned long long channelID;

  if (!PyArg_ParseTuple(args, "KK", &schid, &channelID))
    return NULL;

  anyID* result;
  unsigned int res = ts3_funcs.getChannelClientList((uint64)schid, (uint64)channelID, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    PyObject* pyresult;

    QString error;
    if (!ARRAY_TO_PYLIST(anyID, result, "I", error, &pyresult, 0)) {
      ts3_funcs.freeMemory(result);
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
      return NULL;
    }
    ts3_funcs.freeMemory(result);

    pyret = Py_BuildValue("(IO)", res, pyresult);
    Py_DECREF(pyresult);
  }
  else pyret = Py_BuildValue("(Is)", res, NULL);

  return pyret;
}

PyObject* getChannelConnectInfo(PyObject* /*self*/, PyObject* args) {
  //unsigned int getChannelConnectInfo(uint64 scHandlerID, uint64 channelID, char *path, char *password, size_t maxLen)
  /*
    @staticmethod
    def getChannelConnectInfo(serverConnectionHandlerID, channelID, maxLen):
        """
        Returns the channel connect info (path and password) of a channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
        @type maxLen: int
        @return: a tuple, containing the errorcode, the path and the password of a channel
        @rtype: tuple (int, string, string)
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  unsigned int maxLen = 256;

  if (!PyArg_ParseTuple(args, "KK|I", &schid, &channelID, &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, TRANS("maxLen must be greater than zero").toUtf8().data());
    return NULL;
  }

  char* path = CPPALLOC(char, maxLen);
  char* password = CPPALLOC(char, maxLen);
  unsigned int res = ts3_funcs.getChannelConnectInfo((uint64)schid, (uint64)channelID, path, password, (size_t)maxLen);

  PyObject* ret = Py_BuildValue("(Iss)", res, path, password);
  CPPDELARR(path);
  CPPDELARR(password);

  return ret;
}

PyObject* getChannelIDFromChannelNames(PyObject* /*self*/, PyObject* args) {
  //unsigned int getChannelIDFromChannelNames(uint64 serverConnectionHandlerID, char **channelNameArray, uint64 *result)
  /*
    @staticmethod
    def getChannelIDFromChannelNames(serverConnectionHandlerID, channelNameArray):
        """
        Returns the ID of a channel defined by its name.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelNameArray: list of strings, defining the position of the channel (['grandparent', 'parent', 'channel'])
        @type channelNameArray: list [string]
        @return: a tuple, containing the errorcode and the ID of the channel
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  PyObject* pychannelNameArray;

  if (!PyArg_ParseTuple(args, "KO", &schid, &pychannelNameArray))
    return NULL;

  QString error;
  char** channelNameArray;
  if (!PYLIST_TO_ARRAY(char*, pychannelNameArray, error, &channelNameArray, true)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  uint64 result = 0;
  unsigned int res = ts3_funcs.getChannelIDFromChannelNames((uint64)schid, channelNameArray, &result);
  for (int i = 0; channelNameArray[i] != NULL; ++i)
    free(channelNameArray);
  free(channelNameArray);

  return Py_BuildValue("(IK)", res, result);
}

PyObject* getChannelList(PyObject* /*self*/, PyObject* args) {
  //unsigned int getChannelList(uint64 serverConnectionHandlerID, uint64 **result)
  /*
    @staticmethod
    def getChannelList(serverConnectionHandlerID):
        """
        Returns all channels on the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: a tuple, containing the errorcode and a list of channel IDs
        @rtype: tuple (int, [int])
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  uint64* result;
  unsigned int res = ts3_funcs.getChannelList((uint64)schid, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    PyObject* pyresult;
    QString error;
    if (!ARRAY_TO_PYLIST(uint64, result, "K", error, &pyresult, 0)) {
      ts3_funcs.freeMemory(result);
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
      return NULL;
    }
    ts3_funcs.freeMemory(result);

    pyret = Py_BuildValue("(IO)", res, pyresult);
    Py_DECREF(pyresult);
  }
  else  pyret = Py_BuildValue("(Is)", res, NULL);

  return pyret;
}

PyObject* getChannelOfClient(PyObject* /*self*/, PyObject* args) {
  //unsigned int getChannelOfClient(uint64 serverConnectionHandlerID, anyID clientID, uint64 *result)
  /*
    @staticmethod
    def getChannelOfClient(serverConnectionHandlerID, clientID):
        """
        Returns the channel of a client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @return: a tuple, containing the errorcode and the channel
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned int clientID;

  if (!PyArg_ParseTuple(args, "KI", &schid, &clientID))
    return NULL;

  uint64 result;
  unsigned int res = ts3_funcs.getChannelOfClient((uint64)schid, (anyID)clientID, &result);

  return Py_BuildValue("(IK)", res, result);
}

PyObject* getChannelVariableAsInt(PyObject* /*self*/, PyObject* args) {
  //unsigned int getChannelVariableAsInt(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, int *result)
  /*
    @staticmethod
    def getChannelVariableAsInt(serverConnectionHandlerID, channelID, flag):
        """
        Returns a channel variable as int value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param flag: the flag to return
        @type flag: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KKI", &schid, &channelID, &flag))
    return NULL;

  int result;
  unsigned int res = ts3_funcs.getChannelVariableAsInt((uint64)schid, (uint64)channelID, (size_t)flag, &result);

  return Py_BuildValue("(IK)", res, result);
}

PyObject* getChannelVariableAsString(PyObject* /*self*/, PyObject* args) {
  //unsigned int getChannelVariableAsString(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, char **result)
  /*
    @staticmethod
    def getChannelVariableAsString(serverConnectionHandlerID, channelID, flag):
        """
        Returns a channel variable as string value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param flag: the flag to return
        @type flag: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KKI", &schid, &channelID, &flag))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getChannelVariableAsString((uint64)schid, (uint64)channelID, (size_t)flag, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getChannelVariableAsUInt64(PyObject* /*self*/, PyObject* args) {
  //unsigned int getChannelVariableAsUInt64(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, uint64 *result)
  /*
    @staticmethod
    def getChannelVariableAsUInt64(serverConnectionHandlerID, channelID, flag):
        """
        Returns a channel variable as unsigned long long int value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param flag: the flag to return
        @type flag: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KKI", &schid, &channelID, &flag))
    return NULL;

  uint64 result;
  unsigned int res = ts3_funcs.getChannelVariableAsUInt64((uint64)schid, (uint64)channelID, (size_t)flag, &result);

  return Py_BuildValue("(IK)", res, result);
}

PyObject* getClientDisplayName(PyObject* /*self*/, PyObject* args) {
  //unsigned int getClientDisplayName(uint64 scHandlerID, anyID clientID, char *result, size_t maxLen)
  /*
    @staticmethod
    def getClientDisplayName(serverConnectionHandlerID, clientID, maxLen = 128):
        """
        Returns the client display name receiving from the client's contacts settings.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 128
        @type maxLen: int
        @return: a tuple, containing the errorcode and the display name
        @rtype: tuple (int, string)
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  unsigned int maxLen = 128;

  if (!PyArg_ParseTuple(args, "KI|I", &schid, &clientID, &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, TRANS("maxLen must be greater than zero").toUtf8().data());
    return NULL;
  }

  char* result = CPPALLOC(char, maxLen);
  unsigned int res = ts3_funcs.getClientDisplayName((uint64)schid, (anyID)clientID, result, maxLen);

  PyObject* ret = Py_BuildValue("(Is)", res, result);
  CPPDELARR(result);

  return ret;
}

PyObject* getClientID(PyObject* /*self*/, PyObject* args) {
  //unsigned int getClientID(uint64 serverConnectionHandlerID, anyID *result)
  /*
    @staticmethod
    def getClientID(serverConnectionHandlerID):
        """
        Returns the own client ID on a given serverconnection.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: a tuple, containing the errorcode and the client ID
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  anyID result = 0;
  unsigned int res = ts3_funcs.getClientID((uint64)schid, &result);

  return Py_BuildValue("(II)", res, (unsigned int)result);
}

PyObject* getClientLibVersion(PyObject* /*self*/, PyObject* args) {
  //unsigned int getClientLibVersion(char **result)
  /*
    @staticmethod
    def getClientLibVersion():
        """
        Returns the clientlib's version as string.
        @return: A tuple, containing the errorcode and the result
        @rtype: tuple (int, string)
        """
  */
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getClientLibVersion(&result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getClientLibVersionNumber(PyObject* /*self*/, PyObject* args) {
  //unsigned int getClientLibVersionNumber(uint64 *result)
  /*
    @staticmethod
    def getClientLibVersionNumber():
        """
        Returns the clientlib's version number
        @return: A tuple, containing the errorcode and the result
        @rtype: tuple (int, int)
        """
   */
  uint64 result;

  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  unsigned int res = ts3_funcs.getClientLibVersionNumber(&result);

  return Py_BuildValue("(IK)", res, (unsigned long long)result);
}

PyObject* getClientList(PyObject* /*self*/, PyObject* args) {
  //unsigned int getClientList(uint64 serverConnectionHandlerID, anyID **result)
  /*
    @staticmethod
    def getClientList(serverConnectionHandlerID):
        """
        Returns all clients in view on the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: a tuple, containing the errorcode and the list of client IDs
        @rtype: tuple (int, [int])
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  anyID* result;
  unsigned int res = ts3_funcs.getClientList((uint64)schid, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    PyObject* pyresult;
    QString error;

    if (!ARRAY_TO_PYLIST(anyID, result, "I", error, &pyresult, 0)) {
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
      ts3_funcs.freeMemory(result);
      return NULL;
    }
    ts3_funcs.freeMemory(result);

    pyret = Py_BuildValue("(IO)", res, pyresult);
    Py_DECREF(pyresult);
  }
  else pyret = Py_BuildValue("(Is)", res, NULL);

  return pyret;
}

PyObject* getClientNeededPermission(PyObject* /*self*/, PyObject* args) {
  //unsigned int getClientNeededPermission(uint64 serverConnectionHandlerID, const char *permissionName, int *result)
  /*
    @staticmethod
    def getClientNeededPermission(serverConnectionHandlerID, permissionName):
        """
        Returns the value of the client's needed permission.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param permissionName: name of the permission
        @type permissionName: string
        @return: a tuple, containing the errorcode and the value of the permission
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  char* permissionName;

  if (!PyArg_ParseTuple(args, "Ks", &schid, &permissionName))
    return NULL;

  int result;
  unsigned int res = ts3_funcs.getClientNeededPermission((uint64)schid, permissionName, &result);

  return Py_BuildValue("(Ii)", res, result);
}

PyObject* getClientSelfVariableAsInt(PyObject* /*self*/, PyObject* args) {
  //unsigned int getClientSelfVariableAsInt(uint64 serverConnectionHandlerID, size_t flag, int *result)
  /*
    @staticmethod
    def getClientSelfVariableAsInt(serverConnectionHandlerID, flag):
        """
        Returns the value of a given flag of the own client as int value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param flag: the flag to return
        @type flag: int
        @return: a tuple, containing the errorcode and the value of the queried flag
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KI", &schid, &flag))
    return NULL;

  int result = 0;
  unsigned int res = ts3_funcs.getClientSelfVariableAsInt((uint64)schid, (size_t)flag, &result);

  return Py_BuildValue("(Ii)", res, result);
}

PyObject* getClientSelfVariableAsString(PyObject* /*self*/, PyObject* args) {
  //unsigned int getClientSelfVariableAsString(uint64 serverConnectionHandlerID, size_t flag, char **result)
  /*
    @staticmethod
    def getClientSelfVariableAsString(serverConnectionHandlerID, flag):
        """
        Returns the value of a given flag of the own client as string.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param flag: the flag to return
        @type flag: int
        @return: a tuple, containing the errorcode and the value of the queried flag
        @rtype: tuple (int, string)
        """
  */
  unsigned long long schid;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KI", &schid, &flag))
    return NULL;

  char* result = NULL;
  unsigned int res = ts3_funcs.getClientSelfVariableAsString((uint64)schid, (size_t)flag, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getClientVariableAsInt(PyObject* /*self*/, PyObject* args) {
  //unsigned int getClientVariableAsInt(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, int *result)
  /*
    @staticmethod
    def getClientVariableAsInt(serverConnectionHandlerID, clientID, flag):
        """
        Returns the value of a given flag of a client as int value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @param flag: the flag to return
        @type flag: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KII", &schid, &clientID, &flag))
    return NULL;

  int result;
  unsigned int res = ts3_funcs.getClientVariableAsInt((uint64)schid, (anyID)clientID, (size_t)flag, &result);

  return Py_BuildValue("(Ii)", res, result);
}

PyObject* getClientVariableAsString(PyObject* /*self*/, PyObject* args) {
  //unsigned int getClientVariableAsString(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, char **result)
  /*
    @staticmethod
    def getClientVariableAsString(serverConnectionHandlerID, clientID, flag):
        """
        Returns the value of a given flag of a client as string value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @param flag: the flag to return
        @type flag: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, string)
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KII", &schid, &clientID, &flag))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getClientVariableAsString((uint64)schid, (anyID)clientID, (size_t)flag, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getClientVariableAsUInt64(PyObject* /*self*/, PyObject* args) {
  //unsigned int getClientVariableAsUInt64(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, uint64 *result)
  /*
    @staticmethod
    def getClientVariableAsUInt64(serverConnectionHandlerID, clientID, flag):
        """
        Returns the value of a given flag of a client as unsigned long long int value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @param flag: the flag to return
        @type flag: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KII", &schid, &clientID, &flag))
    return NULL;

  uint64 result;
  unsigned int res = ts3_funcs.getClientVariableAsUInt64((uint64)schid, (anyID)clientID, (size_t)flag, &result);

  return Py_BuildValue("(IK)", res, (unsigned long long)result);
}

PyObject* getConfigPath(PyObject* /*self*/, PyObject* args) {
  //void getConfigPath(char *path, size_t maxLen)
  /*
    @staticmethod
    def getConfigPath(maxLen = 256):
        """
        Returns the ts3 config path.
        @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
        @type maxLen: int
        @return: the config path
        @rtype: string
        """
  */
  unsigned int maxLen = 256;

  if (!PyArg_ParseTuple(args, "|I", &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, TRANS("maxLen must be greater than zero").toUtf8().data());
    return NULL;
  }

  char* path = CPPALLOC(char, maxLen);
  ts3_funcs.getConfigPath(path, (size_t)maxLen);

  PyObject* ret = Py_BuildValue("s", path);
  CPPDELARR(path);

  return ret;
}

PyObject* getConnectionStatus(PyObject* /*self*/, PyObject* args) {
  //unsigned int getConnectionStatus(uint64 serverConnectionHandlerID, int *result)
  /*
    @staticmethod
    def getConnectionStatus(serverConnectionHandlerID):
        """
        Returns the current connection status of a serverconnection.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: a tuple, containing the errorcode and the connection status
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  int result = 0;
  unsigned int res = ts3_funcs.getConnectionStatus((uint64)schid, &result);

  return Py_BuildValue("(Ii)", res, result);
}

PyObject* getConnectionVariableAsDouble(PyObject* /*self*/, PyObject* args) {
  //unsigned int getConnectionVariableAsDouble(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, double *result)
  /*
    @staticmethod
    def getConnectionVariableAsDouble(serverConnectionHandlerID, clientID, flag):
        """
        Returns a client's connection variable as python floating point variable.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, double)
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KII", &schid, &clientID, &flag))
    return NULL;

  double result;
  unsigned int res = ts3_funcs.getConnectionVariableAsDouble((uint64)schid, (anyID)clientID, (size_t)flag, &result);

  return Py_BuildValue("(Id)", res, result);
}

PyObject* getConnectionVariableAsString(PyObject* /*self*/, PyObject* args) {
  //unsigned int getConnectionVariableAsString(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, char **result)
  /*
    @staticmethod
    def getConnectionVariableAsString(serverConnectionHandlerID, clientID, flag):
        """
        Returns a client's connection variable as string variable.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, string)
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KII", &schid, &clientID, &flag))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getConnectionVariableAsString((uint64)schid, (anyID)clientID, (size_t)flag, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getConnectionVariableAsUInt64(PyObject* /*self*/, PyObject* args) {
  //unsigned int getConnectionVariableAsUInt64(uint64 serverConnectionHandlerID, anyID clientID, size_t flag, uint64 *result)
  /*
    @staticmethod
    def getConnectionVariableAsUInt64(serverConnectionHandlerID, clientID, flag):
        """
        Returns a client's connection variable as unsigned long long int variable.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KII", &schid, &clientID, &flag))
    return NULL;

  uint64 result;
  unsigned int res = ts3_funcs.getConnectionVariableAsUInt64((uint64)schid, (anyID)clientID, (size_t)flag, &result);

  return Py_BuildValue("(IK)", res, result);
}

PyObject* getCurrentCaptureDeviceName(PyObject* /*self*/, PyObject* args) {
  //unsigned int getCurrentCaptureDeviceName(uint64 serverConnectionHandlerID, char **result, int *isDefault)
  /*
    @staticmethod
    def getCurrentCaptureDeviceName(serverConnectionHandlerID):
        """
        Queries the current playback device's name on a serverconnection.
        @param serverConnectionHandlerID: ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: A tuple, containing the errorcode, the capture device's name and the status, if it's default
        @rtype: tuple (int, string, int)
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  char* result;
  int isDefault = 0;
  unsigned int res = ts3_funcs.getCurrentCaptureDeviceName((uint64)schid, &result, &isDefault);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Isi)", res, result, isDefault);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Isi)", res, "", isDefault);

  return pyret;
}

PyObject* getCurrentCaptureMode(PyObject* /*self*/, PyObject* args) {
  //unsigned int getCurrentCaptureMode(uint64 serverConnectionHandlerID, char **result)
  /*
    @staticmethod
    def getCurrentCaptureMode(serverConnectionHandlerID):
        """
        Queries the current capture mode on a serverconnection.
        @param serverConnectionHandlerID: ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: A tuple, containing the errorcode and the current capture mode
        @rtype: tuple (int, string)
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getCurrentCaptureMode((uint64)schid, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getCurrentPlaybackDeviceName(PyObject* /*self*/, PyObject* args) {
  //unsigned int getCurrentPlaybackDeviceName(uint64 serverConnectionHandlerID, char **result, int *isDefault)
  /*
    @staticmethod
    def getCurrentPlaybackDeviceName(serverConnectionHandlerID):
        """
        Queries the current playback device's name on a serverconnection.
        @param serverConnectionHandlerID: ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: A tuple, containing the errorcode, the playback device's name and the status, if it's default
        @rtype: tuple (int, string, int)
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  char* result;
  int isDefault = 0;
  unsigned int res = ts3_funcs.getCurrentPlaybackDeviceName((uint64)schid, &result, &isDefault);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Isi)", res, result, isDefault);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Isi)", res, "", isDefault);

  return pyret;
}

PyObject* getCurrentPlayBackMode(PyObject* /*self*/, PyObject* args) {
  //unsigned int getCurrentPlayBackMode(uint64 serverConnectionHandlerID, char **result)
  /*
    @staticmethod
    def getCurrentPlayBackMode(serverConnectionHandlerID):
        """
        Queries the current playback mode on a serverconnection.
        @param serverConnectionHandlerID: ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: A tuple, containing the errorcode and the current playback mode
        @rtype: tuple (int, string)
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getCurrentPlayBackMode(schid, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getCurrentServerConnectionHandlerID(PyObject* /*self*/, PyObject* args) {
  //uint64 getCurrentServerConnectionHandlerID()
  /*
    @staticmethod
    def getCurrentServerConnectionHandlerID():
        """
        Returns the current serverconnection handler.
        @return: the ID of the current serverconnection handler
        @rtype: int
        """
  */
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  return Py_BuildValue("K", (unsigned long long)ts3_funcs.getCurrentServerConnectionHandlerID());
}

PyObject* getCurrentTransferSpeed(PyObject* /*self*/, PyObject* args) {
  //unsigned int getCurrentTransferSpeed(anyID transferID, float *result)
  /*
    @staticmethod
    def getCurrentTransferSpeed(transferID):
        """
        Returns the current transfer speed of a filetransfer.
        @param transferID: the ID of the filetransfer
        @type transferID: int
        @return: a tuple, containing the errorcode and the speed
        @rtype: tuple (int, float)
        """
  */
  unsigned int transferID;

  if (!PyArg_ParseTuple(args, "I", &transferID))
    return NULL;

  float result = 0;
  unsigned int res = ts3_funcs.getCurrentTransferSpeed((anyID)transferID, &result);

  return Py_BuildValue("(If)", res, result);
}

PyObject* getDefaultCaptureDevice(PyObject* /*self*/, PyObject* args) {
  //unsigned int getDefaultCaptureDevice(const char *modeID, char ***result)
  /*
    @staticmethod
    def getDefaultCaptureDevice(modeID):
        """
        Queries the default capture device.
        @param modeID: Defines the capture mode to use
        @type modeID: string
        @return: A tuple, containing the errorcode and the default capture device as tuple (devicename, deviceid)
        @rtype: tuple (int, (string, string))
        """
  */
  char* modeID;

  if (!PyArg_ParseTuple(args, "s", &modeID))
    return NULL;

  char** result;
  unsigned int res = ts3_funcs.getDefaultCaptureDevice(modeID, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(I(ss))", res, result[0], result[1]);
    ts3_funcs.freeMemory(result[0]);
    ts3_funcs.freeMemory(result[1]);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(I(ss))", res, "", "");

  return pyret;
}

PyObject* getDefaultCaptureMode(PyObject* /*self*/, PyObject* args) {
  //unsigned int getDefaultCaptureMode(char **result)
  /*
    @staticmethod
    def getDefaultCaptureMode():
        """
        Queries the default capture mode.
        @return: A tuple, containing the errorcode and the default capture mode
        @rtype: tuple (int, string)
        """
  */
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getDefaultCaptureMode(&result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getDefaultPlaybackDevice(PyObject* /*self*/, PyObject* args) {
  //unsigned int getDefaultPlaybackDevice(const char *modeID, char ***result)
  /*
    @staticmethod
    def getDefaultPlaybackDevice(modeID):
        """
        Queries the default playback device.
        @param modeID: Defines the playback mode to use
        @type modeID: string
        @return: A tuple, containing the errorcode and the default playback device as tuple (devicename, deviceid)
        @rtype: tuple (int, (string, string))
        """
  */
  char* modeID;

  if (!PyArg_ParseTuple(args, "s", &modeID))
    return NULL;

  char** result;
  unsigned int res = ts3_funcs.getDefaultPlaybackDevice(modeID, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(I(ss))", res, result[0], result[1]);
    ts3_funcs.freeMemory(result[0]);
    ts3_funcs.freeMemory(result[1]);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(I(ss))", res, "", "");

  return pyret;
}

PyObject* getDefaultPlayBackMode(PyObject* /*self*/, PyObject* args) {
  //unsigned int getDefaultPlayBackMode(char **result)
  /*
    @staticmethod
    def getDefaultPlayBackMode():
        """
        Queries the default playback mode.
        @return: A tuple, containing the errorcode and the default playback mode
        @rtype: tuple (int, string)
        """
  */
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getDefaultPlayBackMode(&result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getDirectories(PyObject* /*self*/, PyObject* args) {
  //void getDirectories(const char *path, char *result, size_t maxLen)
  /*
    @staticmethod
    def getDirectories(path, maxLen = 256):
        """
        Returns a list of subdirectories of a path as space-separated string.
        @param path: the parent path
        @type path: str
        @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
        @type maxLen: int
        @return: the resulting path
        @rtype: string
        """
  */
  char* path;
  unsigned int maxLen = 256;

  if (!PyArg_ParseTuple(args, "s|I", &path, &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, TRANS("maxLen must be greater than zero").toUtf8().data());
    return NULL;
  }

  char* result = CPPALLOC(char, maxLen);
  ts3_funcs.getDirectories(path, result, (size_t)maxLen);

  PyObject* ret = Py_BuildValue("s", result);
  CPPDELARR(result);

  return ret;
}

PyObject* getEncodeConfigValue(PyObject* /*self*/, PyObject* args) {
  //unsigned int getEncodeConfigValue(uint64 serverConnectionHandlerID, const char *ident, char **result)
  /*
    @staticmethod
    def getEncodeConfigValue(serverConnectionHandlerID, ident):
        """
        Queries a speex encoder option.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param ident: the encoder option to be queried
        @type ident: string
        @return: A tuple, containing the errorcode and the flag's value
        @rtype: tuple (int, string)
        """
  */
  unsigned long long schid;
  char* ident;

  if (!PyArg_ParseTuple(args, "Ks", &schid, &ident))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getEncodeConfigValue((uint64)schid, ident, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getErrorMessage(PyObject* /*self*/, PyObject* args) {
  //unsigned int getErrorMessage(unsigned int errorCode, char **error)
  /*
    @staticmethod
    def getErrorMessage(errorCode):
        """
        Queries a printable error string for a specific error code.
        @param errorCode: The error code returned from all Client Lib functions
        @type errorCode: int
        @return: A tuple, containing the errorcode and the resulting string
        @rtype: tuple (int, string)
        """
  */
  char* error;
  int errorCode;

  if (!PyArg_ParseTuple(args, "i", &errorCode))
    return NULL;

  unsigned int res = ts3_funcs.getErrorMessage((unsigned int)errorCode, &error);

  PyObject* pyret = Py_BuildValue("(Is)", res, error);
  if (res == ERROR_ok)
    ts3_funcs.freeMemory(error);

  return pyret;
}

PyObject* getHotkeyFromKeyword(PyObject* /*self*/, PyObject* args) {
  //unsigned int getHotkeyFromKeyword(const char *pluginID, const char **keywords, char **hotkeys, size_t arrayLen, size_t hotkeyBufSize)
  /*
    @staticmethod
    def getHotkeyFromKeyword(keywords):
        """

        @param keywords:
        @type keywords:
        @return:
        @rtype:
        """
  */
  PyObject* pykeywords;
  if (!PyArg_ParseTuple(args, "O", &pykeywords))
    return NULL;

  QString error;
  char** keywords;
  if (!PYLIST_TO_ARRAY(char*, pykeywords, error, &keywords, true)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  char** hotkeys = (char**)malloc(sizeof(char*) * PyList_Size(pykeywords));
  for (int i = 0; i < PyList_Size(pykeywords); ++i)
    hotkeys[i] = (char*)malloc(sizeof(char) * PLUGIN_HOTKEY_BUFSZ);

  unsigned res = ts3_funcs.getHotkeyFromKeyword(ts3_pluginid, (const char**)keywords, hotkeys, PyList_Size(pykeywords), PLUGIN_HOTKEY_BUFSZ);
  for (int i = 0; i <= PyList_Size(pykeywords); ++i)
    free(keywords[i]);
  free(keywords);

  PyObject* pyret;
  if (res == ERROR_ok) {
    PyObject* pyhotkeys;
    if (!ARRAY_TO_PYLIST(char*, hotkeys, "s", error, &pyhotkeys, PyList_Size(pykeywords))) {
      for (int i = 0; i < PyList_Size(pykeywords); ++i)
        free(keywords[i]);
      free(keywords);
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
      return NULL;
    }
    for (int i = 0; i < PyList_Size(pykeywords); ++i)
      free(keywords[i]);
    free(keywords);

    pyret = Py_BuildValue("(IO)", res, pyhotkeys);
    Py_DECREF(pyhotkeys);
  }
  else pyret = Py_BuildValue("(Is)", res, NULL);

  return pyret;
}

PyObject* getParentChannelOfChannel(PyObject* /*self*/, PyObject* args) {
  //unsigned int getParentChannelOfChannel(uint64 serverConnectionHandlerID, uint64 channelID, uint64 *result)
  /*
    @staticmethod
    def getParentChannelOfChannel(serverConnectionHandlerID, channelID):
        """
        Returns the parent channel of another channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @return: a tuple, containing the errorcode and the ID of the parent channel
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned long long channelID;

  if (!PyArg_ParseTuple(args, "KK", &schid, &channelID))
    return NULL;

  uint64 result = 0;
  unsigned int res = ts3_funcs.getParentChannelOfChannel((uint64)schid, (uint64)channelID, &result);

  return Py_BuildValue("(IK)", res, result);
}

PyObject* getPermissionIDByName(PyObject* /*self*/, PyObject* args) {
  //unsigned int getPermissionIDByName(uint64 serverConnectionHandlerID, const char *permissionName, unsigned int *result)
  /*
    @staticmethod
    def getPermissionIDByName(serverConnectionHandlerID, permissionName):
        """
        Returns the ID of a permission defined by its name.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param permissionName: name of the permission
        @type permissionName: string
        @return: a tuple, containing the errorcode and the ID of the permission
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  char* permissionName;

  if (!PyArg_ParseTuple(args, "Ks", &schid, &permissionName))
    return NULL;

  unsigned int result;
  unsigned int res = ts3_funcs.getPermissionIDByName((uint64)schid, permissionName, &result);

  return Py_BuildValue("(II)", res, result);
}

PyObject* getPlaybackConfigValueAsFloat(PyObject* /*self*/, PyObject* args) {
  //unsigned int getPlaybackConfigValueAsFloat(uint64 serverConnectionHandlerID, const char *ident, float *result)
  /*
    @staticmethod
    def getPlaybackConfigValueAsFloat(serverConnectionHandlerID, ident):
        """
        Queries a playback option.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param ident: the playback option to be queried
        @type ident: string
        @return: A tuple, containing the errorcode and the flag's value
        @rtype: tuple (int, float)
        """
  */
  unsigned long long schid;
  char* ident;

  if (!PyArg_ParseTuple(args, "Ks", &schid, &ident))
    return NULL;

  float result = 0;
  unsigned int res = ts3_funcs.getPlaybackConfigValueAsFloat((uint64)schid, ident, &result);

  return Py_BuildValue("(If)", res, result);
}

PyObject* getPlaybackDeviceList(PyObject* /*self*/, PyObject* args) {
  //unsigned int getPlaybackDeviceList(const char *modeID, char ****result)
  /*
    @staticmethod
    def getPlaybackDeviceList(modeID):
        """
        Queries all available playback devices.
        @param modeID: Defines the playback mode to use.
        @type modeID: string
        @return: A tuple, containing the errorcode and the list of playback devices as tuple (devicename, deviceid)
        @rtype: tuple (int, [(string, string)])
        """
  */
  char* modeID;

  if (!PyArg_ParseTuple(args, "s", &modeID))
    return NULL;

  char*** result = NULL;
  unsigned int res = ts3_funcs.getPlaybackDeviceList(modeID, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    QString error;
    PyObject* pyresult;
    if (!deviceArrayToPyList(result, error, &pyresult)) {
      for (int i = 0; result[i] != NULL; ++i) {
        ts3_funcs.freeMemory(result[i][0]);
        ts3_funcs.freeMemory(result[i][1]);
        ts3_funcs.freeMemory(result[i]);
      }
      ts3_funcs.freeMemory(result);
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
      return NULL;
    }

    for (int i = 0; result[i] != NULL; ++i) {
      ts3_funcs.freeMemory(result[i][0]);
      ts3_funcs.freeMemory(result[i][1]);
      ts3_funcs.freeMemory(result[i]);
    }
    ts3_funcs.freeMemory(result);

    pyret = Py_BuildValue("(IO)", res, pyresult);
    Py_DECREF(pyresult);
  }
  else pyret = Py_BuildValue("(Is)", res, NULL);

  return pyret;
}

PyObject* getPlaybackModeList(PyObject* /*self*/, PyObject* args) {
  //unsigned int getPlaybackModeList(char ***result)
  /*
    @staticmethod
    def getPlaybackModeList():
        """
        Queries all available playback modes.
        @return: A tuple, containing the errorcode and the list of modes
        @rtype: tuple (int, [string])
        """
  */
  char** result;

  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  unsigned int res = ts3_funcs.getPlaybackModeList(&result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    QString error;
    PyObject* pyresult;
    if (!ARRAY_TO_PYLIST(char*, result, "s", error, &pyresult, 0)) {
      for (int i = 0; result[i] != NULL; ++i)
        ts3_funcs.freeMemory(result[i]);
      ts3_funcs.freeMemory(result);
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
      return NULL;
    }

    for (int i = 0; result[i] != NULL; ++i)
      ts3_funcs.freeMemory(result[i]);
    ts3_funcs.freeMemory(result);

    pyret = Py_BuildValue("(IO)", res, pyresult);
    Py_DECREF(pyresult);
  }
  else pyret = Py_BuildValue("Is", res, NULL);

  return pyret;
}

PyObject* getPluginPath(PyObject* /*self*/, PyObject* args) {
  //void getPluginPath(char *path, size_t maxLen, const char *pluginID)
  /*
    @staticmethod
    def getPluginPath(path, maxLen, pluginID):
        """
        
        @param path:
        @type path:
        @param maxLen:
        @type maxLen:
        @param pluginID:
        @type pluginID:
        """

  */
  unsigned int maxLen = 256;

  if (!PyArg_ParseTuple(args, "|I", &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, TRANS("maxLen must be greater than zero").toUtf8().data());
    return NULL;
  }

  char* path = CPPALLOC(char, maxLen);
  ts3_funcs.getPluginPath(path, (size_t)maxLen, ts3_pluginid);

  PyObject* ret = Py_BuildValue("s", path);
  CPPDELARR(path);

  return ret;
}

PyObject* getPreProcessorConfigValue(PyObject* /*self*/, PyObject* args) {
  //unsigned int getPreProcessorConfigValue(uint64 serverConnectionHandlerID, const char *ident, char **result)
  /*
    @staticmethod
    def getPreProcessorInfoValue(serverConnectionHandlerID, ident):
        """
        Querie a sound preprocessor flag and returns it as string.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param ident: the flag to be queried
        @type ident: string
        @return: A tuple, containing the errorcode and the value of the queried flag
        @rtype: tuple (int, string)
        """
  */
  unsigned long long schid;
  char* ident;

  if (!PyArg_ParseTuple(args, "Ks", &schid, &ident))
    return NULL;

  char* result;
  int res = ts3_funcs.getPreProcessorConfigValue((uint64)schid, ident, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getPreProcessorInfoValueFloat(PyObject* /*self*/, PyObject* args) {
  //unsigned int getPreProcessorInfoValueFloat(uint64 serverConnectionHandlerID, const char *ident, float *result)
  /*
    @staticmethod
    def getPreProcessorInfoValueFloat(serverConnectionHandlerID, ident):
        """
        Queries a sound preprocessor flag and returns it as float.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param ident: the flag to be queried
        @type ident: string
        @return: A tuple, containing the errorcode and the value of the queried flag
        @rtype: tuple (int, float)
        """
  */
  unsigned long long schid;
  char* ident;

  if (!PyArg_ParseTuple(args, "Ks", &schid, &ident))
    return NULL;

  float result = 0;
  unsigned int res = ts3_funcs.getPreProcessorInfoValueFloat((uint64)schid, ident, &result);

  return Py_BuildValue("(If)", res, result);
}

PyObject* getProfileList(PyObject* /*self*/, PyObject* args) {
  //unsigned int getProfileList(enum PluginGuiProfile profile, int *defaultProfileIdx, char ***result)
  /*
    @staticmethod
    def getProfileList(profile):
        """
        Returns a list of existing profiles and the default profile's index in list.
        @param profile: the profile type, see ts3defines.PluginGuiProfile
        @type profile: int
        @return: a tuple, containing the errorcode, the default profile's index and the profile list
        @rtype: tuple (int, int, [string])
        """
  */
  int profile;

  if (!PyArg_ParseTuple(args, "i", &profile))
    return NULL;

  int defaultProfileIdx = -1;
  char** result;

  unsigned int res = ts3_funcs.getProfileList((enum PluginGuiProfile)profile, &defaultProfileIdx, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    QString error;
    PyObject* pyresult;
    if (!ARRAY_TO_PYLIST(char*, result, "s", error, &pyresult, 0)) {
      for (int i = 0; result[i] != NULL; ++i)
        ts3_funcs.freeMemory(result[i]);
      ts3_funcs.freeMemory(result);
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
      return NULL;
    }
    for (int i = 0; result[i] != NULL; ++i)
      ts3_funcs.freeMemory(result[i]);
    ts3_funcs.freeMemory(result);

    pyret = Py_BuildValue("(IiO)", res, defaultProfileIdx, pyresult);
    Py_DECREF(pyresult);
  }
  else pyret = Py_BuildValue("(Iis)", res, NULL);

  return pyret;
}

PyObject* getResourcesPath(PyObject* /*self*/, PyObject* args) {
  //void getResourcesPath(char *path, size_t maxLen)
  /*
    @staticmethod
    def getResourcesPath(maxLen = 256):
        """
        Returns the ts3 resources path.
        @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
        @type maxLen: int
        @return: the resources path
        @rtype: string
        """
  */
  unsigned int maxLen = 256;

  if (!PyArg_ParseTuple(args, "|I", &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, TRANS("maxLen must be greater than zero").toUtf8().data());
    return NULL;
  }

  char* path = CPPALLOC(char, maxLen);
  ts3_funcs.getResourcesPath(path, (size_t)maxLen);

  PyObject* ret = Py_BuildValue("s", path);
  CPPDELARR(path);

  return ret;
}

PyObject* getServerConnectInfo(PyObject* /*self*/, PyObject* args) {
  //unsigned int getServerConnectInfo(uint64 scHandlerID, char *host, unsigned short *port, char *password, size_t maxLen)
  /*
    @staticmethod
    def getServerConnectInfo(serverConnectionHandlerID, maxLen = 256):
        """
        Returns the connect info (host, port and password) of a serverconnection.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
        @type maxLen: int
        @return: a tuple, containing the errorcode, the host, the port and the password of the serverconnection
        @rtype: tuple (int, string, int, string)
        """
  */
  unsigned long long schid;
  unsigned int maxLen = 256;

  if (!PyArg_ParseTuple(args, "K|I", &schid, &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, TRANS("maxLen must be greater than zero").toUtf8().data());
    return NULL;
  }

  char* host = CPPALLOC(char, maxLen);
  host[0] = '\0';
  unsigned short port;
  char* password = CPPALLOC(char, maxLen);
  password[0] = '\0';

  unsigned res = ts3_funcs.getServerConnectInfo((uint64)schid, host, &port, password, (size_t)maxLen);

  PyObject* ret = Py_BuildValue("(IsHs)", res, host, port, password);
  CPPDELARR(host);
  CPPDELARR(password);

  return ret;
}

PyObject* getServerConnectionHandlerList(PyObject* /*self*/, PyObject* args) {
  //unsigned int getServerConnectionHandlerList(uint64 **result)
  /*
    @staticmethod
    def getServerConnectionHandlerList():
        """
        Returns a list of serverconnection handlers.
        @return: a tuple, containing the errorcode and the list of serverconnection handler IDs
        @rtype: tuple (int, [int])
        """
  */
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  uint64* result;
  unsigned int res = ts3_funcs.getServerConnectionHandlerList(&result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    QString error;
    PyObject* pyresult;

    if (!ARRAY_TO_PYLIST(uint64, result, "K", error, &pyresult, 0)) {
      ts3_funcs.freeMemory(result);
      PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
      return NULL;
    }
    ts3_funcs.freeMemory(result);

    pyret = Py_BuildValue("(IO)", res, pyresult);
    Py_DECREF(pyresult);
  }
  else pyret = Py_BuildValue("(Is)", res, NULL);

  return pyret;
}

PyObject* getServerVariableAsInt(PyObject* /*self*/, PyObject* args) {
  //unsigned int getServerVariableAsInt(uint64 serverConnectionHandlerID, size_t flag, int *result)
  /*
    @staticmethod
    def getServerVariableAsInt(serverConnectionHandlerID, flag):
        """
        Returns a server variable as int value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param flag: the flag to return
        @type flag: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KI", &schid, &flag))
    return NULL;

  int result = 0;
  unsigned int res = ts3_funcs.getServerVariableAsInt((uint64)schid, (size_t)flag, &result);

  return Py_BuildValue("(Ii)", res, result);
}

PyObject* getServerVariableAsString(PyObject* /*self*/, PyObject* args) {
  //unsigned int getServerVariableAsString(uint64 serverConnectionHandlerID, size_t flag, char **result)
  /*
    @staticmethod
    def getServerVariableAsString(serverConnectionHandlerID, flag):
        """
        Returns a server variable as string value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param flag: the flag to return
        @type flag: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, string)
        """
  */
  unsigned long long schid;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KI", &schid, &flag))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getServerVariableAsString((uint64)schid, (size_t)flag, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getServerVariableAsUInt64(PyObject* /*self*/, PyObject* args) {
  //unsigned int getServerVariableAsUInt64(uint64 serverConnectionHandlerID, size_t flag, uint64 *result)
  /*
    @staticmethod
    def getServerVariableAsUInt64(serverConnectionHandlerID, flag):
        """
        Returns a server variable as unsigned long long int value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param flag: the flag to return
        @type flag: int
        @return: a tuple, containing the errorcode and the value of the flag
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned int flag;

  if (!PyArg_ParseTuple(args, "KI", &schid, &flag))
    return NULL;

  uint64 result = 0;
  unsigned int res = ts3_funcs.getServerVariableAsUInt64((uint64)schid, (size_t)flag, &result);

  return Py_BuildValue("(IK)", res, result);
}

PyObject* getServerVersion(PyObject* /*self*/, PyObject* args) {
  //uint64 getServerVersion(uint64 scHandlerID)
  /*
    @staticmethod
    def getServerVersion(serverConnectionHandlerID):
        """
        Returns the server version.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: the server version
        @rtype: int
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  return Py_BuildValue("K", (unsigned long long)ts3_funcs.getServerVersion((uint64)schid));
}

PyObject* getTransferFileName(PyObject* /*self*/, PyObject* args) {
  //unsigned int getTransferFileName(anyID transferID, char **result)
  /*
    @staticmethod
    def getTransferFileName(transferID):
        """
        Returns the filename of a filetransfer.
        @param transferID: the ID of the filetransfer
        @type transferID: int
        @return: a tuple, containing the errorcode and the filename
        @rtype: tuple (int, string)
        """
  */
  unsigned int transferID;

  if (!PyArg_ParseTuple(args, "I", &transferID))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getTransferFileName((anyID)transferID, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getTransferFilePath(PyObject* /*self*/, PyObject* args) {
  //unsigned int getTransferFilePath(anyID transferID, char **result)
  /*
    @staticmethod
    def getTransferFilePath(transferID):
        """
        Returns the filepath of a filetransfer.
        @param transferID: the ID of the filetransfer
        @type transferID: int
        @return: a tuple, containing the errorcode and the filepath
        @rtype: tuple (int, string)
        """
  */
  unsigned int transferID;

  if (!PyArg_ParseTuple(args, "I", &transferID))
    return NULL;

  char* result;
  unsigned int res = ts3_funcs.getTransferFilePath((anyID)transferID, &result);

  PyObject* pyret;
  if (res == ERROR_ok) {
    pyret = Py_BuildValue("(Is)", res, result);
    ts3_funcs.freeMemory(result);
  }
  else pyret = Py_BuildValue("(Is)", res, "");

  return pyret;
}

PyObject* getTransferFileSize(PyObject* /*self*/, PyObject* args) {
  //unsigned int getTransferFileSize(anyID transferID, uint64 *result)
  /*
    @staticmethod
    def getTransferFileSize(transferID):
        """
        Returns the total filesize (in Bytes) of a filetransfer.
        @param transferID: the ID of the filetransfer
        @type transferID: int
        @return: a tuple, containing the errorcode and the filesize
        @rtype: tuple (int, int)
        """
  */
  unsigned int transferID;

  if (!PyArg_ParseTuple(args, "I", &transferID))
    return NULL;

  uint64 result = 0;
  unsigned int res = ts3_funcs.getTransferFileSize((anyID)transferID, &result);

  return Py_BuildValue("(IK)", res, result);
}

PyObject* getTransferFileSizeDone(PyObject* /*self*/, PyObject* args) {
  //unsigned int getTransferFileSizeDone(anyID transferID, uint64 *result)
  /*
    @staticmethod
    def getTransferFileSizeDone(transferID):
        """
        Returns the already downloaded size (in Bytes) of a filetransfer.
        @param transferID: the ID of the filetransfer
        @type transferID: int
        @return: a tuple, containing the errorcode and the size
        @rtype: tuple (int, int)
        """
  */
  unsigned int transferID;

  if (!PyArg_ParseTuple(args, "I", &transferID))
    return NULL;

  uint64 result = 0;
  unsigned int res = ts3_funcs.getTransferFileSizeDone((anyID)transferID, &result);

  return Py_BuildValue("(IK)", res, result);
}

PyObject* getTransferRunTime(PyObject* /*self*/, PyObject* args) {
  //unsigned int getTransferRunTime(anyID transferID, uint64 *result)
  /*
    @staticmethod
    def getTransferRunTime(transferID):
        """
        Returns the runtime of a filetransfer.
        @param transferID: the ID of the filetransfer
        @type transferID: int
        @return: a tuple, containing the errorcode and the runtime in seconds
        @rtype: tuple (int, int)
        """
  */
  unsigned int transferID;

  if (!PyArg_ParseTuple(args, "I", &transferID))
    return NULL;

  uint64 result = 0;
  unsigned int res = ts3_funcs.getTransferRunTime((anyID)transferID, &result);

  return Py_BuildValue("(IK)", res, result);
}

PyObject* getTransferStatus(PyObject* /*self*/, PyObject* args) {
  //unsigned int getTransferStatus(anyID transferID, int *result)
  /*
    @staticmethod
    def getTransferStatus(transferID):
        """
        Returns the status of a filetransfer, whether if it is initialising, active or finished see ts3defines.FileTransferState
        @param transferID: the ID of the filetransfer
        @type transferID: int
        @return: a tuple, containing the errorcode and the status
        @rtype: tuple (int, int)
        """
  */
  unsigned int transferID;

  if (!PyArg_ParseTuple(args, "I", &transferID))
    return NULL;

  int result = 0;
  unsigned int res = ts3_funcs.getTransferStatus((anyID)transferID, &result);

  return Py_BuildValue("(Ii)", res, result);

}

PyObject* guiConnect(PyObject* /*self*/, PyObject* args) {
  //unsigned int guiConnect(enum PluginConnectTab connectTab, const char *serverLabel, const char *serverAddress, const char *serverPassword, const char *nickname, const char *channel, const char *channelPassword, const char *captureProfile, const char *playbackProfile, const char *hotkeyProfile, const char *soundProfile, const char *userIdentity, const char *oneTimeKey, const char *phoneticName, uint64 *scHandlerID)
  /*
    @staticmethod
    def guiConnect(connectTab, serverLabel, serverAddress, serverPassword, nickname, channel, channelPassword, captureProfile, playbackProfile, hotkeyProfile, userIdentity, oneTimeKey, phoneticName):
        """
        Connects to a server and displays it as tab in the client.
        @param connectTab: defines, which tab will be used, see ts3defines.PluginConnectTab
        @type connectTab: int
        @param serverLabel: the label of the connection
        @type serverLabel: string
        @param serverAddress: host or ip address
        @type serverAddress: string
        @param serverPassword: password to the server, pass an empty string if the server is not password protected
        @type serverPassword: string
        @param nickname: the user's nickname
        @type nickname: string
        @param channel: complete path to the channel to connect to
        @type channel: string
        @param channelPassword: password to the channel, pass an empty string if the channel is not password protected
        @type channelPassword: string
        @param captureProfile: the name of the capture profile to use
        @type captureProfile: string
        @param playbackProfile: the name of the playback profile to use
        @type playbackProfile: string
        @param hotkeyProfile: the name of the hotkey profile to use
        @type hotkeyProfile: string
        @param userIdentity: identity to use
        @type userIdentity: string
        @param oneTimeKey: privilege key to use on connect
        @type oneTimeKey: string
        @param phoneticName: phonetic nickname
        @type phoneticName: string
        @return: a tuple, containing the errorcode and the ID of the created serverconnection handler
        @rtype: tuple (int, int)
        """
  */
  int connectTab;
  char* serverLabel;
  char* serverAddress;
  char* serverPassword;
  char* nickname;
  char* channel;
  char* channelPassword;
  char* captureProfile;
  char* playbackProfile;
  char* hotkeyProfile;
  char* soundProfile;
  char* userIdentity;
  char* oneTimeKey;
  char* phoneticName;

  if (!PyArg_ParseTuple(args, "isssssssssssss", &connectTab, &serverLabel, &serverAddress, &serverPassword, &nickname, &channel, &channelPassword, &captureProfile, &playbackProfile, &hotkeyProfile, &soundProfile, &userIdentity, &oneTimeKey, &phoneticName))
    return NULL;

  uint64 schid;
  unsigned int res = ts3_funcs.guiConnect((enum PluginConnectTab)connectTab, serverLabel, serverAddress, serverPassword, nickname, channel, channelPassword, captureProfile, playbackProfile, hotkeyProfile, soundProfile, userIdentity, oneTimeKey, phoneticName, &schid);

  return Py_BuildValue("(IK)", res, schid);
}

PyObject* guiConnectBookmark(PyObject* /*self*/, PyObject* args) {
  //unsigned int guiConnectBookmark(enum PluginConnectTab connectTab, const char *bookmarkuuid, uint64 *scHandlerID)
  /*
    @staticmethod
    def guiConnectBookmark(connectTab, bookmarkuuid):
        """
        Connects to a server from a bookmark and displays it as tab in the client.
        @param connectTab: defines, which tab will be used, see ts3defines.PluginConnectTab
        @type connectTab: int
        @param bookmarkuuid: UID of the bookmark
        @type bookmarkuuid: string
        @return: a tuple, containing the errorcode and the ID of the created serverconnection handler
        @rtype: tuple (int, int)
        """
  */
  int connectTab;
  char* bookmarkuuid;

  if (!PyArg_ParseTuple(args, "is", &connectTab, &bookmarkuuid))
    return NULL;

  uint64 schid;
  unsigned int res = ts3_funcs.guiConnectBookmark((enum PluginConnectTab)connectTab, bookmarkuuid, &schid);

  return Py_BuildValue("(IK)", res, schid);
}

PyObject* haltTransfer(PyObject* /*self*/, PyObject* args) {
  //unsigned int haltTransfer(uint64 serverConnectionHandlerID, anyID transferID, int deleteUnfinishedFile, const char *returnCode)
  /*
    @staticmethod
    def haltTransfer(serverConnectionHandlerID, transferID, deleteUnfinishedFile, returnCode):
        """
        Halts a currently running filetransfer.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param transferID: the ID of the filetransfer
        @type transferID: int
        @param deleteUnfinishedFile: if set to 1 (or True) and the file is not yet finished, it will be deleted; to prevent, pass 0 (or False)
        @type deleteUnfinishedFile: int or bool
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return:
        @rtype:
        """
  */
  unsigned long long schid;
  unsigned int transferID;
  int deleteUnfinishedFile;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KIi|s", &schid, &transferID, &deleteUnfinishedFile, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.haltTransfer((uint64)schid, (anyID)transferID, deleteUnfinishedFile, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* initiateGracefulPlaybackShutdown(PyObject* /*self*/, PyObject* args) {
  //unsigned int initiateGracefulPlaybackShutdown(uint64 serverConnectionHandlerID)
  /*
    @staticmethod
    def initiateGracefulPlaybackShutdown(serverConnectionHandlerID):
        """
        Graceful shutdown the playback device on a serverconnection.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: the errorcode
        @rtype: int
        """
  */

  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  unsigned int res = ts3_funcs.initiateGracefulPlaybackShutdown((uint64)schid);

  return Py_BuildValue("I", res);
}

PyObject* isReceivingWhisper(PyObject* /*self*/, PyObject* args) {
  //unsigned int isReceivingWhisper(uint64 scHandlerID, anyID clientID, int *result)
  /*
    @staticmethod
    def isReceivingWhisper(serverConnectionHandlerID, clientID):
        """
        //FIXME:
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @return: a tuple, containing the errorcode and the status
        @rtype: tuple (int, int or bool)
        """
  */
  unsigned long long schid;
  unsigned int clientID;

  if (!PyArg_ParseTuple(args, "KI", &schid, &clientID))
    return NULL;

  int result = 0;
  unsigned int res = ts3_funcs.isReceivingWhisper((uint64)schid, (anyID)clientID, &result);

  return Py_BuildValue("(Ii)", res, result);

}

PyObject* isTransferSender(PyObject* /*self*/, PyObject* args) {
  //unsigned int isTransferSender(anyID transferID, int *result)
  /*
    @staticmethod
    def isTransferSender(transferID):
        """
        Returns the upload/download direction of a filetransfer.
        @param transferID: the ID of the filetransfer
        @type transferID: int
        @return: a tuple, containing the errorcode and 1 if it's an upload or 0 if it's a download
        @rtype: tuple (int, int or bool)
        """
  */
  unsigned int transferID;

  if (!PyArg_ParseTuple(args, "I", &transferID))
    return NULL;

  int result = 0;
  unsigned int res = ts3_funcs.isTransferSender((anyID)transferID, &result);

  return Py_BuildValue("(Ii)", res, result);
}

PyObject* isWhispering(PyObject* /*self*/, PyObject* args) {
  //unsigned int isWhispering(uint64 scHandlerID, anyID clientID, int *result)
  /*
    @staticmethod
    def isWhispering(serverConnectionHandlerID, clientID):
        """
        Returns the status of a client whether he is currently whispering to the own client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: a tuple, containing the errorcode and the status
        @rtype: tuple (int, int or bool)
        """
  */
  unsigned long long schid;
  unsigned int clientID;

  if (!PyArg_ParseTuple(args, "KI", &schid, &clientID))
    return NULL;

  int result = 0;
  unsigned int res = ts3_funcs.isWhispering((uint64)schid, (anyID)clientID, &result);

  return Py_BuildValue("(Ii)", res, result);
}

PyObject* logMessage(PyObject* /*self*/, PyObject* args) {
  //unsigned int logMessage(const char *logMessage, enum LogLevel severity, const char *channel, uint64 logID)
  /*
    @staticmethod
    def logMessage(logMessage, severity, channel, logID):
        """
        Logs a string.
        @param logMessage: Text which should be logged
        @type logMessage: string
        @param severity: The level of the message, warning or error. Defined by the class LogLevel
        @type severity: int
        @param channel: Custom text to categorize the message channel
        @type channel: string
        @param logID: ID of the serverconnection to identify the current server connection when using multiple connections, 0 if unused
        @type logID: int
        @return: The errorcode
        @rtype: int
        """
  */
  char* logMessage;
  int severity;
  char* channel;
  unsigned long long logID;

  if (!PyArg_ParseTuple(args, "sisK", &logMessage, &severity, &channel, &logID))
    return NULL;

  unsigned int res = ts3_funcs.logMessage(logMessage, (enum LogLevel)severity, channel, (uint64)logID);

  return Py_BuildValue("I", res);
}

PyObject* openCaptureDevice(PyObject* /*self*/, PyObject* args) {
  //unsigned int openCaptureDevice(uint64 serverConnectionHandlerID, const char *modeID, const char *captureDevice)
  /*
    @staticmethod
    def openCaptureDevice(serverConnectionHandlerID, modeID, captureDevice):
        """
        Opens a playback device on a serverconnection.
        @param serverConnectionHandlerID: ID of the serverconnection on which the capture device should be initialized on
        @type serverConnectionHandlerID: int
        @param modeID: the playback mode to use
        @type modeID: string
        @param captureDevice: the id of the capture device
        @type captureDevice: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* modeID;
  char* captureDevice;

  if (!PyArg_ParseTuple(args, "Kss", &schid, &modeID, &captureDevice))
    return NULL;

  unsigned int res = ts3_funcs.openCaptureDevice(schid, modeID, captureDevice);

  return Py_BuildValue("I", res);
}

PyObject* openPlaybackDevice(PyObject* /*self*/, PyObject* args) {
  //unsigned int openPlaybackDevice(uint64 serverConnectionHandlerID, const char *modeID, const char *playbackDevice)
  /*
    @staticmethod
    def openPlaybackDevice(serverConnectionHandlerID, modeID, playbackDevice):
        """
        Opens a playback device on a serverconnection.
        @param serverConnectionHandlerID: ID of the serverconnection on which the playback device should be initialized on
        @type serverConnectionHandlerID: int
        @param modeID: the playback mode to use
        @type modeID: string
        @param playbackDevice: the id of the playback device
        @type playbackDevice: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* modeID;
  char* playbackDevice;

  if (!PyArg_ParseTuple(args, "Kss", &schid, &modeID, &playbackDevice))
    return NULL;

  unsigned int res = ts3_funcs.openPlaybackDevice(schid, modeID, playbackDevice);

  return Py_BuildValue("I", res);
}

PyObject* pauseWaveFileHandle(PyObject* /*self*/, PyObject* args) {
  //unsigned int pauseWaveFileHandle(uint64 serverConnectionHandlerID, uint64 waveHandle, int pause)
  /*
    @staticmethod
    def pauseWaveFileHandle(serverConnectionHandlerID, waveHandle, pause):
        """
        Pauses a wavefile sound previously started with playWaveFileHandle.
        @param serverConnectionHandlerID: the ID of the serverConnection the sound is played on
        @type serverConnectionHandlerID: int
        @param waveHandle: the handle returned by playWaveFileHandle
        @type waveHandle: int
        @param pause: if set to 1 (or True), the sound will pause, 0 (or False) will unpause the sound
        @type pause: int or bool
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long waveHandle;
  int pause;

  if (!PyArg_ParseTuple(args, "KKi", &schid, &waveHandle, &pause))
    return NULL;

  unsigned int res = ts3_funcs.pauseWaveFileHandle((uint64)schid, (uint64)waveHandle, pause);

  return Py_BuildValue("I", res);
}

PyObject* playWaveFile(PyObject* /*self*/, PyObject* args) {
  //unsigned int playWaveFile(uint64 serverConnectionHandlerID, const char *path)
  /*
    @staticmethod
    def playWaveFile(serverConnectionHandlerID, path):
        """
        Plays a wavefile sound on a serverconnection.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param path: the path to the wavefile on the system
        @type path: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* path;

  if (!PyArg_ParseTuple(args, "Ks", &schid, &path))
    return NULL;

  unsigned int res = ts3_funcs.playWaveFile((uint64)schid, path);

  return Py_BuildValue("I", res);
}

PyObject* playWaveFileHandle(PyObject* /*self*/, PyObject* args) {
  //unsigned int playWaveFileHandle(uint64 serverConnectionHandlerID, const char *path, int loop, uint64 *waveHandle)
  /*
    @staticmethod
    def playWaveFileHandle(serverConnectionHandlerID, path, loop):
        """
        Plays a wavefile sound on a serverconnection and returns a handle to it.
        @param serverConnectionHandlerID: the ID of the serverconnection on which the sound will be played on
        @type serverConnectionHandlerID: int
        @param path: the path to the wavefile on the system
        @type path: string
        @param loop: if set to 1 (or True), the sound will loop
        @type loop: int or bool
        @return: A tuple, containing the errorcode and the handle, with which the sound can be paused and unpaused
        @rtype: tuple (int, int)
        """
  */
  long schid;
  char* path;
  int loop;

  if (!PyArg_ParseTuple(args, "Ksi", &schid, &path, &loop))
    return NULL;

  uint64 waveHandle;
  unsigned int res = ts3_funcs.playWaveFileHandle((uint64)schid, path, loop, &waveHandle);

  return Py_BuildValue("(IK)", res, (unsigned long long)waveHandle);
}

PyObject* printMessage(PyObject* /*self*/, PyObject* args) {
  //void printMessage(uint64 serverConnectionHandlerID, const char *message, enum PluginMessageTarget messageTarget)
  /*
    @staticmethod
    def printMessage(serverConnectionHandlerID, message, messageTarget):
        """
        Prints a message to a specific client chat tab.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param message: the message to print
        @type message: string
        @param messageTarget: the target to send the message, see ts3defines.PluginMessageTarget
        @type messageTarget: int
        """
  */
  unsigned long long schid;
  char* message;
  int messageTarget;

  if (!PyArg_ParseTuple(args, "Ksi", &schid, &message, &messageTarget))
    return NULL;

  ts3_funcs.printMessage((uint64)schid, message, (enum PluginMessageTarget)messageTarget);

  Py_RETURN_NONE;
}

PyObject* printMessageToCurrentTab(PyObject* /*self*/, PyObject* args) {
  //void printMessageToCurrentTab(const char *message)
  /*
    @staticmethod
    def printMessageToCurrentTab(message):
        """
        Prints a message to the currently visible tab.
        @param message: the message to send
        @type message: string
        """
  */
  char* message;

  if (!PyArg_ParseTuple(args, "s", &message))
    return NULL;

  ts3_funcs.printMessageToCurrentTab(message);

  Py_RETURN_NONE;
}

PyObject* privilegeKeyUse(PyObject* /*self*/, PyObject* args) {
  //unsigned int privilegeKeyUse(uint64 serverConnectionHandler, const char *tokenKey, const char *returnCode)
  /*
    @staticmethod
    def privilegeKeyUse(serverConnectionHandlerID, tokenKey, returnCode):
        """
        Uses a privilege key as the current client of the serverconnection.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param tokenKey: the token
        @type tokenKey: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* token;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Ks|s", &schid, &token, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.privilegeKeyUse((uint64)schid, token, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* processCustomCaptureData(PyObject* /*self*/, PyObject* args) {
  //unsigned int processCustomCaptureData(const char *deviceName, const short *buffer, int samples)
  /*
    @staticmethod
    def processCustomCaptureData(deviceName, buffer):
        """
        Sends captured data from a custom device to the client libg
        @param deviceName: the name of the device capturing the data, previously registered with registerCustomDevice
        @type deviceName: string
        @param buffer: a list containing the buffered data
        @type buffer: list [int]
        @return: the errorcode
        @rtype: int
        """
  */
  char* deviceName;
  PyObject* pybuffer;

  if (!PyArg_ParseTuple(args, "sO", &deviceName, &pybuffer))
    return NULL;

  QString error;
  short* buffer;
  if (!PYLIST_TO_ARRAY(short, pybuffer, error, &buffer, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("No valid short list in argument buffer").toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.processCustomCaptureData(deviceName, buffer, PyList_Size(pybuffer));
  free(buffer);

  return Py_BuildValue("I", res);
}

PyObject* registerCustomDevice(PyObject* /*self*/, PyObject* args) {
  //unsigned int registerCustomDevice(const char *deviceID, const char *deviceDisplayName, int capFrequency, int capChannels, int playFrequency, int playChannels)
  /*
    @staticmethod
    def registerCustomDevice(deviceID, deviceDisplayName, capFrequency, capChannels, playFrequency, playChannels):
        """
        Registers a custom device, announcing the device ID and name to the Client Lib.
        @param deviceID: ID string of the custom device, under which the device can be later accessed
        @type deviceID: string
        @param deviceDisplayName: Displayed name of the custom device. Freely choose a name which identifies your device
        @type deviceDisplayName: string
        @param capFrequency: Frequency of the capture device
        @type capFrequency: int
        @param capChannels: Number of channels of the capture device. This value depends on if the used codec is a mono or stereo CodecEncryptionMode
        @type capChannels: int
        @param playFrequency: Frequency of the playback deviceDisplayName
        @type playFrequency: int
        @param playChannels: Number of channels of the playback device
        @type playChannels: int
        @return: the errorcode
        @rtype: int
        """
  */
  char* deviceID;
  char* deviceDisplayName;
  int capFrequency;
  int capChannels;
  int playFrequency;
  int playChannels;

  if (!PyArg_ParseTuple(args, "ssiiii", &deviceID, &deviceDisplayName, &capFrequency, &capChannels, &playFrequency, &playChannels))
    return NULL;

  unsigned int res = ts3_funcs.registerCustomDevice(deviceID, deviceDisplayName, capFrequency, capChannels, playFrequency, playChannels);

  return Py_BuildValue("I", res);
}

PyObject* requestBanList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestBanList(uint64 serverConnectionHandlerID, const char *returnCode)
  /*
    @staticmethod
    def requestBanList(serverConnectionHandlerID, returnCode):
        """
        Requests the banlist on a server. The event onBanListEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "K|s", &schid, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestBanList((uint64)schid, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelAddPerm(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelAddPerm(uint64 serverConnectionHandlerID, uint64 channelID, const unsigned int *permissionIDArray, const int *permissionValueArray, int arraySize, const char *returnCode)
  /*
    @staticmethod
    def requestChannelAddPerm(serverConnectionHandlerID, channelID, permissionIDArray, permissionValueArray, returnCode):
        """
        Adds a list of permissions to a channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param permissionIDArray: a list of permission IDs
        @type permissionIDArray: list [int]
        @param permissionValueArray: list of permission values, in order of the permissions in permissionIDArray
        @type permissionValueArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  PyObject* pypermissionIDArray;
  PyObject* pypermissionValueArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKOO|s", &schid, &channelID, &pypermissionIDArray, &pypermissionValueArray, &returnCode))
    return NULL;

  QString error;
  unsigned int* permissionIDArray;
  if (!PYLIST_TO_ARRAY(unsigned int, pypermissionIDArray, error, &permissionIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionIDArray failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  int* permissionValueArray;
  if (!PYLIST_TO_ARRAY(int, pypermissionValueArray, error, &permissionValueArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionValueArray failed with error \"%1\"").arg(error).toUtf8().data());
    free(permissionIDArray);
    return NULL;
  }

  //pyListToArray checked for PyLists
  if (PyList_Size(pypermissionIDArray) != PyList_Size(pypermissionValueArray)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("List lengthes do not match").toUtf8().data());
    free(permissionIDArray);
    free(permissionValueArray);
    return NULL;
  }

  unsigned int res = ts3_funcs.requestChannelAddPerm((uint64)schid, (uint64)channelID, permissionIDArray, permissionValueArray, PyList_Size(pypermissionIDArray), returnCode);
  free(permissionIDArray);
  free(permissionValueArray);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelClientAddPerm(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelClientAddPerm(uint64 serverConnectionHandlerID, uint64 channelID, uint64 clientDatabaseID, const unsigned int *permissionIDArray, const int *permissionValueArray, int arraySize, const char *returnCode)
  /*
    @staticmethod
    def requestChannelClientAddPerm(serverConnectionHandlerID, channelID, clientDatabaseID, permissionIDArray, permissionValueArray, returnCode):
        """
        Adds a list of permissions on a channel to a user.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param clientDatabaseID: the database ID of the user
        @type clientDatabaseID: int
        @param permissionIDArray: a list of permission IDs
        @type permissionIDArray: list [int]
        @param permissionValueArray: list of permission values, in order of the permissions in permissionIDArray
        @type permissionValueArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  unsigned long long clientDatabaseID;
  PyObject* pypermissionIDArray;
  PyObject* pypermissionValueArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKKOO|s", &schid, &channelID, &clientDatabaseID, &pypermissionIDArray, &pypermissionValueArray, &returnCode))
    return NULL;

  QString error;
  unsigned int* permissionIDArray;
  if (!PYLIST_TO_ARRAY(unsigned int, pypermissionIDArray, error, &permissionIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionIDArray failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  int* permissionValueArray;
  if (!PYLIST_TO_ARRAY(int, pypermissionValueArray, error, &permissionValueArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionValueArray failed with error \"%1\"").arg(error).toUtf8().data());
    free(permissionIDArray);
    return NULL;
  }

  if (PyList_Size(pypermissionIDArray) != PyList_Size(pypermissionValueArray)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("List lengthes do not match").toUtf8().data());
    free(permissionIDArray);
    free(permissionValueArray);
    return NULL;
  }

  unsigned int res = ts3_funcs.requestChannelClientAddPerm((uint64)schid, (uint64)channelID, (uint64)clientDatabaseID, permissionIDArray, permissionValueArray, PyList_Size(pypermissionIDArray), returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelClientDelPerm(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelClientDelPerm(uint64 serverConnectionHandlerID, uint64 channelID, uint64 clientDatabaseID, const unsigned int *permissionIDArray, int arraySize, const char *returnCode)
  /*
    @staticmethod
    def requestChannelClientDelPerm(serverConnectionHandlerID, channelID, clientDatabaseID, permissionIDArray, returnCode):
        """
        Deletes a list of permissions of a user in a channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param clientDatabaseID: the database ID of the user
        @type clientDatabaseID: int
        @param permissionIDArray: a list of permission IDs
        @type permissionIDArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  unsigned long long clientDatabaseID;
  PyObject* pypermissionIDArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKKO|s", &schid, &channelID, &clientDatabaseID, &pypermissionIDArray, &returnCode))
    return NULL;

  QString error;
  unsigned int* permissionIDArray;
  if (!PYLIST_TO_ARRAY(unsigned int, pypermissionIDArray, error, &permissionIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.requestChannelClientDelPerm((uint64)schid, (uint64)channelID, (uint64)clientDatabaseID, permissionIDArray, PyList_Size(pypermissionIDArray), returnCode);
  free(permissionIDArray);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelClientPermList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelClientPermList(uint64 serverConnectionHandlerID, uint64 channelID, uint64 clientDatabaseID, const char *returnCode)
  /*
    @staticmethod
    def requestChannelClientPermList(serverConnectionHandlerID, channelID, clientDatabaseID, returnCode):
        """
        Requests the list of permissions of a user in a channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param clientDatabaseID: the database ID of the user
        @type clientDatabaseID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  unsigned long long clientDatabaseID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKK|s", &schid, &channelID, &clientDatabaseID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestChannelClientPermList((uint64)schid, (uint64)channelID, (uint64)clientDatabaseID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelDelete(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelDelete(uint64 serverConnectionHandlerID, uint64 channelID, int force, const char *returnCode)
  /*
    @staticmethod
    def requestChannelDelete(serverConnectionHandlerID, channelID, force, returnCode):
        """
        Deletes a channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel to delete
        @type channelID: int
        @param force: if set to 1 (or True), the channel will be deleted even when it is not empty
        @type force: int or bool
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  int force;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKi|s", &schid, &channelID, &force, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestChannelDelete((uint64)schid, (uint64)channelID, force, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelDelPerm(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelDelPerm(uint64 serverConnectionHandlerID, uint64 channelID, const unsigned int *permissionIDArray, int arraySize, const char *returnCode)
  /*
    @staticmethod
    def requestChannelDelPerm(serverConnectionHandlerID, channelID, permissionIDArray, returnCode):
        """
        Deletes a list of permissions from a channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param permissionIDArray: a list of permission IDs
        @type permissionIDArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  PyObject* pypermissionIDArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKO|s", &schid, &channelID, &pypermissionIDArray, &returnCode))
    return NULL;

  QString error;
  unsigned int* permissionIDArray;
  if (!PYLIST_TO_ARRAY(unsigned int, pypermissionIDArray, error, &permissionIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.requestChannelDelPerm((uint64)schid, (uint64)channelID, permissionIDArray, PyList_Size(pypermissionIDArray), returnCode);
  free(permissionIDArray);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelDescription(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelDescription(uint64 serverConnectionHandlerID, uint64 channelID, const char *returnCode)
  /*
    @staticmethod
    def requestChannelDescription(serverConnectionHandlerID, channelID, returnCode):
        """
        Requests the channel description of a channel. Afterwards, getChannelVariableAsString can return it.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &channelID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestChannelDescription((uint64)schid, (uint64)channelID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelGroupAdd(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelGroupAdd(uint64 serverConnectionHandlerID, const char *groupName, int groupType, const char *returnCode)
  /*
    @staticmethod
    def requestChannelGroupAdd(serverConnectionHandlerID, groupName, groupType, returnCode):
        """
        Adds a channelgroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param groupName: the name of the channelgroup to create
        @type groupName: string
        @param groupType: type of the channelgroup, see ts3defines.GroupType
        @type groupType: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* groupName;
  int groupType;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Ksi|s", &schid, &groupName, &groupType, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestChannelGroupAdd((uint64)schid, groupName, groupType, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelGroupAddPerm(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelGroupAddPerm(uint64 serverConnectionHandlerID, uint64 channelGroupID, int continueonerror, const unsigned int *permissionIDArray, const int *permissionValueArray, int arraySize, const char *returnCode)
  /*
    @staticmethod
    def requestChannelGroupAddPerm(serverConnectionHandlerID, channelGroupID, continueonerror, permissionIDArray, permissionValueArray, returnCode):
        """
        Adds a list of permissions to a channelgroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelGroupID: the ID of the channelgroup
        @type channelGroupID: int
        @param continueonerror: if set to True, if an error with a permission occurs, the other permissions will even though be handled
        @type continueonerror: bool
        @param permissionIDArray: a list of permission IDs
        @type permissionIDArray: list [int]
        @param permissionValueArray: list of permission values, in order of the permissions in permissionIDArray
        @type permissionValueArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelGroupID;
  PyObject* pycontinueonerror;
  PyObject* pypermissionIDArray;
  PyObject* pypermissionValueArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKO!OO|s", &schid, &channelGroupID, &PyBool_Type, &pycontinueonerror, &pypermissionIDArray, &pypermissionValueArray, &returnCode))
    return NULL;

  QString error;
  unsigned int* permissionIDArray;
  if (!PYLIST_TO_ARRAY(unsigned int, pypermissionIDArray, error, &permissionIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionIDArray failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  int* permissionValueArray;
  if (!PYLIST_TO_ARRAY(int, pypermissionValueArray, error, &permissionValueArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionValueArray failed with error \"%1\"").arg(error).toUtf8().data());
    free(permissionIDArray);
    return NULL;
  }

  //pyListToArray checked for PyLists
  if (PyList_Size(pypermissionIDArray) != PyList_Size(pypermissionValueArray)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("List lengthes do not match").toUtf8().data());
    free(permissionIDArray);
    free(permissionValueArray);
    return NULL;
  }

  unsigned int res = ts3_funcs.requestChannelGroupAddPerm((uint64)schid, (uint64)channelGroupID, PyObject_IsTrue(pycontinueonerror) ? 1 : 0, permissionIDArray, permissionValueArray, PyList_Size(pypermissionIDArray), returnCode);
  free(permissionIDArray);
  free(permissionValueArray);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelGroupDel(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelGroupDel(uint64 serverConnectionHandlerID, uint64 channelGroupID, int force, const char *returnCode)
  /*
    @staticmethod
    def requestChannelGroupDel(serverConnectionHandlerID, channelGroupID, force, returnCode):
        """
        Deletes a channelgroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelGroupID: the ID of the channelgroup
        @type channelGroupID: int
        @param force: if set to 1 (or True), even if there are users assigned to this channelgroup, it will be deleted
        @type force: int or bool
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelGroupID;
  int force;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKi|s", &schid, &channelGroupID, &force, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestChannelGroupDel((uint64)schid, (uint64)channelGroupID, force, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelGroupDelPerm(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelGroupDelPerm(uint64 serverConnectionHandlerID, uint64 channelGroupID, int continueOnError, const unsigned int *permissionIDArray, int arraySize, const char *returnCode)
  /*
    @staticmethod
    def requestChannelGroupDelPerm(serverConnectionHandlerID, channelGroupID, continueOnError, permissionIDArray, returnCode):
        """
        Deletes a list of permissions from a channelgroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelGroupID: the ID of the channelgroup
        @type channelGroupID: int
        @param continueOnError: if set to 1 (or True), if an error with a permission occurs, the other permissions will even though be handled
        @type continueOnError: int or bool
        @param permissionIDArray: a list of permission IDs
        @type permissionIDArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelGroupID;
  int continueOnError;
  PyObject* pypermissionIDArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKiO|s", &schid, &channelGroupID, &continueOnError, &pypermissionIDArray, &returnCode))
    return NULL;

  QString error;
  unsigned int* permissionIDArray;
  if (!PYLIST_TO_ARRAY(unsigned int, pypermissionIDArray, error, &permissionIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.requestChannelGroupDelPerm((uint64)schid, (uint64)channelGroupID, continueOnError, permissionIDArray, PyList_Size(pypermissionIDArray), returnCode);
  free(permissionIDArray);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelGroupList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelGroupList(uint64 serverConnectionHandlerID, const char *returnCode)
  /*
    @staticmethod
    def requestChannelGroupList(serverConnectionHandlerID, returnCode):
        """
        Requests the list of channelgroups. The events onChannelGroupListEvent and onChannelGroupListEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "K|s", &schid, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestChannelGroupList((uint64)schid, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelGroupPermList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelGroupPermList(uint64 serverConnectionHandlerID, uint64 channelGroupID, const char *returnCode)
  /*
    @staticmethod
    def requestChannelGroupPermList(serverConnectionHandlerID, channelGroupID, returnCode):
        """
        Requests the list of permissions assigned to a channelgroup. The events onChannelGroupPermListEvent and onChannelGroupPermListFinishedEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelGroupID: the ID of the channelGroupID
        @type channelGroupID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelGroupID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &channelGroupID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestChannelGroupPermList((uint64)schid, (uint64)channelGroupID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelMove(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelMove(uint64 serverConnectionHandlerID, uint64 channelID, uint64 newChannelParentID, uint64 newChannelOrder, const char *returnCode)
  /*
    @staticmethod
    def requestChannelMove(serverConnectionHandlerID, channelID, newChannelParentID, newChannelOrder, returnCode):
        """
        Moves a channel to a new parent channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel to move
        @type channelID: int
        @param newChannelParentID: the ID of the new parent channel
        @type newChannelParentID: int
        @param newChannelOrder: Channel order defining where the channel should be sorted under the new parent. Pass 0 to sort the channel right after the parent
        @type newChannelOrder: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  unsigned long long newChannelParentID;
  unsigned long long newChannelOrder;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKKK|s", &schid, &channelID, &newChannelParentID, &newChannelOrder, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestChannelMove((uint64)schid, (uint64)channelID, (uint64)newChannelParentID, (uint64)newChannelOrder, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelPermList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelPermList(uint64 serverConnectionHandlerID, uint64 channelID, const char *returnCode)
  /*
    @staticmethod
    def requestChannelPermList(serverConnectionHandlerID, channelID, returnCode):
        """
        Requests the list of permissions assigned to a channel. The events onChannelPermListEvent and onChannelPermListFinishedEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &channelID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestChannelPermList((uint64)schid, (uint64)channelID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelSubscribe(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelSubscribe(uint64 serverConnectionHandlerID, const uint64 *channelIDArray, const char *returnCode)
  /*
    @staticmethod
    def requestChannelSubscribe(serverConnectionHandlerID, channelIDArray, returnCode):
        """
        Subscribes to a list of channels to get notifications of the clients in them.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelIDArray: a list of channel IDs
        @type channelIDArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  PyObject* pychannelIDArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KO|s", &schid, &pychannelIDArray, &returnCode))
    return NULL;

  QString error;
  uint64* channelIDArray;
  if (!PYLIST_TO_ARRAY(uint64, pychannelIDArray, error, &channelIDArray, true)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.requestChannelSubscribe((uint64)schid, channelIDArray, returnCode);
  free(channelIDArray);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelSubscribeAll(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelSubscribeAll(uint64 serverConnectionHandlerID, const char *returnCode)
  /*
    @staticmethod
    def requestChannelSubscribeAll(serverConnectionHandlerID, returnCode):
        """
        Subscribes to all channels on the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "K|s", &schid, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestChannelSubscribeAll((uint64)schid, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelUnsubscribe(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelUnsubscribe(uint64 serverConnectionHandlerID, const uint64 *channelIDArray, const char *returnCode)
  /*
    @staticmethod
    def requestChannelUnsubscribe(serverConnectionHandlerID, channelIDArray, returnCode):
        """
        Unsubscribes from a list channels.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelIDArray: a list of channel IDs
        @type channelIDArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  PyObject* pychannelIDArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KO|s", &schid, &pychannelIDArray, &returnCode))
    return NULL;

  QString error;
  uint64* channelIDArray;
  if (!PYLIST_TO_ARRAY(uint64, pychannelIDArray, error, &channelIDArray, true)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.requestChannelUnsubscribe((uint64)schid, channelIDArray, returnCode);
  free(channelIDArray);

  return Py_BuildValue("I", res);
}

PyObject* requestChannelUnsubscribeAll(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestChannelUnsubscribeAll(uint64 serverConnectionHandlerID, const char *returnCode)
  /*
    @staticmethod
    def requestChannelUnsubscribeAll(serverConnectionHandlerID, returnCode):
        """
        Unsubscribes from all channels on the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "K|s", &schid, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestChannelUnsubscribeAll((uint64)schid, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientAddPerm(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientAddPerm(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, const unsigned int *permissionIDArray, const int *permissionValueArray, const int *permissionSkipArray, int arraySize, const char *returnCode)
  /*
    @staticmethod
    def requestClientAddPerm(serverConnectionHandlerID, clientDatabaseID, permissionIDArray, permissionValueArray, permissionSkipArray, returnCode):
        """
        Adds a list of permissions to a user.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientDatabaseID: the database ID of the user
        @type clientDatabaseID: int
        @param permissionIDArray: a list of permission IDs
        @type permissionIDArray: list [int]
        @param permissionValueArray: list of permission values, in order of the permissions in permissionIDArray
        @type permissionValueArray: list [int]
        @param permissionSkipArray: list of permission skip values, in order of the permissions in permissionIDArray
        @type permissionSkipArray: list [int]
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long clientDatabaseID;
  PyObject* pypermissionIDArray;
  PyObject* pypermissionValueArray;
  PyObject* pypermissionSkipArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKOOO|s", &schid, &clientDatabaseID, &pypermissionIDArray, &pypermissionValueArray, &pypermissionSkipArray, &returnCode))
    return NULL;

  QString error;
  unsigned int* permissionIDArray;
  if (!PYLIST_TO_ARRAY(unsigned int, pypermissionIDArray, error, &permissionIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionIDArray failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  int* permissionValueArray;
  if (!PYLIST_TO_ARRAY(int, pypermissionValueArray, error, &permissionValueArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionValueArray failed with error \"%1\"").arg(error).toUtf8().data());
    free(permissionIDArray);
    return NULL;
  }

  int* permissionSkipArray;
  if (!PYLIST_TO_ARRAY(int, pypermissionSkipArray, error, &permissionSkipArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionSkipArray failed with error \"%1\"").arg(error).toUtf8().data());
    free(permissionIDArray);
    free(permissionValueArray);
    return NULL;
  }

  if ((PyList_Size(pypermissionIDArray) != PyList_Size(pypermissionValueArray)) || (PyList_Size(pypermissionValueArray) != PyList_Size(pypermissionSkipArray))) {
    PyErr_SetString(PyExc_AttributeError, TRANS("List lengthes do not match").toUtf8().data());
    free(permissionIDArray);
    free(permissionValueArray);
    free(permissionSkipArray);
    return NULL;
  }

  unsigned int res = ts3_funcs.requestClientAddPerm((uint64)schid, (uint64)clientDatabaseID, permissionIDArray, permissionValueArray, permissionSkipArray, PyList_Size(pypermissionIDArray), returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientDBIDfromUID(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientDBIDfromUID(uint64 serverConnectionHandlerID, const char *clientUniqueIdentifier, const char *returnCode)
  /*
    @staticmethod
    def requestClientDBIDfromUID(serverConnectionHandlerID, clientUniqueIdentifier, returnCode):
        """
        Requests the database ID of a client defined by the UID. The event onClientDBIDfromUIDEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientUniqueIdentifier: the UID of the client
        @type clientUniqueIdentifier: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* clientUniqueIdentifier;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Ks|s", &schid, &clientUniqueIdentifier, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientDBIDfromUID((uint64)schid, clientUniqueIdentifier, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientDelPerm(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientDelPerm(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, const unsigned int *permissionIDArray, int arraySize, const char *returnCode)
  /*
    @staticmethod
    def requestClientDelPerm(serverConnectionHandlerID, clientDatabaseID, permissionIDArray, returnCode):
        """
        Deletes a list of permissions from a user.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientDatabaseID: the database ID of the user
        @type clientDatabaseID: int
        @param permissionIDArray: a list of permission IDs
        @type permissionIDArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long clientDatabaseID;
  PyObject* pypermissionIDArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKO|s", &schid, &clientDatabaseID, &pypermissionIDArray, &returnCode))
    return NULL;

  QString error;
  unsigned int* permissionIDArray;
  if (!PYLIST_TO_ARRAY(unsigned int, pypermissionIDArray, error, &permissionIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.requestClientDelPerm((uint64)schid, (uint64)clientDatabaseID, permissionIDArray, PyList_Size(pypermissionIDArray), returnCode);
  free(permissionIDArray);

  return Py_BuildValue("I", res);
}

PyObject* requestClientEditDescription(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientEditDescription(uint64 serverConnectionHandlerID, anyID clientID, const char *clientDescription, const char *returnCode)
  /*
    @staticmethod
    def requestClientEditDescription(serverConnectionHandlerID, clientID, clientDescription, returnCode):
        """
        Sets the description of a client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @param clientDescription: the description to set
        @type clientDescription: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  char* clientDescription;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KIs|s", &schid, &clientID, &clientDescription, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientEditDescription((uint64)schid, (anyID)clientID, clientDescription, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientIDs(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientIDs(uint64 serverConnectionHandlerID, const char *clientUniqueIdentifier, const char *returnCode)
  /*
    @staticmethod
    def requestClientIDs(serverConnectionHandlerID, clientUniqueIdentifier, returnCode):
        """
        Requests the client IDs for a given UID. Will trigger the event onClientIDsEvent.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientUniqueIdentifier: the UID of the client
        @type clientUniqueIdentifier: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* clientUniqueIdentifier;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Ks|s", &schid, &clientUniqueIdentifier, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientIDs((uint64)schid, clientUniqueIdentifier, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientKickFromChannel(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientKickFromChannel(uint64 serverConnectionHandlerID, anyID clientID, const char *kickReason, const char *returnCode)
  /*
    @staticmethod
    def requestClientKickFromChannel(serverConnectionHandlerID, clientID, kickReason, returnCode):
        """
        Kicks a client from its current channel to the default one.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client to kick
        @type clientID: int
        @param kickReason: the reason for the kick
        @type kickReason: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  char* kickReason;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KIs|s", &schid, &clientID, &kickReason, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientKickFromChannel((uint64)schid, (anyID)clientID, kickReason, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientKickFromServer(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientKickFromServer(uint64 serverConnectionHandlerID, anyID clientID, const char *kickReason, const char *returnCode)
  /*
    @staticmethod
    def requestClientKickFromServer(serverConnectionHandlerID, clientID, kickReason, returnCode):
        """
        Kicks a client from the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client to kick
        @type clientID: int
        @param kickReason: the reason for the kick
        @type kickReason: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return:
        @rtype:
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  char* kickReason;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KIs|s", &schid, &clientID, &kickReason, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientKickFromServer((uint64)schid, (anyID)clientID, kickReason, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientMove(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientMove(uint64 serverConnectionHandlerID, anyID clientID, uint64 newChannelID, const char *password, const char *returnCode)
  /*
    @staticmethod
    def requestClientMove(serverConnectionHandlerID, clientID, newChannelID, password, returnCode):
        """
        Moves a client to a different channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client to be moved
        @type clientID: int
        @param newChannelID: the ID of the channel moving the client to
        @type newChannelID: int
        @param password: password of the channel, leave empty if channel is not password protected
        @type password: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  unsigned long long newChannelID;
  char* password;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KIKs|s", &schid, &clientID, &newChannelID, &password, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientMove((uint64)schid, (anyID)clientID, (uint64)newChannelID, password, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientNamefromDBID(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientNamefromDBID(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, const char *returnCode)
  /*
    @staticmethod
    def requestClientNamefromDBID(serverConnectionHandlerID, clientDatabaseID, returnCode):
        """
        Requests the name of a client defined by the database ID. The event onClientNamefromDBIDEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientDatabaseID: the database ID of the client
        @type clientDatabaseID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long clientDatabaseID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &clientDatabaseID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientNamefromDBID((uint64)schid, (uint64)clientDatabaseID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientNamefromUID(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientNamefromUID(uint64 serverConnectionHandlerID, const char *clientUniqueIdentifier, const char *returnCode)
  /*
    @staticmethod
    def requestClientNamefromUID(serverConnectionHandlerID, clientUniqueIdentifier, returnCode):
        """
        Requests the name of a client defined by the UID. The event onClientNamefromUIDEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientUniqueIdentifier: the UID of the client
        @type clientUniqueIdentifier: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* clientUniqueIdentifier;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Ks|s", &schid, &clientUniqueIdentifier, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientNamefromUID((uint64)schid, clientUniqueIdentifier, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientPermList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientPermList(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, const char *returnCode)
  /*
    @staticmethod
    def requestClientPermList(serverConnectionHandlerID, clientDatabaseID, returnCode):
        """
        Requests the list of permissions assigned to a user. The events
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientDatabaseID: the database ID of the user
        @type clientDatabaseID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long clientDatabaseID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &clientDatabaseID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientPermList((uint64)schid, (uint64)clientDatabaseID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientPoke(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientPoke(uint64 serverConnectionHandlerID, anyID clientID, const char *message, const char *returnCode)
  /*
    @staticmethod
    def requestClientPoke(serverConnectionHandlerID, clientID, message, returnCode):
        """
        Pokes a client with a given message.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @param message: the message
        @type message: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  char* message;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KIs|s", &schid, &clientID, &message, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientPoke((uint64)schid, (anyID)clientID, message, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientSetIsTalker(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientSetIsTalker(uint64 serverConnectionHandlerID, anyID clientID, int isTalker, const char *returnCode)
  /*
    @staticmethod
    def requestClientSetIsTalker(serverConnectionHandlerID, clientID, isTalker, returnCode):
        """
        Grants or revokes the talker flag of a client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @param isTalker: if set to 1 (or True) grants talker flag, if 0 (or False) revokes talker flag
        @type isTalker: int or bool
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  int isTalker;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KIi|s", &schid, &clientID, &isTalker, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientSetIsTalker((uint64)schid, (anyID)clientID, isTalker, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestClientSetWhisperList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientSetWhisperList(uint64 serverConnectionHandlerID, anyID clientID, const uint64 *targetChannelIDArray, const anyID *targetClientIDArray, const char *returnCode)
  /*
    @staticmethod
    def requestClientSetWhisperList(serverConnectionHandlerID, clientID, targetChannelIDArray, targetClientIDArray, returnCode):
        """
        Modifies the whisper list of a client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client whose whisperlist is modified. If set to 0, the own whisper list is modified
        @type clientID: int
        @param targetChannelIDArray: a list of channel IDs the client will whisper to
        @type targetChannelIDArray: list [int]
        @param targetClientIDArray: a list of client IDs the client will whisper to
        @type targetClientIDArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  PyObject* pytargetChannelIDArray;
  PyObject* pytargetClientIDArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KIOO|s", &schid, &clientID, &pytargetChannelIDArray, &pytargetClientIDArray, &returnCode))
    return NULL;

  QString error;
  uint64* targetChannelIDArray;
  if (!PYLIST_TO_ARRAY(uint64, pytargetChannelIDArray, error, &targetChannelIDArray, true)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of targetChannelIDArray failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  anyID* targetClientIDArray;
  if (!PYLIST_TO_ARRAY(anyID, pytargetClientIDArray, error, &targetClientIDArray, true)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of targetClientIDArray failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.requestClientSetWhisperList((uint64)schid, (anyID)clientID, targetChannelIDArray, targetClientIDArray, returnCode);
  free(targetChannelIDArray);
  free(targetClientIDArray);

  return Py_BuildValue("I", res);
}

PyObject* requestClientVariables(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestClientVariables(uint64 serverConnectionHandlerID, anyID clientID, const char *returnCode)
  /*
    @staticmethod
    def requestClientVariables(serverConnectionHandlerID, clientID, returnCode):
      """
      Requests latest data for a given client. The event onUpdateClientEvent will be triggered.
      @param serverConnectionHandlerID: the ID of the serverconnection
      @type serverConnectionHandlerID: int
      @param clientID: the ID of the client
      @type clientID: int
      @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
      @type returnCode: string
      @return: the errorcode
      @rtype: int
      """
  */
  unsigned long long schid;
  unsigned int clientID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KI|s", &schid, &clientID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestClientVariables((uint64)schid, (anyID)clientID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestComplainAdd(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestComplainAdd(uint64 serverConnectionHandlerID, uint64 targetClientDatabaseID, const char *complainReason, const char *returnCode)
  /*
    @staticmethod
    def requestComplainAdd(serverConnectionHandlerID, targetClientDatabaseID, complainReason, returnCode):
        """
        Adds a complain to a user defined by his database ID.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param targetClientDatabaseID: the database ID of the user
        @type targetClientDatabaseID: int
        @param complainReason: the reason for the complain
        @type complainReason: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long targetClientDatabaseID;
  char* complainReason;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKs|s", &schid, &targetClientDatabaseID, &complainReason, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestComplainAdd((uint64)schid, (uint64)targetClientDatabaseID, complainReason, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestComplainDel(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestComplainDel(uint64 serverConnectionHandlerID, uint64 targetClientDatabaseID, uint64 fromClientDatabaseID, const char *returnCode)
  /*
    @staticmethod
    def requestComplainDel(serverConnectionHandlerID, targetClientDatabaseID, fromClientDatabaseID, returnCode):
        """
        Deletes a complain to a user by a different user.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param targetClientDatabaseID: the database ID of the complained user
        @type targetClientDatabaseID: int
        @param fromClientDatabaseID: the database ID of the complaining user
        @type fromClientDatabaseID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long targetClientDatabaseID;
  unsigned long long fromClientDatabaseID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKK|s", &schid, &targetClientDatabaseID, &fromClientDatabaseID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestComplainDel((uint64)schid, (uint64)targetClientDatabaseID, (uint64)fromClientDatabaseID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestComplainDelAll(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestComplainDelAll(uint64 serverConnectionHandlerID, uint64 targetClientDatabaseID, const char *returnCode)
  /*
    @staticmethod
    def requestComplainDelAll(serverConnectionHandlerID, targetClientDatabaseID, returnCode):
        """
        Deletes all complains to a user.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param targetClientDatabaseID: the database ID of the user
        @type targetClientDatabaseID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long targetClientDatabaseID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &targetClientDatabaseID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestComplainDelAll((uint64)schid, (uint64)targetClientDatabaseID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestComplainList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestComplainList(uint64 serverConnectionHandlerID, uint64 targetClientDatabaseID, const char *returnCode)
  /*
    @staticmethod
    def requestComplainList(serverConnectionHandlerID, targetClientDatabaseID, returnCode):
        """
        Requests the list of complains to a user. The event onComplainListEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param targetClientDatabaseID: the database ID of the user
        @type targetClientDatabaseID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long targetClientDatabaseID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &targetClientDatabaseID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestComplainList((uint64)schid, (uint64)targetClientDatabaseID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestConnectionInfo(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestConnectionInfo(uint64 serverConnectionHandlerID, anyID clientID, const char *returnCode)
  /*
    @staticmethod
    def requestConnectionInfo(serverConnectionHandlerID, clientID, returnCode):
        """
        Requests the connection info of a client. The event onConnectionInfoEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the ID of the client
        @type clientID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KI|s", &schid, &clientID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestConnectionInfo((uint64)schid, (anyID)clientID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestCreateDirectory(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestCreateDirectory(uint64 serverConnectionHandlerID, uint64 channelID, const char *channelPW, const char *directoryPath, const char *returnCode)
  /*
    @staticmethod
    def requestCreateDirectory(serverConnectionHandlerID, channelID, channelPW, directoryPath, returnCode):
        """
        Creates a directory in a channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param channelPW: the password of the channel, pass an empty string if channel is not password protected
        @type channelPW: string
        @param directoryPath: the complete path of the to be created directory
        @type directoryPath: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return:
        @rtype:
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  char* channelPW;
  char* directoryPath;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKss|s", &schid, &channelID, &channelPW, &directoryPath, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestCreateDirectory((uint64)schid, (uint64)channelID, channelPW, directoryPath, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestDeleteFile(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestDeleteFile(uint64 serverConnectionHandlerID, uint64 channelID, const char *channelPW, const char **file, const char *returnCode)
  /*
    @staticmethod
    def requestDeleteFile(serverConnectionHandlerID, channelID, channelPW, files, returnCode):
        """
        Deletes a list of files in a channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param channelPW: the password of the channel, pass an empty string if channel is not password protected
        @type channelPW: string
        @param files: a list of complete pathes of the file to delete
        @type files: list [string]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  char* channelPW;
  PyObject* pyfile;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKsO|s", &schid, &channelID, &channelPW, &pyfile, &returnCode))
    return NULL;

  QString error;
  char** file;
  if (!PYLIST_TO_ARRAY(char*, pyfile, error, &file, true)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.requestDeleteFile((uint64)schid, (uint64)channelID, channelPW, (const char**)file, returnCode);
  for (int i = 0; file[i] != NULL; ++i)
    free(file[i]);
  free(file);

  return Py_BuildValue("I", res);
}

PyObject* requestFile(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestFile(uint64 serverConnectionHandlerID, uint64 channelID, const char *channelPW, const char *file, int overwrite, int resume, const char *destinationDirectory, anyID *result, const char *returnCode)
  /*
    @staticmethod
    def requestFile(serverConnectionHandlerID, channelID, channelPW, file, overwrite, resume, destinationDirectory, returnCode):
        """
        Starts a filedownload from the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel in which the file is placed in
        @type channelID: int
        @param channelPW: the password of the channel, pass an empty string if the channel is not password protected
        @type channelPW: string
        @param file: the complete path to the file in the channel
        @type file: string
        @param overwrite: if set to 1 (or True) and a file with that name already exists will be overwritten
        @type overwrite: int or bool
        @param resume: if set to 1 (or True), a previously started filetransfer can be resumed
        @type resume: int or bool
        @param destinationDirectory: the path to the directory, where the downloaded fill will be placed in
        @type destinationDirectory: strin
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: a tuple, containing the errorcode and the ID of the filetransfer
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  char* channelPW;
  char* file;
  int overwrite;
  int resume;
  char* destinationDirectory;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKssiis|s", &schid, &channelID, &channelPW, &file, &overwrite, &resume, &destinationDirectory, &returnCode))
    return NULL;

  anyID result = 0;
  unsigned int res = ts3_funcs.requestFile((uint64)schid, (uint64)channelID, channelPW, file, overwrite, resume, destinationDirectory, &result, returnCode);

  return Py_BuildValue("(II)", res, result);
}

PyObject* requestFileInfo(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestFileInfo(uint64 serverConnectionHandlerID, uint64 channelID, const char *channelPW, const char *file, const char *returnCode)
  /*
    @staticmethod
    def requestFileInfo(serverConnectionHandlerID, channelID, channelPW, file, returnCode):
        """
        Requests the info to a file in a channel. The event onFileInfoEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param channelPW: the password of the channel, pass an empty string if the channel is not password protected
        @type channelPW: string
        @param file: the complete path to the file
        @type file: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  char* channelPW;
  char* file;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKss|s", &schid, &channelID, &channelPW, &file, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestFileInfo((uint64)schid, (uint64)channelID, channelPW, file, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestFileList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestFileList(uint64 serverConnectionHandlerID, uint64 channelID, const char *channelPW, const char *path, const char *returnCode)
  /*
    @staticmethod
    def requestFileList(serverConnectionHandlerID, channelID, channelPW, path, returnCode):
        """
        Requests the filelist of a channel. The events onFileListEvent and onFileListFinishedEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param channelPW: the password of the channel, pass an empty string if the channel is not password protected
        @type channelPW: string
        @param path: the path of the directory to be listed, pass '/' for the root path
        @type path: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  char* channelPW;
  char* path;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKss|s", &schid, &channelID, &channelPW, &path, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestFileList((uint64)schid, (uint64)channelID, channelPW, path, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestHotkeyInputDialog(PyObject* /*self*/, PyObject* args) {
  //void requestHotkeyInputDialog(const char *pluginID, const char *keyword, int isDown, void *qParentWindow)
  /*
    @staticmethod
    def requestHotkeyInputDialog(keyword, isDown, qParentWindow):
        """

        @param keyword:
        @type keyword:
        @param isDown:
        @type isDown:
        @param qParentWindow:
        @type qParentWindow:
        """
  */
  char* keyword;
  PyObject* pyisDown;
  PythonQtInstanceWrapper* pyqParentWindow = NULL;

  if (!PyArg_ParseTuple(args, "sO!|O!", &keyword, &PyBool_Type, &pyisDown, &PythonQtInstanceWrapper_Type, &pyqParentWindow))
    return NULL;

  ts3_funcs.requestHotkeyInputDialog(ts3_pluginid, keyword, PyObject_IsTrue(pyisDown) ? 1 : 0, pyqParentWindow ? pyqParentWindow->_obj : NULL);
  Py_RETURN_NONE;
}

PyObject* requestInfoUpdate(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestInfoUpdate(uint64 scHandlerID, enum PluginItemType itemType, uint64 itemID)
  /*
    @staticmethod
    def requestInfoUpdate(serverConnectionHandlerID, itemType, itemID):
        """
        Requests to update the info data.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param itemType: specifies, which info data update is requested, see ts3defines.PluginItemType
        @type itemType: int
        @param itemID: the ID of the item to update (only usefull if itemType != ts3defines.PluginItemType.PLUGIN_MENU_TYPE_GLOBAL)
        @type itemID: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  int itemType;
  unsigned long long itemID;

  if (!PyArg_ParseTuple(args, "KiK", &schid, &itemType, &itemID))
    return NULL;

  unsigned int res = ts3_funcs.requestInfoUpdate((uint64)schid, (enum PluginItemType)itemType, (uint64)itemID);

  return Py_BuildValue("I", res);
}

PyObject* requestIsTalker(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestIsTalker(uint64 serverConnectionHandlerID, int isTalkerRequest, const char *isTalkerRequestMessage, const char *returnCode)
  /*
    @staticmethod
    def requestIsTalker(serverConnectionHandlerID, isTalkerRequest, isTalkerRequestMessage, returnCode):
        """
        Requests talk power or revokes the talk power request.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param isTalkerRequest: if set to 1 (or True) requests talk power, if 0 (or False) revokes the talk power request
        @type isTalkerRequest: int or bool
        @param isTalkerRequestMessage: the message of the request
        @type isTalkerRequestMessage: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  int isTalkerRequest;
  char* isTalkerRequestMessage;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Kis|s", &schid, &isTalkerRequest, &isTalkerRequestMessage, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestIsTalker((uint64)schid, isTalkerRequest, isTalkerRequestMessage, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestMessageAdd(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestMessageAdd(uint64 serverConnectionHandlerID, const char *toClientUID, const char *subject, const char *message, const char *returnCode)
  /*
    @staticmethod
    def requestMessageAdd(serverConnectionHandlerID, toClientUID, subject, message, returnCode):
        """
        Sends an offline message to another user.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param toClientUID: the UID of the user
        @type toClientUID: string
        @param subject: the subject of the message
        @type subject: string
        @param message: the message
        @type message: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* toClientUID;
  char* subject;
  char* message;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Ksss|s", &schid, &toClientUID, &subject, &message, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestMessageAdd((uint64)schid, toClientUID, subject, message, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestMessageDel(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestMessageDel(uint64 serverConnectionHandlerID, uint64 messageID, const char *returnCode)
  /*
    @staticmethod
    def requestMessageDel(serverConnectionHandlerID, messageID, returnCode):
        """
        Deletes an offline message.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param messageID: the ID of the message
        @type messageID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long messageID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &messageID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestMessageDel((uint64)schid, (uint64)messageID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestMessageGet(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestMessageGet(uint64 serverConnectionHandlerID, uint64 messageID, const char *returnCode)
  /*
    @staticmethod
    def requestMessageGet(serverConnectionHandlerID, messageID, returnCode):
        """
        Requests an offline message defined by its ID. The event onMessageGetEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param messageID: the ID of the message
        @type messageID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long messageID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &messageID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestMessageGet((uint64)schid, (uint64)messageID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestMessageList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestMessageList(uint64 serverConnectionHandlerID, const char *returnCode)
  /*
    @staticmethod
    def requestMessageList(serverConnectionHandlerID, returnCode):
        """
        Requests the list of offline messages. The event onMessageListEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "K|s", &schid, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestMessageList((uint64)schid, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestMessageUpdateFlag(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestMessageUpdateFlag(uint64 serverConnectionHandlerID, uint64 messageID, int flag, const char *returnCode)
  /*
    @staticmethod
    def requestMessageUpdateFlag(serverConnectionHandlerID, messageID, flag, returnCode):
        """
        Sets the message read/unread flag of an offline message
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param messageID: the ID of the message
        @type messageID: int
        @param flag: set to 0 to set message as unread, set to 1 to set message as read
        @type flag:
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long messageID;
  int flag;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKi|s", &schid, &messageID, &flag, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestMessageUpdateFlag((uint64)schid, (uint64)messageID, flag, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestMuteClients(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestMuteClients(uint64 serverConnectionHandlerID, const anyID *clientIDArray, const char *returnCode)
  /*
    @staticmethod
    def requestMuteClients(serverConnectionHandlerID, clientIDArray, returnCode):
        """
        Mutes a list of clients.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientIDArray: a list of client IDs
        @type clientIDArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  PyObject* pyclientIDArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KO|s", &schid, &pyclientIDArray, &returnCode))
    return NULL;

  QString error;
  anyID* clientIDArray;
  if (!PYLIST_TO_ARRAY(anyID, pyclientIDArray, error, &clientIDArray, true)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.requestMuteClients((uint64)schid, clientIDArray, returnCode);
  free(clientIDArray);

  return Py_BuildValue("I", res);
}

PyObject* requestPermissionList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestPermissionList(uint64 serverConnectionHandler, const char *returnCode)
  /*
    @staticmethod
    def requestPermissionList(serverConnectionHandlerID, returnCode):
        """
        Requests the list of permissions available on the server. The events onPermissionListEvent and onPermissionListFinishedEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "K|s", &schid, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestPermissionList((uint64)schid, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestPermissionOverview(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestPermissionOverview(uint64 serverConnectionHandler, uint64 clientDBID, uint64 channelID, const char *returnCode)
  /*
    @staticmethod
    def requestPermissionOverview(serverConnectionHandlerID, clientDBID, channelID, returnCode):
        """
        Requests the permission overview of a user in a channel. The events onPermissionOverviewEvent and onPermissionOverviewFinishedEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientDBID: the database ID of the user
        @type clientDBID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long clientDBID;
  unsigned long long channelID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKK|s", &schid, &clientDBID, &channelID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestPermissionOverview((uint64)schid, (uint64)clientDBID, (uint64)channelID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestRenameFile(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestRenameFile(uint64 serverConnectionHandlerID, uint64 fromChannelID, const char *channelPW, uint64 toChannelID, const char *toChannelPW, const char *oldFile, const char *newFile, const char *returnCode)
  /*
    @staticmethod
    def requestRenameFile(serverConnectionHandlerID, fromChannelID, channelPW, toChannelID, toChannelPW, oldFile, newFile, returnCode):
        """
        Renames a file or moves it to another channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param fromChannelID: the ID of the channel, the file is currently placed in
        @type fromChannelID: int
        @param channelPW: the password of the channel, the file is currently placed in, pass an empty string if channel is not password protected
        @type channelPW: string
        @param toChannelID: the ID of the channel, the file should be placed in after, pass 0, if just renaming, not moving
        @type toChannelID: int
        @param toChannelPW: the password of the channel, to which the file should move to, pass an empty string if channel is not password protected; this is ignored, if just renaming, not moving
        @type toChannelPW: string
        @param oldFile: the complete path to the file
        @type oldFile: string
        @param newFile: the complete path to the new filename
        @type newFile: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long fromChannelID;
  char* channelPW;
  unsigned long long toChannelID;
  char* toChannelPW;
  char* oldFile;
  char* newFile;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKsKsss|s", &schid, &fromChannelID, &channelPW, &toChannelID, &toChannelPW, &oldFile, &newFile, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestRenameFile((uint64)schid, (uint64)fromChannelID, channelPW, (uint64)toChannelID, toChannelPW, oldFile, newFile, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestSendChannelTextMsg(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestSendChannelTextMsg(uint64 serverConnectionHandlerID, const char *message, uint64 targetChannelID, const char *returnCode)
  /*
    @staticmethod
    def requestSendChannelTextMsg(serverConnectionHandlerID, message, targetChannelID, returnCode):
        """
        Sends a text message to all clients in a channel.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param message: the message to send
        @type message: string
        @param targetChannelID: the ID of the channel
        @type targetChannelID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* message;
  unsigned long long targetChannelID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KsK|s", &schid, &message, &targetChannelID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestSendChannelTextMsg((uint64)schid, message, (uint64)targetChannelID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestSendClientQueryCommand(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestSendClientQueryCommand(uint64 serverConnectionHandlerID, const char *command, const char *returnCode)
  /*
    @staticmethod
    def requestSendClientQueryCommand(serverConnectionHandlerID, command, returnCode):
        """
        Requests to execute a clientquery command.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param command: the command to execute
        @type command: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* command;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Ks|s", &schid, &command, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestSendClientQueryCommand((uint64)schid, command, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestSendPrivateTextMsg(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestSendPrivateTextMsg(uint64 serverConnectionHandlerID, const char *message, anyID targetClientID, const char *returnCode)
  /*
    @staticmethod
    def requestSendPrivateTextMsg(serverConnectionHandlerID, message, targetClientID, returnCode):
        """
        Sends a private text message to a client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param message: the message to send
        @type message: string
        @param targetClientID: the ID of the client to send the message to
        @type targetClientID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* message;
  unsigned int targetClientID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KsI|s", &schid, &message, &targetClientID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestSendPrivateTextMsg((uint64)schid, message, (anyID)targetClientID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestSendServerTextMsg(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestSendServerTextMsg(uint64 serverConnectionHandlerID, const char *message, const char *returnCode)
  /*
    @staticmethod
    def requestSendServerTextMsg(serverConnectionHandlerID, message, returnCode):
        """
        Sends a text message to all clients on the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param message: the message to send
        @type message: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* message;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Ks|s", &schid, &message, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestSendServerTextMsg((uint64)schid, message, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerGroupAdd(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerGroupAdd(uint64 serverConnectionHandlerID, const char *groupName, int groupType, const char *returnCode)
  /*
    @staticmethod
    def requestServerGroupAdd(serverConnectionHandlerID, groupName, groupType, returnCode):
        """
        Adds a servergroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param groupName: the name of the group to create
        @type groupName: string
        @param groupType: type of the servergroup, see ts3defines.GroupType
        @type groupType: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long schid;
  char* groupName;
  int groupType;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Ksi|s", &schid, &groupName, &groupType, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestServerGroupAdd((uint64)schid, groupName, groupType, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerGroupAddClient(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerGroupAddClient(uint64 serverConnectionHandlerID, uint64 serverGroupID, uint64 clientDatabaseID, const char *returnCode)
  /*
    @staticmethod
    def requestServerGroupAddClient(serverConnectionHandlerID, serverGroupID, clientDatabaseID, returnCode):
        """
        Adds a user defined by his database ID to a servergroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverGroupID: the ID of the servergroup
        @type serverGroupID: int
        @param clientDatabaseID: the database ID of the user
        @type clientDatabaseID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long serverGroupID;
  unsigned long long clientDatabaseID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKK|s", &schid, &serverGroupID, &clientDatabaseID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestServerGroupAddClient((uint64)schid, (uint64)serverGroupID, (uint64)clientDatabaseID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerGroupAddPerm(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerGroupAddPerm(uint64 serverConnectionHandlerID, uint64 serverGroupID, int continueonerror, const unsigned int *permissionIDArray, const int *permissionValueArray, const int *permissionNegatedArray, const int *permissionSkipArray, int arraySize, const char *returnCode)
  /*
    @staticmethod
    def requestServerGroupAddPerm(serverConnectionHandlerID, serverGroupID, continueonerror, permissionIDArray, permissionValueArray, permissionNegatedArray, permissionSkipArray, returnCode):
        """
        Adds a list of permissions to a servergroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverGroupID: the ID of the servergroup
        @type serverGroupID: int
        @param continueonerror: if set to 1 (or True), if an error with a permission occurs, the other permissions will even though be handled
        @type continueonerror: int or bool
        @param permissionIDArray: list of permission IDs
        @type permissionIDArray: list [int]
        @param permissionValueArray: list of permission values, in order of the permissions in permissionIDArray
        @type permissionValueArray: list [int]
        @param permissionNegatedArray: list of permission negated values, in order of the permissions in permissionIDArray
        @type permissionNegatedArray: list [int]
        @param permissionSkipArray: list of permission skip values, in order of the permissions in permissionIDArray
        @type permissionSkipArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long serverGroupID;
  int continueonerror;
  PyObject* pypermissionIDArray;
  PyObject* pypermissionValueArray;
  PyObject* pypermissionNegatedArray;
  PyObject* pypermissionSkipArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKiOOOO|s", &schid, &serverGroupID, &continueonerror, &pypermissionIDArray, &pypermissionValueArray, &pypermissionNegatedArray, &pypermissionSkipArray, &returnCode))
    return NULL;

  QString error;
  unsigned int* permissionIDArray;
  if (!PYLIST_TO_ARRAY(unsigned int, pypermissionIDArray, error, &permissionIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionIDArray failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  int* permissionValueArray;
  if (!PYLIST_TO_ARRAY(int, pypermissionValueArray, error, &permissionValueArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionValueArray failed with error \"%1\"").arg(error).toUtf8().data());
    free(permissionIDArray);
    return NULL;
  }

  int* permissionNegatedArray;
  if (!PYLIST_TO_ARRAY(int, pypermissionNegatedArray, error, &permissionNegatedArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionNegatedArray failed with error \"%1\"").arg(error).toUtf8().data());
    free(permissionIDArray);
    free(permissionValueArray);
    return NULL;
  }

  int* permissionSkipArray;
  if (!PYLIST_TO_ARRAY(int, pypermissionSkipArray, error, &permissionSkipArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of permissionSkipArray failed with error \"%1\"").arg(error).toUtf8().data());
    free(permissionIDArray);
    free(permissionValueArray);
    free(permissionNegatedArray);
    return NULL;
  }

  if ((PyList_Size(pypermissionIDArray) != PyList_Size(pypermissionValueArray)) || (PyList_Size(pypermissionNegatedArray) != PyList_Size(pypermissionSkipArray)) || (PyList_Size(pypermissionIDArray) != PyList_Size(pypermissionNegatedArray))) {
    PyErr_SetString(PyExc_AttributeError, TRANS("List lengthes do not match").toUtf8().data());
    free(permissionIDArray);
    free(permissionValueArray);
    free(permissionNegatedArray);
    free(permissionSkipArray);
    return NULL;
  }

  unsigned int res = ts3_funcs.requestServerGroupAddPerm((uint64)schid, (uint64)serverGroupID, continueonerror, permissionIDArray, permissionValueArray, permissionNegatedArray, permissionSkipArray, PyList_Size(pypermissionIDArray), returnCode);
  free(permissionIDArray);
  free(permissionValueArray);
  free(permissionNegatedArray);
  free(permissionSkipArray);

  return Py_BuildValue("I", res);
}

PyObject* requestServerGroupClientList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerGroupClientList(uint64 serverConnectionHandlerID, uint64 serverGroupID, int withNames, const char *returnCode)
  /*
    @staticmethod
    def requestServerGroupClientList(serverConnectionHandlerID, serverGroupID, withNames, returnCode):
        """
        Requests the list of clients assigned to a servergroup. The event onServerGroupClientListEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverGroupID: the ID of the servergroup
        @type serverGroupID: int
        @param withNames: if set to 1 (or True), the event will contain the nick and uid of the user instead of empty strings
        @type withNames: int or bool
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long serverGroupID;
  int withNames;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKi|s", &schid, &serverGroupID, &withNames, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestServerGroupClientList((uint64)schid, (uint64)serverGroupID, withNames, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerGroupDel(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerGroupDel(uint64 serverConnectionHandlerID, uint64 serverGroupID, int force, const char *returnCode)
  /*
    @staticmethod
    def requestServerGroupDel(serverConnectionHandlerID, serverGroupID, force, returnCode):
        """
        Deletes a servergroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverGroupID: the ID of the servergroup
        @type serverGroupID: int
        @param force: if set to 1 (or True), even if there are users assigned to this servergroup, it will be deleted
        @type force: int or bool
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return:
        @rtype:
        """
  */
  unsigned long long schid;
  unsigned long long serverGroupID;
  int force;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKi|s", &schid, &serverGroupID, &force, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestServerGroupDel((uint64)schid, (uint64)serverGroupID, force, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerGroupDelClient(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerGroupDelClient(uint64 serverConnectionHandlerID, uint64 serverGroupID, uint64 clientDatabaseID, const char *returnCode)
  /*
    @staticmethod
    def requestServerGroupDelClient(serverConnectionHandlerID, serverGroupID, clientDatabaseID, returnCode):
        """
        Deletes a user defined by his database ID from a servergroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverGroupID: the ID of the servergroup
        @type serverGroupID: int
        @param clientDatabaseID: the database ID of the user
        @type clientDatabaseID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long serverGroupID;
  unsigned long long clientDatabaseID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKK|s", &schid, &serverGroupID, &clientDatabaseID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestServerGroupDelClient((uint64)schid, (uint64)serverGroupID, (uint64)clientDatabaseID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerGroupDelPerm(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerGroupDelPerm(uint64 serverConnectionHandlerID, uint64 serverGroupID, int continueOnError, const unsigned int *permissionIDArray, int arraySize, const char *returnCode)
  /*
    @staticmethod
    def requestServerGroupDelPerm(serverConnectionHandlerID, serverGroupID, continueOnError, permissionIDArray, returnCode):
        """
        Deletes a list of permissions from a servergroup.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverGroupID: the ID of the servergroup
        @type serverGroupID: int
        @param continueOnError: if set to 1 (or True), if an error with a permission occurs, the other permissions will even though be handled
        @type continueOnError: int or bool
        @param permissionIDArray: list of permission IDs
        @type permissionIDArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long serverGroupID;
  int continueOnError;
  PyObject* pypermissionIDArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKiO|s", &schid, &serverGroupID, &continueOnError, &pypermissionIDArray, &returnCode))
    return NULL;

  QString error;
  unsigned int* permissionIDArray;
  if (!PYLIST_TO_ARRAY(unsigned int, pypermissionIDArray, error, &permissionIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.requestServerGroupDelPerm((uint64)schid, (uint64)serverGroupID, continueOnError, permissionIDArray, PyList_Size(pypermissionIDArray), returnCode);
  free(permissionIDArray);

  return Py_BuildValue("I", res);
}

PyObject* requestServerGroupList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerGroupList(uint64 serverConnectionHandlerID, const char *returnCode)
  /*
    @staticmethod
    def requestServerGroupList(serverConnectionHandlerID, returnCode):
        """
        Requests the list of servergroups. The events onServerGroupListEvent and onServerGroupListFinishedEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "K|s", &schid, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestServerGroupList((uint64)schid, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerGroupPermList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerGroupPermList(uint64 serverConnectionHandlerID, uint64 serverGroupID, const char *returnCode)
  /*
    @staticmethod
    def requestServerGroupPermList(serverConnectionHandlerID, serverGroupID, returnCode):
        """
        Requests the list of permissions assigned to a servergroup. The events onServerGroupPermListEvent and onServerGroupPermListFinishedEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverGroupID: the ID of the servergroup
        @type serverGroupID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long serverGroupID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &serverGroupID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestServerGroupPermList((uint64)schid, (uint64)serverGroupID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerGroupsByClientID(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerGroupsByClientID(uint64 serverConnectionHandlerID, uint64 clientDatabaseID, const char *returnCode)
  /*
    @staticmethod
    def requestServerGroupsByClientID(serverConnectionHandlerID, clientDatabaseID, returnCode):
        """
        Requests all servergroups of a user defined by his database ID. The event onServerGroupByClientIDEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientDatabaseID: the database ID of the user
        @type clientDatabaseID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long clientDatabaseID;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KK|s", &schid, &clientDatabaseID, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestServerGroupsByClientID((uint64)schid, (uint64)clientDatabaseID, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerTemporaryPasswordAdd(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerTemporaryPasswordAdd(uint64 serverConnectionHandlerID, const char *password, const char *description, uint64 duration, uint64 targetChannelID, const char *targetChannelPW, const char *returnCode)
  /*
    @staticmethod
    def requestServerTemporaryPasswordAdd(serverConnectionHandlerID, password, description, duration, targetChannelID, targetChannelPW, returnCode):
        """
        Adds a temporary password to the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param password: the temporary password
        @type password: string
        @param description: the description of the temporary password
        @type description: string
        @param duration: the duration in seconds
        @type duration: int
        @param targetChannelID: the ID of the channel to which the accessing clients will join by default
        @type targetChannelID: int
        @param targetChannelPW: the password of the targetChannel, pass an empty string, if the channel is not password protected
        @type targetChannelPW: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* password;
  char* description;
  unsigned long long duration;
  unsigned long long targetChannelID;
  char* targetChannelPW;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KssKKs|s", &schid, &password, &description, &duration, &targetChannelID, &targetChannelPW, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestServerTemporaryPasswordAdd((uint64)schid, password, description, duration, targetChannelID, targetChannelPW, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerTemporaryPasswordDel(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerTemporaryPasswordDel(uint64 serverConnectionHandlerID, const char *password, const char *returnCode)
  /*
    @staticmethod
    def requestServerTemporaryPasswordDel(serverConnectionHandlerID, password, returnCode):
        """
        Deletes an existing temporary password.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param password: the password to delete
        @type password: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return:
        @rtype:
        """
  */
  unsigned long long schid;
  char* password;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Ks|s", &schid, &password, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestServerTemporaryPasswordDel((uint64)schid, password, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerTemporaryPasswordList(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerTemporaryPasswordList(uint64 serverConnectionHandlerID, const char *returnCode)
  /*
    @staticmethod
    def requestServerTemporaryPasswordList(serverConnectionHandlerID, returnCode):
        """
        Requests a list of existing temporary passwords. The event onServerTemporaryPasswordListEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "K|s", &schid, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.requestServerTemporaryPasswordList((uint64)schid, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestServerVariables(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestServerVariables(uint64 serverConnectionHandlerID)
  /*
    @staticmethod
    def requestServerVariables(serverConnectionHandlerID):
        """
        Requests all server variables of a serverconnection. The event onServerUpdatedEvent will be triggered.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  unsigned int res = ts3_funcs.requestServerVariables((uint64)schid);

  return Py_BuildValue("I", res);
}

PyObject* requestSetClientChannelGroup(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestSetClientChannelGroup(uint64 serverConnectionHandlerID, const uint64 *channelGroupIDArray, const uint64 *channelIDArray, const uint64 *clientDatabaseIDArray, int arraySize, const char *returnCode)
  /*
    @staticmethod
    def requestSetClientChannelGroup(serverConnectionHandlerID, channelGroupIDArray, channelIDArray, clientDatabaseIDArray, returnCode):
        """
        Adds a list of users to a list of channelgroups.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelGroupIDArray: a list of channelgroup IDs
        @type channelGroupIDArray: list [int]
        @param channelIDArray: a list of channel IDs
        @type channelIDArray: list [int]
        @param clientDatabaseIDArray: a list of client database IDs
        @type clientDatabaseIDArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  PyObject* pychannelGroupIDArray;
  PyObject* pychannelIDArray;
  PyObject* pyclientDatabaseIDArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KOOO|s", &schid, &pychannelGroupIDArray, &pychannelIDArray, &pyclientDatabaseIDArray, &returnCode))
    return NULL;

  QString error;
  uint64* channelGroupIDArray;
  if (!PYLIST_TO_ARRAY(uint64, pychannelGroupIDArray, error, &channelGroupIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of channelGroupIDArray failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  uint64* channelIDArray;
  if (!PYLIST_TO_ARRAY(uint64, pychannelIDArray, error, &channelIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of channelIDArray failed with error \"%1\"").arg(error).toUtf8().data());
    free(channelGroupIDArray);
    return NULL;
  }

  uint64* clientDatabaseIDArray;
  if (!PYLIST_TO_ARRAY(uint64, pyclientDatabaseIDArray, error, &clientDatabaseIDArray, false)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion of clientDatabaseIDArray failed with error \"%1\"").arg(error).toUtf8().data());
    free(channelGroupIDArray);
    free(channelIDArray);
    return NULL;
  }

  //pyListToArray had checked for PyLists
  if ((PyList_Size(pychannelGroupIDArray) != PyList_Size(pychannelIDArray)) || (PyList_Size(pychannelIDArray) != PyList_Size(pyclientDatabaseIDArray))) {
    PyErr_SetString(PyExc_AttributeError, TRANS("List lengthes do not match)").toUtf8().data());
    free(channelGroupIDArray);
    free(channelIDArray);
    free(clientDatabaseIDArray);
    return NULL;
  }

  unsigned int res = ts3_funcs.requestSetClientChannelGroup((uint64)schid, channelGroupIDArray, channelIDArray, clientDatabaseIDArray, PyList_Size(pychannelGroupIDArray), returnCode);

  return Py_BuildValue("I", res);
}

PyObject* requestUnmuteClients(PyObject* /*self*/, PyObject* args) {
  //unsigned int requestUnmuteClients(uint64 serverConnectionHandlerID, const anyID *clientIDArray, const char *returnCode)
  /*
    @staticmethod
    def requestUnmuteClients(serverConnectionHandlerID, clientIDArray, returnCode):
        """
        Unmutes a list of clients.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientIDArray: a list of client IDs
        @type clientIDArray: list [int]
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  PyObject* pyclientIDArray;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KO|s", &schid, &pyclientIDArray, &returnCode))
    return NULL;

  QString error;
  anyID* clientIDArray;
  if (!PYLIST_TO_ARRAY(anyID, pyclientIDArray, error, &clientIDArray, true)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.requestUnmuteClients((uint64)schid, clientIDArray, returnCode);
  free(clientIDArray);

  return Py_BuildValue("I", res);
}

PyObject* sendFile(PyObject* /*self*/, PyObject* args) {
  //unsigned int sendFile(uint64 serverConnectionHandlerID, uint64 channelID, const char *channelPW, const char *file, int overwrite, int resume, const char *sourceDirectory, anyID *result, const char *returnCode)
  /*
    @staticmethod
    def sendFile(serverConnectionHandlerID, channelID, channelPW, file, overwrite, resume, sourceDirectory, returnCode):
        """
        Starts a fileupload to the server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel in which the file will be placed in
        @type channelID: int
        @param channelPW: the password of the channel, pass an empty string if the channel is not password protected
        @type channelPW: string
        @param file: the complete path to the file in the channel
        @type file: string
        @param overwrite: if set to 1 (or True) and a file with that name already exists will be overwritten
        @type overwrite: int or bool
        @param resume: if set to 1 (or True), a previously started filetransfer can be resumed
        @type resume: int or bool
        @param sourceDirectory: the directory on the system, where the original file is placed in
        @type sourceDirectory: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: a tuple, containing the errorcode and the ID of the filetransfer
        @rtype: tuple (int, int)
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  char* channelPW;
  char* file;
  int overwrite;
  int resume;
  char* sourceDirectory;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKssiis|s", &schid, &channelID, &channelPW, &file, &overwrite, &resume, &sourceDirectory, &returnCode))
    return NULL;

  anyID result = 0;
  unsigned int res = ts3_funcs.sendFile((uint64)schid, (uint64)channelID, channelPW, file, overwrite, resume, sourceDirectory, &result, returnCode);

  return Py_BuildValue("(II)", res, result);
}

PyObject* sendPluginCommand(PyObject* /*self*/, PyObject* args) {
  //void sendPluginCommand(uint64 serverConnectionHandlerID, const char *pluginID, const char *command, int targetMode, const anyID *targetIDs, const char *returnCode)
  /*
    @staticmethod
    def sendPluginCommand(serverConnectionHandlerID, command, targetMode, targetIDs, returnCode):
        """
        Sends a plugin command to other users.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param command: the command string
        @type command: string
        @param targetMode: specifies, to whom the command will be send, see ts3defines.PluginTargetMode
        @type targetMode: int
        @param targetIDs: a list of client IDs, only needed if targetMode == ts3defines.PluginTargetMode.PluginCommandTarget_CLIENT
        @type targetIDs: list [int]
        """
  */
  unsigned long long schid;
  char* command;
  int targetMode;
  PyObject* pytargetIDs;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KsiO|s", &schid, &command, &targetMode, &pytargetIDs, &returnCode))
    return NULL;

  QString error;
  anyID* targetIDs;
  if (!PYLIST_TO_ARRAY(anyID, pytargetIDs, error, &targetIDs, true)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  ts3_funcs.sendPluginCommand((uint64)schid, ts3_pluginid, command, targetMode, targetIDs, returnCode);
  free(targetIDs);

  Py_RETURN_NONE;
}

PyObject* serverPropertyStringToFlag(PyObject* /*self*/, PyObject* args) {
  //unsigned int serverPropertyStringToFlag(const char *serverPropertyString, size_t *resultFlag)
  /*
    @staticmethod
    def serverPropertyStringToFlag(serverPropertyString):
        """
        Converts a server property name used in strings (eg the serverquery) to the corresponding flag.
        @param serverPropertyString: the lowercase string representation
        @type serverPropertyString: str
        @return: a tuple, containing the errorcode and the flag (see ts3defines.ClientProperties and ts3defines.ClientPropertiesRare)
        @rtype: tuple (int, int)
        """
  */
  char* serverPropertyString;

  if (!PyArg_ParseTuple(args, "s", &serverPropertyString))
    return NULL;

  size_t resultFlag;
  unsigned int res = ts3_funcs.serverPropertyStringToFlag(serverPropertyString, &resultFlag);

  return Py_BuildValue("(II)", res, (unsigned int)resultFlag);
}

PyObject* set3DWaveAttributes(PyObject* /*self*/, PyObject* args) {
  //unsigned int set3DWaveAttributes(uint64 serverConnectionHandlerID, uint64 waveHandle, const TS3_VECTOR *position)
  /*
    @staticmethod
    def set3DWaveAttributes(serverConnectionHandlerID, waveHandle, position):
        """
        Positions a wave file that was opened previously with playWaveFileHandle in 3D space.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param waveHandle: handle of the played wavefile sound returned by playWaveFileHandle
        @type waveHandle: int
        @param position: A tuple defining the 3D position of the sound
        @type position: tuple (float, float, float)
        @return: the errorcod
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long waveHandle;
  float x, y, z;

  if (!PyArg_ParseTuple(args, "KK(fff)", &schid, &waveHandle, &x, &y, &z))
    return NULL;

  TS3_VECTOR position = VECTOR(x, y, z);
  unsigned int res = ts3_funcs.set3DWaveAttributes((uint64)schid, (uint64)waveHandle, &position);

  return Py_BuildValue("I", res);
}

PyObject* setChannelVariableAsInt(PyObject* /*self*/, PyObject* args) {
  //unsigned int setChannelVariableAsInt(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, int value)
  /*
    @staticmethod
    def setChannelVariableAsInt(serverConnectionHandlerID, channelID, flag, value):
        """
        Sets a channel variable to a new int value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel, pass 0 to set a new channel's variables
        @type channelID: int
        @param flag: the flag to set
        @type flag: int
        @param value: the new value
        @type value: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  unsigned int flag;
  int value;

  if (!PyArg_ParseTuple(args, "KKIi", &schid, &channelID, &flag, &value))
    return NULL;

  unsigned int res = ts3_funcs.setChannelVariableAsInt((uint64)schid, (uint64)channelID, (size_t)flag, value);

  return Py_BuildValue("I", res);
}

PyObject* setChannelVariableAsString(PyObject* /*self*/, PyObject* args) {
  //unsigned int setChannelVariableAsString(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, const char *value)
  /*
    @staticmethod
    def setChannelVariableAsString(serverConnectionHandlerID, channelID, flag, value):
        """
        Sets a channel variable to a new string value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel, pass 0 to set a new channel's variables
        @type channelID: int
        @param flag: the flag to set
        @type flag: int
        @param value: the new value
        @type value: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  unsigned int flag;
  char* value;

  if (!PyArg_ParseTuple(args, "KKIs", &schid, &channelID, &flag, &value))
    return NULL;

  unsigned int res = ts3_funcs.setChannelVariableAsString((uint64)schid, (uint64)channelID, (size_t)flag, value);

  return Py_BuildValue("I", res);
}

PyObject* setChannelVariableAsUInt64(PyObject* /*self*/, PyObject* args) {
  //unsigned int setChannelVariableAsUInt64(uint64 serverConnectionHandlerID, uint64 channelID, size_t flag, uint64 value)
  /*
    @staticmethod
    def setChannelVariableAsUInt64(serverConnectionHandlerID, channelID, flag, value):
        """
        Sets a channel variable to a new unsigned long long int value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel, pass 0 to set a new channel's variables
        @type channelID: int
        @param flag: the flag to set
        @type flag: int
        @param value: the new value
        @type value: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  unsigned int flag;
  unsigned long long value;

  if (!PyArg_ParseTuple(args, "KKIK", &schid, &channelID, &flag, &value))
    return NULL;

  unsigned int res = ts3_funcs.setChannelVariableAsUInt64((uint64)schid, (uint64)channelID, (size_t)flag, (uint64)value);

  return Py_BuildValue("I", res);
}

PyObject* setClientSelfVariableAsInt(PyObject* /*self*/, PyObject* args) {
  //unsigned int setClientSelfVariableAsInt(uint64 serverConnectionHandlerID, size_t flag, int value)
  /*
    @staticmethod
    def setClientSelfVariableAsInt(serverConnectionHandlerID, flag, value):
        """
        Sets a variable of the own client to a new int value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param flag: the flag to set
        @type flag: int
        @param value: the new value
        @type value: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int flag;
  int value;

  if (!PyArg_ParseTuple(args, "KIi", &schid, &flag, &value))
    return NULL;

  unsigned int res = ts3_funcs.setClientSelfVariableAsInt((uint64)schid, (size_t)flag, value);

  return Py_BuildValue("I", res);
}

PyObject* setClientSelfVariableAsString(PyObject* /*self*/, PyObject* args) {
  //unsigned int setClientSelfVariableAsString(uint64 serverConnectionHandlerID, size_t flag, const char *value)
  /*
    @staticmethod
    def setClientSelfVariableAsString(serverConnectionHandlerID, flag, value):
        """
        Sets a variable of the own client to a new string value.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param flag: the flag to set
        @type flag: int
        @param value: the new value
        @type value: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int flag;
  char* value;

  if (!PyArg_ParseTuple(args, "KIs", &schid, &flag, &value))
    return NULL;

  unsigned int res = ts3_funcs.setClientSelfVariableAsString((uint64)schid, (size_t)flag, value);

  return Py_BuildValue("I", res);
}

PyObject* setClientVolumeModifier(PyObject* /*self*/, PyObject* args) {
  //unsigned int setClientVolumeModifier(uint64 serverConnectionHandlerID, anyID clientID, float value)
  /*
    @staticmethod
    def setClientVolumeModifier(serverConnectionHandlerID, clientID, value):
        """
        Sets the volume modifier of a client.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param clientID: the client's ID
        @type clientID: int
        @param value: the value to set
        @type value: float
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned int clientID;
  float value;

  if (!PyArg_ParseTuple(args, "KIf", &schid, &clientID, &value))
    return NULL;

  unsigned int res = ts3_funcs.setClientVolumeModifier((uint64)schid, (anyID)clientID, value);

  return Py_BuildValue("I", res);
}

PyObject* setPlaybackConfigValue(PyObject* /*self*/, PyObject* args) {
  //unsigned int setPlaybackConfigValue(uint64 serverConnectionHandlerID, const char *ident, const char *value)
  /*
    @staticmethod
    def setPlaybackConfigValue(serverConnectionHandlerID, ident, value):
        """
        Sets a playback option.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param ident: the playback option to reset
        @type ident: string
        @param value: the value to set
        @type value: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* ident;
  char* value;

  if (!PyArg_ParseTuple(args, "Kss", &schid, &ident, &value))
    return NULL;

  unsigned int res = ts3_funcs.setPlaybackConfigValue((uint64)schid, ident, value);

  return Py_BuildValue("I", res);
}

PyObject* setPluginMenuEnabled(PyObject* /*self*/, PyObject* args) {
  //void setPluginMenuEnabled(const char *pluginID, int menuID, int enabled)
  /*
    @staticmethod
    def setPluginMenuEnabled(menuID, enabled):
        """
        Enables or disables a menuitem. The menuID must be the global id, not the local id plugin developers set in menuItems. Retrieve it with PluginHost.globalMenuID.
        @param menuID: global id of the menuitem
        @type menuID: int
        @param enabled: set to True to enable it, False otherwise
        @type enabled: bool
        """
  */
  int menuID;
  PyObject* pyenabled;

  if (!PyArg_ParseTuple(args, "iO!", &menuID, &PyBool_Type, &pyenabled))
    return NULL;

  ts3_funcs.setPluginMenuEnabled(ts3_pluginid, menuID, PyObject_IsTrue(pyenabled) ? 1 : 0);
  Py_RETURN_NONE;
}

PyObject* setPreProcessorConfigValue(PyObject* /*self*/, PyObject* args) {
  //unsigned int setPreProcessorConfigValue(uint64 serverConnectionHandlerID, const char *ident, const char *value)
  /*
    @staticmethod
    def setPreProcessorConfigValue(serverConnectionHandlerID, ident, value):
        """
        Sets a sound preprocessor flag.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param ident: the flag to be set
        @type ident: string
        @param value: the value to set the flag to
        @type value: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* ident;
  char* value;

  if (!PyArg_ParseTuple(args, "Kss", &schid, &ident, &value))
    return NULL;

  unsigned int res = ts3_funcs.setPreProcessorConfigValue((uint64)schid, ident, value);

  return Py_BuildValue("I", res);
}

PyObject* showHotkeySetup(PyObject* /*self*/, PyObject* args) {
  //void showHotkeySetup()
  /*
    @staticmethod
    def showHotkeySetup():
        """

        """
  */
  if (!PyArg_ParseTuple(args, ""))
    return NULL;

  ts3_funcs.showHotkeySetup();
  Py_RETURN_NONE;
}

PyObject* spawnNewServerConnectionHandler(PyObject* /*self*/, PyObject* args) {
  //unsigned int spawnNewServerConnectionHandler(int port, uint64 *result)
  /*
    @staticmethod
    def spawnNewServerConnectionHandler(port):
        """
        Creates a new server connection handler and receive its ID.
        @param port: Port the client should bind on. Specify zero to let the operating system chose any free port
        @type port: int
        @return: A tuple, containig the errorcode and the resulting ID
        @rtype: tuple (int, int)
        """
  */
  int port;

  if (!PyArg_ParseTuple(args, "i", &port))
    return NULL;

  uint64 result;
  unsigned int res = ts3_funcs.spawnNewServerConnectionHandler(port, &result);

  return Py_BuildValue("(IK)", res, (unsigned long long)result);
}

PyObject* startConnection(PyObject* /*self*/, PyObject* args) {
  //unsigned int startConnection(uint64 serverConnectionHandlerID, const char *identity, const char *ip, unsigned int port, const char *nickname, const char **defaultChannelArray, const char *defaultChannelPassword, const char *serverPassword)
  /*
    @staticmethod
    def startConnection(serverConnectionHandlerID, identity, ip, port, nickname, defaultChannelArray, defaultChannelPassword, serverPassword):
        """
        Starts a connection to the given server.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param identity: the client's identity
        @type identity: string
        @param ip: hostname or ip of the server
        @type ip: string
        @param port: port of the server
        @type port: int
        @param nickname: the client's nickname
        @type nickname: string
        @param defaultChannelArray: list of strings defining the path to a channel on the server, pass empty list to join in server's default channel
        @type defaultChannelArray: list [string]
        @param defaultChannelPassword: password of the default channel, pass an empty string if not using defaultChannelArray or channel is not password protected
        @type defaultChannelPassword: string
        @param serverPassword: password of the server, pass an empty string if the server is not password protected
        @type serverPassword: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* identity;
  char* ip;
  unsigned int port;
  char* nickname;
  PyObject* pydefaultChannelArray;
  char* defaultChannelPassword;
  char* serverPassword;

  if (!PyArg_ParseTuple(args, "KssIsOss", &schid, &identity, &ip, &port, &nickname, &pydefaultChannelArray, &defaultChannelPassword, &serverPassword))
    return NULL;

  QString error;
  char** defaultChannelArray;
  if (!PYLIST_TO_ARRAY(char*, pydefaultChannelArray, error, &defaultChannelArray, true)) {
    PyErr_SetString(PyExc_AttributeError, TRANS("Conversion failed with error \"%1\"").arg(error).toUtf8().data());
    return NULL;
  }

  unsigned int res = ts3_funcs.startConnection((uint64)schid, identity, ip, port, nickname, (const char**)defaultChannelArray, defaultChannelPassword, serverPassword);
  for (int i = 0; defaultChannelArray[i] != NULL; ++i)
    free(defaultChannelArray[i]);
  free(defaultChannelArray);

  return Py_BuildValue("I", res);
}

PyObject* startVoiceRecording(PyObject* /*self*/, PyObject* args) {
  //unsigned int startVoiceRecording(uint64 serverConnectionHandlerID)
  /*
    @staticmethod
    def startVoiceRecording(serverConnectionHandlerID):
        """
        Starts voice recording on a serverconnection.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  unsigned int res = ts3_funcs.startVoiceRecording((uint64)schid);

  return Py_BuildValue("I", res);
}

PyObject* stopConnection(PyObject* /*self*/, PyObject* args) {
  //unsigned int stopConnection(uint64 serverConnectionHandlerID, const char *quitMessage)
  /*
    @staticmethod
    def stopConnection(serverConnectionHandlerID, quitMessage):
        """
        Stops the connection of a serverconnection.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param quitMessage: a message displayed when leaving the server encoded in UTF-8
        @type quitMessage: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* quitMessage;

  if (!PyArg_ParseTuple(args, "Ks", &schid, &quitMessage))
    return NULL;

  unsigned int res = ts3_funcs.stopConnection((uint64)schid, quitMessage);

  return Py_BuildValue("I", res);
}

PyObject* stopVoiceRecording(PyObject* /*self*/, PyObject* args) {
  //unsigned int stopVoiceRecording(uint64 serverConnectionHandlerID)
  /*
    @staticmethod
    def stopVoiceRecording(serverConnectionHandlerID):
        """
        Stops voice recording on a serverconnection
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;

  if (!PyArg_ParseTuple(args, "K", &schid))
    return NULL;

  unsigned int res = ts3_funcs.stopVoiceRecording((uint64)schid);

  return Py_BuildValue("I", res);
}

PyObject* systemset3DListenerAttributes(PyObject* /*self*/, PyObject* args) {
  //unsigned int systemset3DListenerAttributes(uint64 serverConnectionHandlerID, const TS3_VECTOR *position, const TS3_VECTOR *forward, const TS3_VECTOR *up)
  /*
    @staticmethod
    def systemset3DListenerAttributes(serverConnectionHandlerID, position, forward, up):
        """
        Sets the position, velocity and orientation of the own client in 3D space
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param position: A tuple defining the 3D position, pass None to ignore
        @type position: tuple (float, float, float)
        @param forward: A tuple defining the forward orientation of the listener. The vector must be of unit length and perpendicular to the up vector. Pass None to ignore.
        @type forward: tuple (float, float, float)
        @param up: A tuple defining the upward orientation of the listener. The vector must be of unit length and perpendicular to the forward vector. Pass None to ignore.
        @type up: tuple (float, float, float)
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  PyObject* pyposition;
  PyObject* pyforward;
  PyObject* pyup;

  if (!PyArg_ParseTuple(args, "KOOO", &schid, &pyposition, &pyforward, &pyup))
    return NULL;

  TS3_VECTOR position;
  TS3_VECTOR forward;
  TS3_VECTOR up;

  float x, y, z;
  if (pyposition != Py_None) {
    if (!PyTuple_Check(pyposition) || !PyArg_ParseTuple(pyposition, "fff", &x, &y, &z)) {
      PyErr_SetString(PyExc_AttributeError, TRANS("No valid TS3Vector tuple at argument position").toUtf8().data());
      return NULL;
    }
    position = VECTOR(x, y, z);
  }

  if (pyforward != Py_None) {
    if (!PyTuple_Check(pyforward) || !PyArg_ParseTuple(pyforward, "fff", &x, &y, &z)) {
      PyErr_SetString(PyExc_AttributeError, TRANS("No valid TS3Vector tuple at argument forward").toUtf8().data());
      return NULL;
    }
    forward = VECTOR(x, y, z);
  }

  if (pyup != Py_None) {
    if (!PyTuple_Check(pyup) || !PyArg_ParseTuple(pyup, "fff", &x, &y, &z)) {
      PyErr_SetString(PyExc_AttributeError, TRANS("No valid TS3Vector tuple at argument up").toUtf8().data());
      return NULL;
    }
    up = VECTOR(x, y, z);
  }

  unsigned int res = ts3_funcs.systemset3DListenerAttributes((uint64)schid, pyposition == Py_None ? NULL : &position, pyforward == Py_None ? NULL : &forward, pyup == Py_None ? NULL : &up);

  return Py_BuildValue("I", res);
}

PyObject* systemset3DSettings(PyObject* /*self*/, PyObject* args) {
  //unsigned int systemset3DSettings(uint64 serverConnectionHandlerID, float distanceFactor, float rolloffScale)
  /*
    @staticmethod
    def systemset3DSettings(serverConnectionHandlerID, distanceFactor, rolloffScale):
        """
        Adjust 3D sound system settings.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param distanceFactor: relative distance factor. Default is 1.0 = 1 meter
        @type distanceFactor: float
        @param rolloffScale: Scaling factor for 3D sound rolloff. Defines how fast sound volume will attenuate. As higher the value, as faster the sound is toned with increasing distance.
        @type rolloffScale: float
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  float distanceFactor;
  float rolloffScale;

  if (!PyArg_ParseTuple(args, "Kff", &schid, &distanceFactor, &rolloffScale))
    return NULL;

  unsigned int res = ts3_funcs.systemset3DSettings((uint64)schid, distanceFactor, rolloffScale);

  return Py_BuildValue("I", res);
}

PyObject* unregisterCustomDevice(PyObject* /*self*/, PyObject* args) {
  //unsigned int unregisterCustomDevice(const char *deviceID)
  /*
    @staticmethod
    def unregisterCustomDevice(deviceID):
        """
        Unregisters a custom device, previously registered with registerCustomDevice.
        @param deviceID: the ID of the device, used in registerCustomDevice
        @type deviceID: string
        @return: the errorcode
        @rtype: int
        """
  */
  //Unregistering a custom device will automatically close the device:
  char* deviceID;

  if (!PyArg_ParseTuple(args, "s", &deviceID))
    return NULL;

  unsigned int res = ts3_funcs.unregisterCustomDevice(deviceID);

  return Py_BuildValue("I", res);
}

PyObject* urlsToBB(PyObject* /*self*/, PyObject* args) {
  //void urlsToBB(const char *text, char *result, size_t maxLen)
  /*
    @staticmethod
    def urlsToBB(text, maxLen = 256):
        """
        Converts an url to the BB-code respresentation.
        @param text: the url
        @type text: string
        @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
        @type maxLen: int
        @return: the BB-code representation
        @rtype: string
        """
  */
  char* text;
  unsigned int maxLen = 256;

  if (!PyArg_ParseTuple(args, "s|I", &text, &maxLen))
    return NULL;

  if (maxLen == 0) {
    PyErr_SetString(PyExc_AttributeError, TRANS("maxLen must be greater than zero").toUtf8().data());
    return NULL;
  }

  char* result = CPPALLOC(char, maxLen);
  ts3_funcs.urlsToBB(text, result, maxLen);

  PyObject* ret = Py_BuildValue("s", result);
  CPPDELARR(result);

  return ret;
}

PyObject* verifyChannelPassword(PyObject* /*self*/, PyObject* args) {
  //unsigned int verifyChannelPassword(uint64 serverConnectionHandlerID, uint64 channelID, const char *channelPassword, const char *returnCode)
  /*
    @staticmethod
    def verifyChannelPassword(serverConnectionHandlerID, channelID, channelPassword, returnCode):
        """
        Verifies the password to a channel. The result can be checked in onServerErrorEvent.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param channelID: the ID of the channel
        @type channelID: int
        @param channelPassword: the password to be verified
        @type channelPassword: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  unsigned long long channelID;
  char* channelPassword;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "KKs|s", &schid, &channelID, &channelPassword, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.verifyChannelPassword((uint64)schid, (uint64)channelID, channelPassword, returnCode);

  return Py_BuildValue("I", res);
}

PyObject* verifyServerPassword(PyObject* /*self*/, PyObject* args) {
  //unsigned int verifyServerPassword(uint64 serverConnectionHandlerID, const char *serverPassword, const char *returnCode)
  /*
    @staticmethod
    def verifyServerPassword(serverConnectionHandlerID, serverPassword, returnCode):
        """
        Verifies the password to a server. The result can be checked in onServerErrorEvent.
        @param serverConnectionHandlerID: the ID of the serverconnection
        @type serverConnectionHandlerID: int
        @param serverPassword: the password to be verified
        @type serverPassword: string
        @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
        @type returnCode: string
        @return: the errorcode
        @rtype: int
        """
  */
  unsigned long long schid;
  char* serverPassword;
  char* returnCode = NULL;

  if (!PyArg_ParseTuple(args, "Ks|s", &schid, &serverPassword, &returnCode))
    return NULL;

  unsigned int res = ts3_funcs.verifyServerPassword((uint64)schid, serverPassword, returnCode);

  return Py_BuildValue("I", res);
}

