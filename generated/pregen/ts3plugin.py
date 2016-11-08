import sys, os, glob

import ts3, ts3defines

import importlib, traceback

from configparser import ConfigParser
from pytsonui import PythonConsole, ConfigurationDialog
from PythonQt.QtGui import QFont, QColor

class PluginMount(type):
    def __init__(cls, name, bases, attrs):
        super(PluginMount, cls).__init__(name, bases, attrs)
        if not hasattr(PluginHost, 'plugins'):
            PluginHost.plugins = {}
            PluginHost.active = {}
        else:
            for a in ['name', 'version', 'apiVersion', 'author', 'description', 'offersConfigure', 'commandKeyword', 'infoTitle', 'menuItems', 'hotkeys']:
                if not hasattr(cls, a):
                    err = ts3.logMessage("Plugin not loaded, missing required attribute %s" % a, ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginMount.init", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Plugin not loaded, missing required attribute %s" % a)

            if not cls.name in PluginHost.plugins:
                PluginHost.plugins[cls.name] = cls
            else:
                err = ts3.logMessage("Error loading python plugin %s, already registered or a plugin with that name already exist" % cls.name, ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginMount.init", 0)
                if err != ts3defines.ERROR_ok:
                    print("Error loading python plugin %s, already registered or a plugin with that name already exist" % cls.name)


class PluginHost(object):
    defaultConfig = [("general", [("differentApi", "False")]), ("plugins", []), ("console", [("backgroundColor", "#000000"), ("textColor", "#FFFFFF"), ("fontFamily", "Monospace"), ("fontSize", "12"), ("tabcomplete", "True"), ("spaces", "True"), ("tabwidth", "2"), ("width", "800"), ("height", "600"), ("startup", ""), ("silentStartup", "False")])]


    @classmethod
    def setupConfig(cls):
        for (section, options) in cls.defaultConfig:
            if not cls.cfg.has_section(section):
                cls.cfg.add_section(section)

            for (o, v) in options:
                if not cls.cfg.has_option(section, o):
                    cls.cfg.set(section, o, v)

    @classmethod
    def init(cls):
        cls.shell = None
        cls.confdlg = None

        cls.modules = {}

        cls.menus = {}
        cls.hotkeys = {}

        cls.cfg = ConfigParser()
        cls.cfg.read(os.path.join(ts3.getConfigPath(), "pyTSon.conf"))

        cls.setupConfig()

        cls.reload()
        cls.start()

    @classmethod
    def start(cls):
        #start plugin if config says so, or if new plugin and requestAutoload is True
        for key in cls.plugins:
            load = False
            if not cls.cfg.has_option("plugins", key):
                if cls.plugins[key].requestAutoload:
                    load = True
            elif cls.cfg.getboolean("plugins", key, fallback=False):
                load = True

            if load:
                if cls.plugins[key].apiVersion != 20:
                    if not cls.cfg.getboolean("general", "differentApi", fallback=False):
                        continue

                try:
                    cls.active[key] = cls.plugins[key]()
                    cls.cfg.set("plugins", key, "True")
                except:
                    err = ts3.logMessage("Error starting python plugin %s: %s" % (key, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.start", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Error starting python plugin %s: %s" % (key, traceback.format_exc()))

        #restore reloaded menus
        for globid, (p, locid) in cls.menus.items():
            if p in cls.active:
                cls.menus[globid] = (cls.active[p], locid)

        #restore reloaded hotkeys
        for keyword, (p, lockey) in cls.hotkeys.items():
            if p in cls.active:
                cls.hotkeys[keyword] = (cls.active[p], lockey)

    @classmethod
    def shutdown(cls):
        if cls.shell:
            cls.shell.delete()
        cls.shell = None
        if cls.confdlg:
            cls.confdlg.delete()
        cls.confdlg = None

        #store config
        with open(os.path.join(ts3.getConfigPath(), "pyTSon.conf"), "w") as f:
            cls.cfg.write(f)

        #stop all plugins
        for key, p in cls.active.items():
            try:
                p.stop()
            except:
                print("Error stopping python plugin %s: %s" % (key, traceback.format_exc()))

        cls.active = {}

        #save local menu ids
        for globid, (p, locid) in cls.menus.items():
            #previously reloaded?
            if not type(p) is str:
                cls.menus[globid] = (p.name, locid)

        #save local hotkeys
        for keyword, (p, lockey) in cls.hotkeys.items():
            if not type(p) is str:
                cls.hotkeys[keyword] = (p.name, lockey)

    @classmethod
    def activate(cls, pname):
        if pname in cls.plugins:
            try:
                cls.active[pname] = cls.plugins[pname]()
                cls.cfg.set("plugins", pname, "True")

                for globid, (p, locid) in cls.menus.items():
                    if type(p) is str and p == pname:
                        cls.menus[globid] = (cls.active[p], locid)

                for keyword, (p, lockey) in cls.hotkeys.items():
                    if type(p) is str and p == pname:
                        cls.hotkeys[keyword] = (cls.active[p], lockey)
            except:
                err = ts3.logMessage("Error starting python plugin %s: %s" % (pname, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.activate", 0)
                if err != ts3defines.ERROR_ok:
                    print("Error starting python plugin %s: %s" % (pname, traceback.format_exc()))

    @classmethod
    def deactivate(cls, pname):
        if pname in cls.active:
            try:
                #remove hotkeys
                for key in cls.hotkeys:
                    if cls.hotkeys[key][0].name == pname:
                        cls.hotkeys[key] = (pname, cls.hotkeys[key][1])

                #remove menuItems
                for key in cls.menus:
                    if cls.menus[key][0].name == pname:
                        cls.menus[key] = (pname, cls.menus[key][1])

                cls.active[pname].stop()
                del cls.active[pname]
                cls.cfg.set("plugins", pname, "False")
            except:
                err = ts3.logMessage("Error stopping python plugin %s: %s" % (pname, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.deactivate", 0)
                if err != ts3defines.ERROR_ok:
                    print("Error stopping python plugin %s: %s" % (pname, traceback.format_exc()))

    @classmethod
    def reload(cls):
        #stop all running modules
        for key, p in cls.active.items():
            try:
                p.stop()
            except:
                err = ts3.logMessage("Error stopping python plugin %s: %s" % (key, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.reload", 0)
                if err != ts3defines.ERROR_ok:
                    print("Error stopping python plugin %s: %s" % (key, traceback.format_exc()))

        cls.active = {}
        cls.plugins = {}

        #import all modules
        for f in glob.glob(os.path.join(ts3.getPluginPath(), "pyTSon", "scripts", "*.py")):
            if os.path.isfile(f) and os.path.basename(f) != "ts3plugin.py":
                base = os.path.basename(f[:-3])
                try:
                    if base in cls.modules:
                        cls.modules[base] = importlib.reload(cls.modules[base])
                    else:
                        cls.modules[base] = importlib.__import__(base)
                except:
                    err = ts3.logMessage("Error loading python plugin from %s: %s" % (os.path.basename(f), traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.init", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Error loading python plugin from %s: %s" % (os.path.basename(f), traceback.format_exc()))

        #save local menu ids
        for globid, (p, locid) in cls.menus.items():
            #previously reloaded?
            if not type(p) is str:
                cls.menus[globid] = (p.name, locid)

        #save local hotkeys
        for keyword, (p, lockey) in cls.hotkeys.items():
            if not type(p) is str:
                cls.hotkeys[keyword] = (p.name, lockey)

    @classmethod
    def showScriptingConsole(cls):
        if not cls.shell:
            cls.shell = PythonConsole(cls.cfg.getboolean("console", "tabcomplete"), cls.cfg.getboolean("console", "spaces"), cls.cfg.getint("console", "tabwidth"), QFont(cls.cfg.get("console", "fontFamily"), cls.cfg.getint("console", "fontSize")), QColor(cls.cfg.get("console", "backgroundColor")), QColor(cls.cfg.get("console", "textColor")), cls.cfg.getint("console", "width"), cls.cfg.getint("console", "height"), cls.cfg.get("console", "startup"), cls.cfg.getboolean("console", "silentStartup"))
            cls.shell.connect("destroyed()", cls.scriptingConsoleDestroyed)
        cls.shell.show()

    @classmethod
    def scriptingConsoleDestroyed(cls):
        cls.cfg.set("console", "width", str(cls.shell.width))
        cls.cfg.set("console", "height", str(cls.shell.height))

    @classmethod
    def configure(cls, mainwindow=None):
        if not cls.confdlg:
            cls.confdlg = ConfigurationDialog(cls.cfg, cls, mainwindow)

        cls.confdlg.show()
        cls.confdlg.raise_()
        cls.confdlg.activateWindow()

    @classmethod
    def callMethod(cls, name, *args):
        meth = getattr(PluginHost, name, None)
        if meth:
            return meth(*args)

        ret = []
        for key, p in cls.active.items():
            meth = getattr(p, name, None)

            if meth:
                try:
                    ret.append(meth(*args))
                except:
                    print("Error calling method of plugin %s: %s" % (key, traceback.format_exc()))

        for r in ret:
            if r:
                return True

        return False

    @classmethod
    def processCommand(cls, schid, command):
        tokens = command.split(' ')

        if len(tokens) == 0 or tokens[0] == "":
            return False

        for key, p in cls.active.items():
            if p.commandKeyword == tokens[0]:
                try:
                    return p.processCommand(schid, " ".join(tokens[1:]))
                except:
                    err = ts3.logMessage("Error calling processCommand of python plugin %s: %s" % (p.name, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.processCommand", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Error calling processCommand of python plugin %s: %s" % (p.name, traceback.format_exc()))

        return False

    @classmethod
    def infoData(cls, schid, aid, atype):
        ret = []
        for key, p in cls.active.items():
            if p.infoTitle is not None:
                try:
                    ret.append(p.infoTitle)
                    ret += p.infoData(schid, aid, atype)
                except:
                    err = ts3.logMessage("Error calling infoData of python plugin %s: %s" % (key, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.infoData", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Error calling infoData of python plugin %s: %s" % (key, traceback.format_exc()))

        return ret

    @classmethod
    def initMenus(cls):
        nextid = 2
        cls.menus = {}
        ret = [(ts3defines.PluginMenuType.PLUGIN_MENU_TYPE_GLOBAL, 0, "Console", ""), (ts3defines.PluginMenuType.PLUGIN_MENU_TYPE_GLOBAL, 1, "Settings", "")]

        for key, p in cls.active.items():
            for (atype, locid, text, icon) in p.menuItems:
                ret.append((atype, nextid, text, icon))
                cls.menus[nextid] = (p, locid)
                nextid += 1

        return ret

    @classmethod
    def globalMenuID(cls, plugin, localid):
        for key, (p, locid) in cls.items():
            if p == plugin and locid == localid:
                return key

        return None

    @classmethod
    def initHotkeys(cls):
        nextkey = 2
        cls.hotkeys = {}
        ret = [("0", "Show the python scripting console"), ("1", "Show the pyTSon settings dialog")]

        for key, p in cls.active.items():
            for (lockey, description) in p.hotkeys:
                ret.append((str(nextkey), description))
                cls.hotkeys[str(nextkey)] = (p, lockey)
                nextkey += 1

        return ret


    @classmethod
    def onMenuItemEvent(cls, schid, atype, menuItemID, selectedItemID):
        if menuItemID == 0:
            cls.showScriptingConsole()
            return
        elif menuItemID == 1:
            cls.configure()
            return

        if menuItemID in cls.menus:
            (plugin, locid) = cls.menus[menuItemID]
            if type(plugin) is not str:
                #if plugin was reloaded, but menuItem does not exist anymore
                try:
                    plugin.onMenuItemEvent(schid, atype, locid, selectedItemID)
                except:
                    err = ts3.logMessage("Error calling onMenuItemEvent of python plugin %s: %s" % (plugin.name, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.onMenuItemEvent", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Error calling onMenuItemEvent of python plugin %s: %s" % (plugin.name, traceback.format_exc()))

    @classmethod
    def globalHotkeyKeyword(cls, plugin, localkeyword):
        for key, (p, lockey) in cls.hotkeys.items():
            if p == plugin and lockey == localkeyword:
                return key

        return None

    @classmethod
    def onHotkeyEvent(cls, keyword):
        if keyword == "0":
            cls.showScriptingConsole()
            return
        elif keyword == "1":
            cls.configure()
            return

        if keyword in cls.hotkeys:
            (plugin, lockey) = cls.hotkeys[keyword]
            if type(plugin) is not str:
                try:
                    plugin.onHotkeyEvent(lockey)
                except:
                    err = ts3.logMessage("Error calling onHotkeyEvent of python plugin %s: %s" % (plugin.name, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.onHotkeyEvent", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Error calling onHotkeyEvent of python plugin %s: %s" % (plugin.name, traceback.format_exc()))

    @classmethod
    def onEditPlaybackVoiceDataEvent(cls, schid, clientID, samples, channels):
        changed = False
        for key, p in cls.active.items():
            (customized, newsamples) = p.onEditPlaybackVoiceDataEvent(schid, clientID, samples, channels)
            if customized and len(newsamples) == len(samples):
                samples = newsamples
                changed = True

        return (changed, samples)

    @classmethod
    def onEditPostProcessVoiceDataEvent(cls, schid, clientID, samples, channels, channelSpeakerArray, channelFillMask):
        changed = False
        for key, p in cls.active.items():
            (customized, newsamples, newchannelFillMask) = p.onEditPostProcessVoiceDataEvent(schid, clientID, samples, channelSpeakerArray, channelFillMask)
            if customized and len(newsamples) == len(samples):
                samples = newsamples
                channelFillMask = newchannelFillMask
                changed = True

        return (changed, samples, channelFillMask)

    @classmethod
    def onEditMixedPlaybackVoiceDataEvent(cls, schid, samples, channels, channelSpeakerArray, channelFillMask):
        changed = False
        for key, p in cls.active.items():
            (customized, newsamples, newchannelFillMask) = p.onEditMixedPlaybackVoiceDataEvent(schid, samples, channels, channelSpeakerArray, channelFillMask)
            if customized and len(newsamples) == len(samples):
                samples = newsamples
                channelFillMask = newchannelFillMask
                changed = True

        return (changed, samples, channelFillMask)

    @classmethod
    def onEditCapturedVoiceDataEvent(cls, schid, samples, channels, edited):
        changed = False
        for key, p in cls.active.items():
            (customized, newsamples, newedited) = p.onEditCapturedVoiceDataEvent(schid, samples, channels, edited)
            if customized and len(newsamples) == len(samples):
                samples = newsamples
                edited = newedited
                changed = True

        return (changed, samples, edited)

    @classmethod
    def onCustom3dRolloffCalculationClientEvent(cls, schid, clientID, distance, volume):
        for key, p in cls.active.items():
            volume = p.onCustom3dRolloffCalculationClientEvent(schid, clientID, distance, volume)

        return volume

    @classmethod
    def onCustom3dRolloffCalculationWaveEvent(cls, schid, waveHandle, distance, volume):
        for key, p in cls.active.items():
            volume = p.onCustom3dRolloffCalculationWaveEvent(schid, waveHandle, distance, volume)

        return volume


class ts3plugin(object, metaclass=PluginMount):
    """
    requestAutoload = False
    name = "__ts3plugin__"
    version = "1.0"
    apiVersion = 20
    author = "Thomas \"PLuS\" Pathmann"
    description = "This is the baseclass for all ts3 python plugins"
    offersConfigure = False
    commandKeyword = "py"
    infoTitle = "pyTSon" #pass None to not show any info
    menuItems = []#[(ts3defines.PluginMenuType.PLUGIN_MENU_TYPE_CLIENT, 0, "text", "icon.png")]
    hotkeys = []#[("keyword", "description")]
    """

    def __init__(self):
        pass

    def stop(self):
        pass

    def configure(self, qParentWidget):
        pass

    def infoData(self, schid, aid, atype):
        return []

    def processCommand(self, schid, command):
        return True

    def onServerErrorEvent(self, schid, errorMessage, error, returnCode, extraMessage):
        return False

    def onTextMessageEvent(self, schid, targetMode, toID, fromID, fromName, fromUniqueIdentifier, message, ffIgnored):
        return False

    def onClientPokeEvent(self, schid, fromClientID, pokerName, pokerUniqueIdentity, message, ffIgnored):
        return False

    def onServerPermissionErrorEvent(self, schid, errorMessage, error, returnCode, failedPermissionID):
        pass

    def onUserLoggingMessageEvent(self, logMessage, logLevel, logChannel, logID, logTime, completeLogString):
        pass

    def onMenuItemEvent(self, schid, atype, menuItemID, selectedItemID):
        pass

    def onHotkeyEvent(self, keyword):
        pass

    def onEditPlaybackVoiceDataEvent(self, schid, clientID, samples, channels):
        return(False, [])

    def onEditPostProcessVoiceDataEvent(self, schid, clientID, samples, channels, channelSpeakerArray, channelFillMask):
        return (False, [], 0)

    def onEditMixedPlaybackVoiceDataEvent(self, schid, samples, channels, channelSpeakerArray, channelFillMask):
        return (False, [], 0)

    def onEditCapturedVoiceDataEvent(self, schid, samples, channels, edited):
        return (False, [], 0)

    def onCustom3dRolloffCalculationClientEvent(self, schid, clientID, distance, volume):
        return volume

    def onCustom3dRolloffCalculationWaveEvent(self, schid, waveHandle, distance, volume):
        return volume

    def onClientPermListFinishedEvent(self, serverConnectionHandlerID, clientDatabaseID):
        pass

    def onNewChannelEvent(self, serverConnectionHandlerID, channelID, channelParentID):
        pass

    def onClientPermListEvent(self, serverConnectionHandlerID, clientDatabaseID, permissionID, permissionValue, permissionNegated, permissionSkip):
        pass

    def onClientChatComposingEvent(self, serverConnectionHandlerID, clientID, clientUniqueIdentity):
        pass

    def onUpdateClientEvent(self, serverConnectionHandlerID, clientID, invokerID, invokerName, invokerUniqueIdentifier):
        pass

    def onChannelClientPermListFinishedEvent(self, serverConnectionHandlerID, channelID, clientDatabaseID):
        pass

    def onPermissionOverviewEvent(self, serverConnectionHandlerID, clientDatabaseID, channelID, overviewType, overviewID1, overviewID2, permissionID, permissionValue, permissionNegated, permissionSkip):
        pass

    def onPermissionListFinishedEvent(self, serverConnectionHandlerID):
        pass

    def onServerGroupClientAddedEvent(self, serverConnectionHandlerID, clientID, clientName, clientUniqueIdentity, serverGroupID, invokerClientID, invokerName, invokerUniqueIdentity):
        pass

    def onServerUpdatedEvent(self, serverConnectionHandlerID):
        pass

    def onChannelPermListFinishedEvent(self, serverConnectionHandlerID, channelID):
        pass

    def onChannelUnsubscribeFinishedEvent(self, serverConnectionHandlerID):
        pass

    def onComplainListEvent(self, serverConnectionHandlerID, targetClientDatabaseID, targetClientNickName, fromClientDatabaseID, fromClientNickName, complainReason, timestamp):
        pass

    def onChannelSubscribeFinishedEvent(self, serverConnectionHandlerID):
        pass

    def onConnectionInfoEvent(self, serverConnectionHandlerID, clientID):
        pass

    def onChannelClientPermListEvent(self, serverConnectionHandlerID, channelID, clientDatabaseID, permissionID, permissionValue, permissionNegated, permissionSkip):
        pass

    def onChannelPermListEvent(self, serverConnectionHandlerID, channelID, permissionID, permissionValue, permissionNegated, permissionSkip):
        pass

    def onClientSelfVariableUpdateEvent(self, serverConnectionHandlerID, flag, oldValue, newValue):
        pass

    def onClientNeededPermissionsEvent(self, serverConnectionHandlerID, permissionID, permissionValue):
        pass

    def onPermissionListGroupEndIDEvent(self, serverConnectionHandlerID, groupEndID):
        pass

    def onChannelGroupListFinishedEvent(self, serverConnectionHandlerID):
        pass

    def onServerGroupClientListEvent(self, serverConnectionHandlerID, serverGroupID, clientDatabaseID, clientNameIdentifier, clientUniqueID):
        pass

    def onClientBanFromServerEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, kickerID, kickerName, kickerUniqueIdentifier, time, kickMessage):
        pass

    def onBanListEvent(self, serverConnectionHandlerID, banid, ip, name, uid, creationTime, durationTime, invokerName, invokercldbid, invokeruid, reason, numberOfEnforcements, lastNickName):
        pass

    def onClientIDsFinishedEvent(self, serverConnectionHandlerID):
        pass

    def onDelChannelEvent(self, serverConnectionHandlerID, channelID, invokerID, invokerName, invokerUniqueIdentifier):
        pass

    def onServerLogFinishedEvent(self, serverConnectionHandlerID, lastPos, fileSize):
        pass

    def onServerGroupByClientIDEvent(self, serverConnectionHandlerID, name, serverGroupList, clientDatabaseID):
        pass

    def onChannelDescriptionUpdateEvent(self, serverConnectionHandlerID, channelID):
        pass

    def onMessageListEvent(self, serverConnectionHandlerID, messageID, fromClientUniqueIdentity, subject, timestamp, flagRead):
        pass

    def onPluginCommandEvent(self, serverConnectionHandlerID, pluginName, pluginCommand):
        pass

    def onClientDBIDfromUIDEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientDatabaseID):
        pass

    def onChannelPasswordChangedEvent(self, serverConnectionHandlerID, channelID):
        pass

    def onClientChannelGroupChangedEvent(self, serverConnectionHandlerID, channelGroupID, channelID, clientID, invokerClientID, invokerName, invokerUniqueIdentity):
        pass

    def onClientNamefromUIDEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientDatabaseID, clientNickName):
        pass

    def onServerGroupListEvent(self, serverConnectionHandlerID, serverGroupID, name, atype, iconID, saveDB):
        pass

    def onIncomingClientQueryEvent(self, serverConnectionHandlerID, commandText):
        pass

    def onServerLogEvent(self, serverConnectionHandlerID, logMsg):
        pass

    def onServerGroupPermListFinishedEvent(self, serverConnectionHandlerID, serverGroupID):
        pass

    def onChannelSubscribeEvent(self, serverConnectionHandlerID, channelID):
        pass

    def onClientMoveMovedEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, moverID, moverName, moverUniqueIdentifier, moveMessage):
        pass

    def onClientChatClosedEvent(self, serverConnectionHandlerID, clientID, clientUniqueIdentity):
        pass

    def onClientMoveSubscriptionEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility):
        pass

    def onFileInfoEvent(self, serverConnectionHandlerID, channelID, name, size, datetime):
        pass

    def onPermissionOverviewFinishedEvent(self, serverConnectionHandlerID):
        pass

    def onChannelGroupPermListEvent(self, serverConnectionHandlerID, channelGroupID, permissionID, permissionValue, permissionNegated, permissionSkip):
        pass

    def onTalkStatusChangeEvent(self, serverConnectionHandlerID, status, isReceivedWhisper, clientID):
        pass

    def onClientIDsEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientID, clientName):
        pass

    def onServerTemporaryPasswordListEvent(self, serverConnectionHandlerID, clientNickname, uniqueClientIdentifier, description, password, timestampStart, timestampEnd, targetChannelID, targetChannelPW):
        pass

    def onFileListFinishedEvent(self, serverConnectionHandlerID, channelID, path):
        pass

    def onHotkeyEvent(self, keyword):
        pass

    def currentServerConnectionChanged(self, serverConnectionHandlerID):
        pass

    def onClientKickFromServerEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, kickerID, kickerName, kickerUniqueIdentifier, kickMessage):
        pass

    def onMenuItemEvent(self, serverConnectionHandlerID, atype, menuItemID, selectedItemID):
        pass

    def onChannelUnsubscribeEvent(self, serverConnectionHandlerID, channelID):
        pass

    def onChannelGroupPermListFinishedEvent(self, serverConnectionHandlerID, channelGroupID):
        pass

    def onFileListEvent(self, serverConnectionHandlerID, channelID, path, name, size, datetime, atype, incompletesize, returnCode):
        pass

    def onPlaybackShutdownCompleteEvent(self, serverConnectionHandlerID):
        pass

    def onClientServerQueryLoginPasswordEvent(self, serverConnectionHandlerID, loginPassword):
        pass

    def onChannelMoveEvent(self, serverConnectionHandlerID, channelID, newChannelParentID, invokerID, invokerName, invokerUniqueIdentifier):
        pass

    def onFileTransferStatusEvent(self, transferID, status, statusMessage, remotefileSize, serverConnectionHandlerID):
        pass

    def onHotkeyRecordedEvent(self, keyword, key):
        pass

    def onMessageGetEvent(self, serverConnectionHandlerID, messageID, fromClientUniqueIdentity, subject, message, timestamp):
        pass

    def onClientKickFromChannelEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, kickerID, kickerName, kickerUniqueIdentifier, kickMessage):
        pass

    def onServerGroupListFinishedEvent(self, serverConnectionHandlerID):
        pass

    def onUpdateChannelEvent(self, serverConnectionHandlerID, channelID):
        pass

    def onClientNamefromDBIDEvent(self, serverConnectionHandlerID, uniqueClientIdentifier, clientDatabaseID, clientNickName):
        pass

    def onServerStopEvent(self, serverConnectionHandlerID, shutdownMessage):
        pass

    def onClientDisplayNameChanged(self, serverConnectionHandlerID, clientID, displayName, uniqueClientIdentifier):
        pass

    def onPermissionListEvent(self, serverConnectionHandlerID, permissionID, permissionName, permissionDescription):
        pass

    def onUpdateChannelEditedEvent(self, serverConnectionHandlerID, channelID, invokerID, invokerName, invokerUniqueIdentifier):
        pass

    def onClientMoveTimeoutEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, timeoutMessage):
        pass

    def onServerEditedEvent(self, serverConnectionHandlerID, editerID, editerName, editerUniqueIdentifier):
        pass

    def onServerConnectionInfoEvent(self, serverConnectionHandlerID):
        pass

    def onClientNeededPermissionsFinishedEvent(self, serverConnectionHandlerID):
        pass

    def onSoundDeviceListChangedEvent(self, modeID, playOrCap):
        pass

    def onServerGroupPermListEvent(self, serverConnectionHandlerID, serverGroupID, permissionID, permissionValue, permissionNegated, permissionSkip):
        pass

    def onConnectStatusChangeEvent(self, serverConnectionHandlerID, newStatus, errorNumber):
        pass

    def onAvatarUpdated(self, serverConnectionHandlerID, clientID, avatarPath):
        pass

    def onServerGroupClientDeletedEvent(self, serverConnectionHandlerID, clientID, clientName, clientUniqueIdentity, serverGroupID, invokerClientID, invokerName, invokerUniqueIdentity):
        pass

    def onClientMoveEvent(self, serverConnectionHandlerID, clientID, oldChannelID, newChannelID, visibility, moveMessage):
        pass

    def onNewChannelCreatedEvent(self, serverConnectionHandlerID, channelID, channelParentID, invokerID, invokerName, invokerUniqueIdentifier):
        pass

    def onChannelGroupListEvent(self, serverConnectionHandlerID, channelGroupID, name, atype, iconID, saveDB):
        pass

