import sys, os, glob

import ts3lib
import ts3defines, pytson

import importlib, traceback
import json

from configparser import ConfigParser
from pytsonui import PythonConsole, ConfigurationDialog
from PythonQt.QtGui import QFont, QColor, QMessageBox
from PythonQt.QtCore import QUrl, QTimer
from PythonQt.QtNetwork import QNetworkAccessManager, QNetworkRequest, QNetworkReply

from weakref import WeakValueDictionary


class PluginHost(object):
    defaultConfig = [("general", [("differentApi", "False"), ("uninstallQuestion", "True"), ("loadAllMenus", "True")]), ("plugins", []), ("console", [("backgroundColor", "#000000"), ("textColor", "#FFFFFF"), ("fontFamily", "Monospace"), ("fontSize", "12"), ("tabcomplete", "True"), ("spaces", "True"), ("tabwidth", "2"), ("width", "800"), ("height", "600"), ("startup", ""), ("silentStartup", "False")])]

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
        pytson._setup()

        cls.shell = None
        cls.confdlg = None

        cls.proxies = WeakValueDictionary()

        cls.nwm = None

        cls.modules = {}

        cls.menus = {}
        cls.hotkeys = {}

        cls.cfg = ConfigParser()
        cls.cfg.read(pytson.getConfigPath("pyTSon.conf"))

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
                if cls.plugins[key].apiVersion != pytson.getCurrentApiVersion():
                    if not cls.cfg.getboolean("general", "differentApi", fallback=False):
                        continue

                try:
                    cls.active[key] = cls.plugins[key]()
                    cls.cfg.set("plugins", key, "True")
                except:
                    err = ts3lib.logMessage("Error starting python plugin %s: %s" % (key, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.start", 0)
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

        if cls.nwm:
            cls.nwm.delete()
        cls.nwm = None

        #store config
        with open(pytson.getConfigPath("pyTSon.conf"), "w") as f:
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
                        ts3lib.setPluginMenuEnabled(globid, True)
                if hasattr(cls.active[pname], "menuCreated"):
                    cls.active[pname].menuCreated()

                for keyword, (p, lockey) in cls.hotkeys.items():
                    if type(p) is str and p == pname:
                        cls.hotkeys[keyword] = (cls.active[p], lockey)

                return True
            except:
                err = ts3lib.logMessage("Error starting python plugin %s: %s" % (pname, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.activate", 0)
                if err != ts3defines.ERROR_ok:
                    print("Error starting python plugin %s: %s" % (pname, traceback.format_exc()))

        return False

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
                    if type(cls.menus[key][0]) is not str:
                        if cls.menus[key][0].name == pname:
                            cls.menus[key] = (pname, cls.menus[key][1])
                            ts3lib.setPluginMenuEnabled(key, False)

                cls.active[pname].stop()
                del cls.active[pname]
                cls.cfg.set("plugins", pname, "False")
            except:
                err = ts3lib.logMessage("Error stopping python plugin %s: %s" % (pname, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.deactivate", 0)
                if err != ts3defines.ERROR_ok:
                    print("Error stopping python plugin %s: %s" % (pname, traceback.format_exc()))

    @classmethod
    def reload(cls):
        #stop all running modules
        for key, p in cls.active.items():
            try:
                p.stop()
            except:
                err = ts3lib.logMessage("Error stopping python plugin %s: %s" % (key, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.reload", 0)
                if err != ts3defines.ERROR_ok:
                    print("Error stopping python plugin %s: %s" % (key, traceback.format_exc()))

        cls.active = {}
        cls.plugins = {}

        #import all modules
        spath = pytson.getPluginPath("scripts")
        for d in glob.glob(os.path.join(spath, "*/")):
            if os.path.isdir(d):
                base = os.path.relpath(d, spath)
                try:
                    if base in cls.modules:
                        cls.modules[base] = importlib.reload(cls.modules[base])
                    else:
                        cls.modules[base] = importlib.__import__(base)
                except:
                    err = ts3lib.logMessage("Error loading python plugin from %s: %s" % (d, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.init", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Error loading python plugin from %s: %s" % (d, traceback.format_exc()))

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
                    print("Error calling method %s of plugin %s: %s" % (name, key, traceback.format_exc()))

        #call callback proxies; they can't affect the return value
        zombies = []
        for p in cls.proxies.values():
            if not p:
                zombies.append(id(p))
            else:
                meth = getattr(p, name, None)

                if meth:
                    try:
                        meth(*args)
                    except:
                        print("Error calling method %s in callbackproxy: %s" % (name, traceback.format_exc()))

        for z in zombies:
            del cls.proxies[z]

        for r in ret:
            if r:
                return True

        return False

    @classmethod
    def registerCallbackProxy(cls, obj):
        if not id(obj) in cls.proxies:
            cls.proxies[id(obj)] = obj

    @classmethod
    def unregisterCallbackProxy(cls, obj):
        if obj in cls.proxies:
            cls.proxies.remove(id(obj))

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
                    err = ts3lib.logMessage("Error calling processCommand of python plugin %s: %s" % (p.name, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.processCommand", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Error calling processCommand of python plugin %s: %s" % (p.name, traceback.format_exc()))

        return False

    @classmethod
    def infoData(cls, schid, aid, atype):
        ret = []
        for key, p in cls.active.items():
            if p.infoTitle is not None and hasattr(p, "infoData"):
                try:
                    if p.infoTitle != "":
                        ret.append(p.infoTitle)
                    ret += p.infoData(schid, aid, atype)
                except:
                    err = ts3lib.logMessage("Error calling infoData of python plugin %s: %s" % (key, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.infoData", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Error calling infoData of python plugin %s: %s" % (key, traceback.format_exc()))

        return ret

    @classmethod
    def parseUpdateReply(cls, repstr):
        def platform_str():
            try:
                import sys, platform
            except:
                #sys can't really fail to load
                raise Exception("Error importing platform module")

            if sys.platform == "linux":
                return "linux_%s" % "amd64" if platform.architecture()[0] == "64bit" else "x86"
            elif sys.platform == "win32":
                return "win%s" % platform.architecture()[0][:2]
            else:
                return "mac"

        try:
            obj = json.loads(repstr)

            if obj["tag_name"] == "v%s" % pytson.getVersion():
                QMessageBox.information(None, "pyTSon Update Check", "You are running the latest pyTSon release")
            else:
                for a in obj["assets"]:
                    if a["name"] == "pyTSon_%s.ts3_plugin" % platform_str():
                        QMessageBox.information(None, "pyTSon Update Check", "There is an update of pyTSon for your platform. Get it from <a href='%s'>here</a>" % obj["html_url"])
                        return

                QMessageBox.information(None, "pyTSon Update Check", "You are running the latest pyTSon release (at least for your platform)")
        except:
            err = ts3lib.logMessage("Error parsing reply from update check: %s" % traceback.format_exc(), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.parseUpdateReply", 0)
            if err != ts3defines.ERROR_ok:
                print("Error parsing reply from update check: %s" % traceback.format_exc())

    @classmethod
    def updateCheckFinished(cls, reply):
        if reply.error() == QNetworkReply.NoError:
            cls.parseUpdateReply(reply.readAll().data().decode('utf-8'))
        else:
            err = ts3lib.logMessage("Error checking for update: %s" % reply.error(), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.updateCheckFinished", 0)
            if err != ts3defines.ERROR_ok:
                print("Error checking for update: %s" % reply.error())

        reply.deleteLater()
        cls.nwm.delete()
        cls.nwm = None

    @classmethod
    def updateCheck(cls):
        if cls.nwm:
            #there is a pending updatecheck
            return

        cls.nwm = QNetworkAccessManager()
        cls.nwm.connect("finished(QNetworkReply*)", cls.updateCheckFinished)
        cls.nwm.get(QNetworkRequest(QUrl("https://api.github.com/repos/pathmann/pyTSon/releases/latest")))

    @classmethod
    def initMenus(cls):
        cls.menus = {}
        ret = [(ts3defines.PluginMenuType.PLUGIN_MENU_TYPE_GLOBAL, 0, "Console", os.path.join("ressources", "octicons", "terminal.svg.png")), (ts3defines.PluginMenuType.PLUGIN_MENU_TYPE_GLOBAL, 1, "Settings", os.path.join("ressources", "octicons", "settings.svg.png")), (ts3defines.PluginMenuType.PLUGIN_MENU_TYPE_GLOBAL, 2, "Check for update", os.path.join("ressources", "octicons", "cloud-download.svg.png"))]
        nextid = len(ret)

        loadall = cls.cfg.getboolean("general", "loadAllMenus")
        menustates = []

        for key, p in sorted(cls.plugins.items()):
            for (atype, locid, text, icon) in p.menuItems:
                if p.name in cls.active:
                    cls.menus[nextid] = (cls.active[p.name], locid)
                    ret.append((atype, nextid, text, icon))
                    menustates.append((nextid, True))
                elif loadall:
                    cls.menus[nextid] = (p.name, locid)
                    ret.append((atype, nextid, text, icon))
                    #we have to remember the id, to disable it afterwards
                    menustates.append((nextid, False))

                nextid += 1

        def deactivateMenus():
            for key, val in menustates:
                ts3lib.setPluginMenuEnabled(key, val)

            for key, p in cls.active.items():
                if hasattr(p, "menuCreated"):
                    p.menuCreated()

        QTimer.singleShot(1000, deactivateMenus)

        return ret

    @classmethod
    def globalMenuID(cls, plugin, localid):
        for key, (p, locid) in cls.menus.items():
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
        elif menuItemID == 2:
            cls.updateCheck()
            return

        if menuItemID in cls.menus:
            (plugin, locid) = cls.menus[menuItemID]
            if type(plugin) is not str:
                #if plugin was reloaded, but menuItem does not exist anymore
                try:
                    plugin.onMenuItemEvent(schid, atype, locid, selectedItemID)
                except:
                    err = ts3lib.logMessage("Error calling onMenuItemEvent of python plugin %s: %s" % (plugin.name, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.onMenuItemEvent", 0)
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
                    err = ts3lib.logMessage("Error calling onHotkeyEvent of python plugin %s: %s" % (plugin.name, traceback.format_exc()), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.PluginHost.onHotkeyEvent", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Error calling onHotkeyEvent of python plugin %s: %s" % (plugin.name, traceback.format_exc()))

