#include "pytson.h"

#include "pythonhost.h"
#include "ts3logdispatcher.h"
#include "global_shared.h"

#ifndef WIN32
#include <dlfcn.h>
#include <QDebug>

void unixDllMain() {
  Dl_info dl_info;

  if (dladdr((void *)loadVersion, &dl_info) != 0) {
    loadVersion(QString(dl_info.dli_fname));
    return;
  }

  setVersion("unkown");
}
#else
#define PATH_LEN 256

BOOL DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/) {
  wchar_t path[PATH_LEN];
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
      if (GetModuleFileName(hModule, path, PATH_LEN) != 0)
        loadVersion(QString::fromWCharArray(path));
      else setVersion("unknown");

      break;
    case DLL_PROCESS_DETACH:
      freeVersion();
      break;
    default:
      break;
  }
  
  return TRUE;
}

#endif

void loadVersion(const QString &thislibpath) {
  QFileInfo info(thislibpath);
  QDir dir = info.absoluteDir();

  if (dir.cd("pyTSon")) {
    if (dir.exists("VERSION")) {
      QFile file(dir.absoluteFilePath("VERSION"));

      if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray ver = file.readLine().trimmed();
        file.close();

        setVersion(ver.trimmed());
        return;
      }
    }
  }

  setVersion("unkown");
}

void setVersion(const QByteArray& ver) {
  pytson_version = new char[ver.length() + 1];
  strncpy(pytson_version, ver, ver.length());
  pytson_version[ver.length()] = '\0';
}

void freeVersion() {
  delete[] pytson_version;
}

const char* ts3plugin_name() {
  return "pyTSon";
}

const char* ts3plugin_version() {
  return pytson_version;
}

int ts3plugin_apiVersion() {
  return 23;
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
  pyhost->freeMemory(data);
}

void ts3plugin_registerPluginID(const char* id) {
  ts3_pluginid = (char*)malloc((strlen(id) +1) * sizeof(char));
  strncpy(ts3_pluginid, id, strlen(id) +1);
}

int ts3plugin_init() {
  qRegisterMetaType<quintptr>("quintptr");

  ts3logdispatcher::instance()->init("pyTSon");

  char path[256];
  ts3_funcs.getPluginPath(path, 256, ts3_pluginid);

  QString error;
  pyhost = new pytsonhost;
  if (pyhost->init(QDir(path), error)) {
    return 0;
  }
  else {
    ts3logdispatcher::instance()->add(QString("Initializing PythonHost failed with \"%1\"").arg(error), LogLevel_ERROR);
    return 1;
  }
}

void ts3plugin_shutdown() {
  pyhost->shutdown();
  delete pyhost;

  if (ts3_pluginid) {
    free(ts3_pluginid);
    ts3_pluginid = NULL;
  }
}

int ts3plugin_offersConfigure() {
  return PLUGIN_OFFERS_CONFIGURE_QT_THREAD;
}

void ts3plugin_configure(void* /*handle*/, void* qParentWidget) {
  pyhost->configure(qParentWidget);
}

const char* ts3plugin_commandKeyword() {
  return "py";
}

int ts3plugin_processCommand(uint64 schid, const char* command) {
  return pyhost->processCommand(schid, command);
}

const char* ts3plugin_infoTitle() {
  return "pyTSon";
}

void ts3plugin_infoData(uint64 schid, uint64 id, enum PluginItemType type, char** data) {
  pyhost->infoData(schid, id, type, data);
}

void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, char** menuIcon) {
  pyhost->initMenus(menuItems, menuIcon);
}

void ts3plugin_initHotkeys(struct PluginHotkey*** hotkeys) {
  pyhost->initHotkeys(hotkeys);
}

int ts3plugin_requestAutoload() {
  return 0;
}

int ts3plugin_onServerErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage) {
  return pyhost->onServerErrorEvent(schid, errorMessage, error, returnCode, extraMessage);
}

int ts3plugin_onTextMessageEvent(uint64 schid, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored) {
  return pyhost->onTextMessageEvent(schid, targetMode, toID, fromID, fromName, fromUniqueIdentifier, message, ffIgnored);
}

int ts3plugin_onClientPokeEvent(uint64 schid, anyID fromClientID, const char* pokerName, const char* pokerUniqueIdentity, const char* message, int ffIgnored) {
  return pyhost->onClientPokeEvent(schid, fromClientID, pokerName, pokerUniqueIdentity, message, ffIgnored);
}

int ts3plugin_onServerPermissionErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, unsigned int failedPermissionID) {
  return pyhost->onServerPermissionErrorEvent(schid, errorMessage, error, returnCode, failedPermissionID);
}

void ts3plugin_onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString) {
  pyhost->onUserLoggingMessageEvent(logMessage, logLevel, logChannel, logID, logTime, completeLogString);
}

void ts3plugin_onEditPlaybackVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels) {
  pyhost->onEditPlaybackVoiceDataEvent(schid, clientID, samples, sampleCount, channels);
}

void ts3plugin_onEditPostProcessVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
  pyhost->onEditPostProcessVoiceDataEvent(schid, clientID, samples, sampleCount, channels, channelSpeakerArray, channelFillMask);
}

void ts3plugin_onEditMixedPlaybackVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask) {
  pyhost->onEditMixedPlaybackVoiceDataEvent(schid, samples, sampleCount, channels, channelSpeakerArray, channelFillMask);
}

void ts3plugin_onEditCapturedVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, int* edited) {
  pyhost->onEditCapturedVoiceDataEvent(schid, samples, sampleCount, channels, edited);
}

void ts3plugin_onCustom3dRolloffCalculationClientEvent(uint64 schid, anyID clientID, float distance, float* volume) {
  pyhost->onCustom3dRolloffCalculationClientEvent(schid, clientID, distance, volume);
}

void ts3plugin_onCustom3dRolloffCalculationWaveEvent(uint64 schid, uint64 waveHandle, float distance, float* volume) {
  pyhost->onCustom3dRolloffCalculationWaveEvent(schid, waveHandle, distance, volume);
}

void ts3plugin_onFileTransferStatusEvent(anyID transferID, unsigned int status, const char* statusMessage, uint64 remotefileSize, uint64 schid) {
  pyhost->onFileTransferStatusEvent(transferID, status, statusMessage, remotefileSize, schid);
}
