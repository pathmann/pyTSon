#include "pytson.h"

#include "pythonhost.h"
#include "ts3logdispatcher.h"
#include "global_shared.h"

const char* ts3plugin_name() {
  return "pyTSon";
}

const char* ts3plugin_version() {
  return "1.0.1";
}

int ts3plugin_apiVersion() {
  return 20;
}

const char* ts3plugin_author() {
  return "Thomas \"PLuS\" Pathmann";
}

const char* ts3plugin_description() {
  return "Plugin to customize the TS3 client with python plugins and/or scripts";
}

void ts3plugin_setFunctionPointers(const struct TS3Functions functions) {
  ts3_funcs = functions;
}

void ts3plugin_freeMemory(void* data) {
  PythonHost::instance()->freeMemory(data);
}

void ts3plugin_registerPluginID(const char* id) {
  ts3_pluginid = (char*)malloc((strlen(id) +1) * sizeof(char));
  strncpy(ts3_pluginid, id, strlen(id) +1);
}

int ts3plugin_init() {
  ts3logdispatcher::instance()->init("pyTSon");

  QString error;
  if (PythonHost::instance()->init(error)) {
    return 0;
  }
  else {
    ts3logdispatcher::instance()->add(QString("Initializing PythonHost failed with \"%1\"").arg(error), LogLevel_ERROR);
    return 1;
  }
}

void ts3plugin_shutdown() {
  PythonHost::instance()->shutdown();

  if (ts3_pluginid) {
    free(ts3_pluginid);
    ts3_pluginid = NULL;
  }
}

int ts3plugin_offersConfigure() {
  return PLUGIN_OFFERS_CONFIGURE_QT_THREAD;
}

void ts3plugin_configure(void* /*handle*/, void* qParentWidget) {
  PythonHost::instance()->configure(qParentWidget);
}

const char* ts3plugin_commandKeyword() {
  return "py";
}

int ts3plugin_processCommand(uint64 schid, const char* command) {
  return PythonHost::instance()->processCommand(schid, command);
}

const char* ts3plugin_infoTitle() {
  return "pyTSon";
}

void ts3plugin_infoData(uint64 schid, uint64 id, enum PluginItemType type, char** data) {
  PythonHost::instance()->infoData(schid, id, type, data);
}

void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, char** menuIcon) {
  PythonHost::instance()->initMenus(menuItems, menuIcon);
}

void ts3plugin_initHotkeys(struct PluginHotkey*** hotkeys) {
  PythonHost::instance()->initHotkeys(hotkeys);
}

int ts3plugin_requestAutoload() {
  return 0;
}

int ts3plugin_onServerErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
  return PythonHost::instance()->onServerErrorEvent(schid, errorMessage, error, returnCode, extraMessage);
}

int ts3plugin_onTextMessageEvent(uint64 schid, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored) {
  return PythonHost::instance()->onTextMessageEvent(schid, targetMode, toID, fromID, fromName, fromUniqueIdentifier, message, ffIgnored);
}

int ts3plugin_onClientPokeEvent(uint64 schid, anyID fromClientID, const char* pokerName, const char* pokerUniqueIdentity, const char* message, int ffIgnored) {
  return PythonHost::instance()->onClientPokeEvent(schid, fromClientID, pokerName, pokerUniqueIdentity, message, ffIgnored);
}

int ts3plugin_onServerPermissionErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, unsigned int failedPermissionID) {
  return PythonHost::instance()->onServerPermissionErrorEvent(schid, errorMessage, error, returnCode, failedPermissionID);
}

void ts3plugin_onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString) {
  PythonHost::instance()->onUserLoggingMessageEvent(logMessage, logLevel, logChannel, logID, logTime, completeLogString);
}

void ts3plugin_onEditPlaybackVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels) {
  PythonHost::instance()->onEditPlaybackVoiceDataEvent(schid, clientID, samples, sampleCount, channels);
}

void ts3plugin_onEditPostProcessVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
  PythonHost::instance()->onEditPostProcessVoiceDataEvent(schid, clientID, samples, sampleCount, channels, channelSpeakerArray, channelFillMask);
}

void ts3plugin_onEditMixedPlaybackVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
  PythonHost::instance()->onEditMixedPlaybackVoiceDataEvent(schid, samples, sampleCount, channels, channelSpeakerArray, channelFillMask);
}

void ts3plugin_onEditCapturedVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, int* edited) {
  PythonHost::instance()->onEditCapturedVoiceDataEvent(schid, samples, sampleCount, channels, edited);
}

void ts3plugin_onCustom3dRolloffCalculationClientEvent(uint64 schid, anyID clientID, float distance, float* volume) {
  PythonHost::instance()->onCustom3dRolloffCalculationClientEvent(schid, clientID, distance, volume);
}

void ts3plugin_onCustom3dRolloffCalculationWaveEvent(uint64 schid, uint64 waveHandle, float distance, float* volume) {
  PythonHost::instance()->onCustom3dRolloffCalculationWaveEvent(schid, waveHandle, distance, volume);
}
