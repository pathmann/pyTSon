#ifndef PYTSON_H
#define PYTSON_H

#ifdef WIN32
#define EXPORT_SYMBOL __declspec(dllexport)
#else
#define EXPORT_SYMBOL __attribute__ ((visibility("default")))
#endif

#include <Python.h>

#include "teamspeak/public_definitions.h"
#include "plugin_definitions.h"
#include "ts3_functions.h"

#ifdef __cplusplus
extern "C" {
#endif


EXPORT_SYMBOL const char* ts3plugin_name();
EXPORT_SYMBOL const char* ts3plugin_version();
EXPORT_SYMBOL int ts3plugin_apiVersion();
EXPORT_SYMBOL const char* ts3plugin_author();
EXPORT_SYMBOL const char* ts3plugin_description();
EXPORT_SYMBOL void ts3plugin_setFunctionPointers(const struct TS3Functions functions);
EXPORT_SYMBOL void ts3plugin_freeMemory(void* data);
EXPORT_SYMBOL void ts3plugin_registerPluginID(const char* id);
EXPORT_SYMBOL int ts3plugin_init();
EXPORT_SYMBOL void ts3plugin_shutdown();
EXPORT_SYMBOL int ts3plugin_offersConfigure();
EXPORT_SYMBOL void ts3plugin_configure(void* handle, void* qParentWidget);
EXPORT_SYMBOL const char* ts3plugin_commandKeyword();
EXPORT_SYMBOL int ts3plugin_processCommand(uint64 schid, const char* command);
EXPORT_SYMBOL const char* ts3plugin_infoTitle();
EXPORT_SYMBOL void ts3plugin_infoData(uint64 schid, uint64 id, enum PluginItemType type, char** data);
EXPORT_SYMBOL void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, char** menuIcon);
EXPORT_SYMBOL void ts3plugin_initHotkeys(struct PluginHotkey*** hotkeys);
EXPORT_SYMBOL int ts3plugin_requestAutoload();
EXPORT_SYMBOL int ts3plugin_onServerErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage);
EXPORT_SYMBOL int ts3plugin_onTextMessageEvent(uint64 schid, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored);
EXPORT_SYMBOL int ts3plugin_onClientPokeEvent(uint64 schid, anyID fromClientID, const char* pokerName, const char* pokerUniqueIdentity, const char* message, int ffIgnored);
EXPORT_SYMBOL int ts3plugin_onServerPermissionErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, unsigned int failedPermissionID);
EXPORT_SYMBOL void ts3plugin_onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString);
EXPORT_SYMBOL void ts3plugin_onEditPlaybackVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels);
EXPORT_SYMBOL void ts3plugin_onEditPostProcessVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);
EXPORT_SYMBOL void ts3plugin_onEditMixedPlaybackVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);
EXPORT_SYMBOL void ts3plugin_onEditCapturedVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, int* edited);
EXPORT_SYMBOL void ts3plugin_onCustom3dRolloffCalculationClientEvent(uint64 schid, anyID clientID, float distance, float* volume);
EXPORT_SYMBOL void ts3plugin_onCustom3dRolloffCalculationWaveEvent(uint64 schid, uint64 waveHandle, float distance, float* volume);

#ifdef __cplusplus
}
#endif


#endif // PYTSON_H
