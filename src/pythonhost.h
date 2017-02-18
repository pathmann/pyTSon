#ifndef PYTHONHOST_H
#define PYTHONHOST_H

#include <Python.h>

#include "singleton.h"
#include "ts3callbackarguments.h"

#include "teamspeak/public_definitions.h"
#include "plugin_definitions.h"

#include <QObject>
#include <QString>
#include <QDir>

#include <thread>

class PythonHost: public QObject, public singleton<PythonHost> {
    friend class singleton<PythonHost>;
    Q_OBJECT
  public:
    bool init(QString& error);
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
    void callInMainThread(ts3callbackarguments* args);
  protected slots:
    void onCallInMainThread(ts3callbackarguments* args);
  protected:
    bool setupDirectories(QString& error);
    bool isReady();

    bool setModuleSearchpath(QString& error);
    bool setSysPath(QString& error);

    QString formatError(const QString& fallback);

    void initPythonQt();
  private:
    PythonHost();
    ~PythonHost();

    QDir m_scriptsdir;
    QDir m_includedir;
    QDir m_libdir;
    QDir m_dynloaddir;
    QDir m_sitepackdir;
    QDir m_base;
    wchar_t* m_interpreter;
    PyObject* m_pluginmod;
    PyObject* m_pmod;
    PyObject* m_pyhost;
    PyObject* m_callmeth;
    PyObject* m_trace;
    bool m_inited;

    std::thread::id m_mainthread;
};

#endif // PYTHONHOST_H
