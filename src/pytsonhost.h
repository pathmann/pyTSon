#ifndef PYTSONHOST_H
#define PYTSONHOST_H

#include <Python.h>

#include <QObject>

#include "pythonhost.h"

#include "teamspeak/public_definitions.h"
#include "plugin_definitions.h"

#include <thread>

#include "ts3callbackarguments.h"

class pytsonhost: public QObject, PythonHost {
    Q_OBJECT

  public:
    pytsonhost();
    ~pytsonhost();

    bool init(const QDir& basedir, QString& error);
    void shutdown();

    void freeMemory(void* data);
    void configure(void* qParentWidget);
    int processCommand(uint64 schid, const char* command);
    void infoData(uint64 schid, uint64 id, enum PluginItemType type, char** data);
    void initMenus(struct PluginMenuItem*** menuItems, char** menuIcon);
    void initHotkeys(struct PluginHotkey*** hotkeys);
    int onServerErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, const char* extraMessage);
    int onTextMessageEvent(uint64 schid, anyID targetMode, anyID toID, anyID fromID, const char* fromName, const char* fromUniqueIdentifier, const char* message, int ffIgnored);
    int onClientPokeEvent(uint64 schid, anyID fromClientID, const char* pokerName, const char* pokerUniqueIdentity, const char* message, int ffIgnored);
    int onServerPermissionErrorEvent(uint64 schid, const char* errorMessage, unsigned int error, const char* returnCode, unsigned int failedPermissionID);
    void onUserLoggingMessageEvent(const char* logMessage, int logLevel, const char* logChannel, uint64 logID, const char* logTime, const char* completeLogString);
    void onEditPlaybackVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels);
    void onEditPostProcessVoiceDataEvent(uint64 schid, anyID clientID, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);
    void onEditMixedPlaybackVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, const unsigned int* channelSpeakerArray, unsigned int* channelFillMask);
    void onEditCapturedVoiceDataEvent(uint64 schid, short* samples, int sampleCount, int channels, int* edited);
    void onCustom3dRolloffCalculationClientEvent(uint64 schid, anyID clientID, float distance, float* volume);
    void onCustom3dRolloffCalculationWaveEvent(uint64 schid, uint64 waveHandle, float distance, float* volume);
    void onFileTransferStatusEvent(anyID transferID, unsigned int status, const char* statusMessage, uint64 remotefileSize, uint64 schid);

    bool callMethod(PyObject** ret, QString& error, const char* format, ...);

  signals:
    void callInMainThread(const ts3callbackarguments args);

  protected slots:
      void onCallInMainThread(const ts3callbackarguments args);

  private:
    PyObject* m_pluginmod;
    PyObject* m_pmod;
    PyObject* m_pyhost;
    PyObject* m_callmeth;

    std::thread::id m_mainthread;
};

#endif
