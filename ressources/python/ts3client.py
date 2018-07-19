import os

from PythonQt.QtCore import Qt
from PythonQt.QtSql import QSqlDatabase
from PythonQt.QtGui import QPixmap, QPainter, QPen

import ts3lib
import ts3defines

from configparser import ConfigParser
from zipfile import ZipFile

import base64


class Config(object):
    """
    Offers an interface to query the TeamSpeak 3 client's config
    database (settings.db).
    You should always del a reference to this object if not needed anymore to
    assure the database connection is closed.
    """

    class _config(object):
        def __init__(self):
            self.db = QSqlDatabase.addDatabase("QSQLITE", "__pyTSon_config__")
            self.db.setDatabaseName(os.path.join(ts3lib.getConfigPath(),
                                                 "settings.db"))

            if not self.db.isValid():
                raise Exception("Database not valid")

            if not self.db.open():
                raise Exception("Database could not be opened")

        def __del__(self):
            self.db.close()
            self.db.delete()
            QSqlDatabase.removeDatabase("__pyTSon_config__")

        def query(self, sql):
            """
            Executes a query on the database.
            @param sql: the query to be executed
            @type sql: str
            @return: the QSqlQuery object
            @rtype: QSqlQuery
            """
            return self.db.exec_(sql)

        def lastError(self):
            """
            Returns the last error occured in the QSqlDatabase as string.
            @return: the error
            @rtype: str
            """
            return self.db.lastError().text()

    objcount = 0
    instance = None

    def __init__(self):
        Config.objcount += 1
        if not Config.instance:
            Config.instance = Config._config()

    def __del__(self):
        Config.objcount -= 1

        if Config.objcount == 0:
            del Config.instance
            Config.instance = None

    def __getattr__(self, name):
        return getattr(Config.instance, name)


class IconPack(object):
    """
    Offers an interface to the TeamSpeak 3 Client's iconpack. IconPack is also
    a context manager.
    """

    _prefixes = ["200x200_", "100x100_", "32x32_", "24x24_", "20x20_",
                 "16x16_", "8x7_", "7x5_", ""]

    _varNames = ['3D_SOUND', '3D_SOUND_ME', 'ABOUT', 'ACTIVATE_MICROPHONE',
                 'ADD', 'ADD_FOE', 'ADD_FRIEND', 'ADDON', 'ADDON_COLLECTION',
                 'ADDON_BROWSER', 'ADDON_ICONPACK', 'ADDON_SOUNDPACK',
                 'ADDON_THEME', 'ADDON_TRANSLATION', 'ARROW_LEFT',
                 'ARROW_RIGHT', 'AWAY', 'BAN_CLIENT', 'BAN_LIST',
                 'BOOKMARK_ADD', 'BOOKMARK_ADD_FOLDER', 'BOOKMARK_DUPLICATE',
                 'BOOKMARK_MANAGER', 'BOOKMARK_REMOVE', 'BROKEN_IMAGE',
                 'CAPTURE', 'CHANGE_NICKNAME', 'CHANGELOG', 'CHANNEL_CHAT',
                 'CHANNEL_COLLAPSE_ALL', 'CHANNEL_COMMANDER', 'CHANNEL_CREATE',
                 'CHANNEL_CREATE_SUB', 'CHANNEL_DELETE', 'CHANNEL_EDIT',
                 'CHANNEL_EXPAND_ALL', 'CHANNEL_GREEN',
                 'CHANNEL_GREEN_SUBSCRIBED', 'CHANNEL_PRIVATE', 'CHANNEL_RED',
                 'CHANNEL_RED_SUBSCRIBED', 'CHANNEL_SWITCH', 'CHANNEL_YELLOW',
                 'CHANNEL_YELLOW_SUBSCRIBED', 'CHECK_UPDATE', 'CONNECT',
                 'COPY', 'DEFAULT', 'DELETE', 'DELETE_AVATAR', 'DISCONNECT',
                 'DOWN', 'DOWN_ARROW', 'EDIT', 'EDIT_FRIEND_FOE_STATUS',
                 'EMOTICON', 'ERROR', 'FILE_HOME', 'FILE_REFRESH', 'FILE_UP',
                 'FILETRANSFER', 'FIND', 'GROUP_100', 'GROUP_200', 'GROUP_300',
                 'GROUP_500', 'GROUP_600', 'GUISETUP', 'HARDWARE_INPUT_MUTED',
                 'HARDWARE_OUTPUT_MUTED', 'HOSTER_BUTTON', 'HOTKEYS',
                 'ICONSVIEW', 'IDENTITY_ADD', 'IDENTITY_DEFAULT',
                 'IDENTITY_EXPORT', 'IDENTITY_IMPORT', 'IDENTITY_MANAGER',
                 'IDENTITY_REMOVE', 'INFO', 'INPUT_MUTED', 'INPUT_MUTED_LOCAL',
                 'IS_TALKER', 'KICK_FROM_CHANNEL', 'KICK_FROM_SERVER',
                 'LISTVIEW', 'LOADING_IMAGE', 'MESSAGE_INCOMING',
                 'MESSAGE_INFO', 'MESSAGE_OUTGOING', 'MODERATED', 'MUSIC',
                 'MYTS_ACCOUNT_ONLINE', 'MYTS_ACCOUNT_OFFLINE',
                 'MYTS_ACCOUNT_ERROR', 'MYTS_ACCOUNT', 'NEW_CHAT',
                 'NOTIFICATIONS', 'OFFLINE_MESSAGES', 'ON_WHISPERLIST',
                 'OUTPUT_MUTED', 'PERMISSION_OVERVIEW',
                 'PERMISSIONS_CHANNEL_CLIENT', 'PERMISSIONS_CHANNEL_GROUPS',
                 'PERMISSIONS_CHANNELS', 'PERMISSIONS_CLIENTS',
                 'PERMISSIONS_SERVER_GROUPS', 'PHONETICSNICKNAME', 'PING_1',
                 'PING_2', 'PING_3', 'PING_4', 'PING_CALCULATING',
                 'PING_DISCONNECTED', 'PLAY', 'PLAYBACK', 'PLAYER_CHAT',
                 'PLAYER_COMMANDER_OFF', 'PLAYER_COMMANDER_ON', 'PLAYER_OFF',
                 'PLAYER_ON', 'PLAYER_WHISPER', 'PLUGINS', 'POKE', 'PRESENT',
                 'QUIT', 'RECORDING_START', 'RECORDING_STOP', 'REGISTER',
                 'REMOVE_FOE', 'REMOVE_FRIEND', 'RELOAD', 'SECURITY',
                 'SELECTFOLDER', 'SEND_COMPLAINT', 'SERVER_GREEN',
                 'SERVER_LOG', 'SERVER_QUERY', 'SETTINGS', 'SETUP_WIZARD',
                 'STOP', 'SUBSCRIBE_TO_ALL_CHANNELS', 'SUBSCRIBE_TO_CHANNEL',
                 'SYNC_ENABLE', 'SYNC_DISABLE', 'SYNC', 'TAB_CLOSE_BUTTON',
                 'TEMP_SERVER_PASSWORD', 'TEMP_SERVER_PASSWORD_ADD',
                 'TEXTFORMAT', 'TEXTFORMAT_BOLD', 'TEXTFORMAT_FOREGROUND',
                 'TEXTFORMAT_ITALIC', 'TEXTFORMAT_UNDERLINE',
                 'TOGGLE_BLOCK_INCOMING_WHISPERS',
                 'TOGGLE_SERVER_QUERY_CLIENTS', 'TOKEN',
                 'UNSUBSCRIBE_FROM_ALL_CHANNELS', 'UNSUBSCRIBE_FROM_CHANNEL',
                 'UP', 'UP_ARROW', 'UPLOAD_AVATAR', 'URLCATCHER',
                 'VIRTUALSERVER_EDIT', 'VOLUME', 'WARNING', 'WEBLIST',
                 'WHISPERLISTS', 'WARNING_EXTERNAL_LINK']

    @staticmethod
    def current():
        """
        Returns the current iconpack used (an Exception is raised if something
        failed).
        @return: the iconpack
        @rtype: IconPack
        """
        cfg = Config()
        q = cfg.query("SELECT value FROM application WHERE key='IconPack'")
        if not q.next():
            err = cfg.lastError()
            del cfg
            raise Exception("Query failed: %s" % err)

        del cfg
        return IconPack(info=q.value(0))

    def __init__(self, *, info=None, name=None):
        """
        Instantiates a new IconPack object referenced by its name or the
        internal info string (an Exception is raised if the iconpack could
        not be located).
        @param info: the info string used in the settings.db
        @type info: str
        @param name: the name of the iconpack
        @type name: str
        """
        if name:
            cfg = Config()
            q = cfg.query("SELECT value FROM addons WHERE value LIKE "
                          "'%%name=%s%%'" % name)

            if not q.next():
                del cfg
                raise Exception("No iconpack with name %s found in database" %
                                name)

            for l in q.value(0).split(' '):
                if l.startswith("info="):
                    self.info = l.split('=')[1]
                    break

            del cfg

            if self.info == "default.zip":
                self.info = "default"
            elif not self.info:
                raise Exception("Unknown settings format in database")
        elif info:
            self.info = info
        else:
            raise Exception("No info or name given")

        self.path = None
        self.zip = False
        self.emos = {}

        if info[-4:].lower() == ".zip":
            self.zip = True

            if os.path.isfile(os.path.join(ts3lib.getResourcesPath(),
                                           "gfx", info)):
                self.path = os.path.join(ts3lib.getResourcesPath(),
                                         "gfx", info)
            elif os.path.isfile(os.path.join(ts3lib.getConfigPath(), "gfx",
                                             info)):
                self.path = os.path.join(ts3lib.getConfigPath(), "gfx", info)
        else:
            if os.path.isdir(os.path.join(ts3lib.getResourcesPath(), "gfx",
                                          info)):
                self.path = os.path.join(ts3lib.getResourcesPath(), "gfx",
                                         info)
            elif os.path.isfile(os.path.join(ts3lib.getResourcesPath(), "gfx",
                                             info + ".zip")):
                self.zip = True
                self.path = os.path.join(ts3lib.getResourcesPath(), "gfx",
                                         info + ".zip")
            elif os.path.isdir(os.path.join(ts3lib.getConfigPath(), "gfx",
                                            info)):
                self.path = os.path.join(ts3lib.getConfigPath(), "gfx", info)
            elif os.path.isfile(os.path.join(ts3lib.getConfigPath(), "gfx",
                                             info + ".zip")):
                self.zip = True
                self.path = os.path.join(ts3lib.getConfigPath(), "gfx",
                                         info + ".zip")

        if not self.path:
            raise Exception("Iconpack %s not found" % info)

    def _loadEmoticonSettings(self, f):
        self.emos.clear()
        for line in f:
            if self.zip:
                line = line.decode('utf-8')
            e = line.split('=')
            if len(e) == 2:
                txt = e[1].strip()
                if txt.startswith('"') and txt.endswith('"') and len(txt) >= 3:
                    self.emos[txt[1:-1]] = e[0].strip()

    def open(self):
        """
        Reads the settings for the iconpack and if it's zip-based, opens the
        file for reading. Must be called once before any icon can be accessed.
        """
        if self.zip:
            self.cont = ZipFile(self.path)
            if self.info != "default":
                if "settings.ini" not in self.cont.namelist():
                    raise Exception("No settings.ini in iconpack found")

                self.settings = ConfigParser(strict=False)
                with self.cont.open("settings.ini") as f:
                    self.settings.read_string(f.read().decode('utf-8'))

            emconf = os.path.join("emoticons", "emoticons.txt")
            if emconf in self.cont.namelist():
                with self.cont.open(emconf) as f:
                    self._loadEmoticonSettings(f)
        else:
            if ("settings.ini" not in os.listdir(self.path) or
               not os.path.isfile(os.path.join(self.path, "settings.ini"))):
                raise Exception("No settings.ini in iconpack found")

            self.settings = ConfigParser(strict=False)
            self.settings.read(os.path.join(self.path, "settings.ini"))

            emconf = os.path.join(self.path, "emoticons", "emoticons.txt")
            if os.path.isfile(emconf):
                with open(emconf, "r", encoding="utf-8") as f:
                    self._loadEmoticonSettings(f)

    def close(self):
        """
        If the iconpack is zip-based, the file is closed. After this is called,
        no icons can be accessed (till open is called again).
        """
        if self.zip:
            self.cont.close()

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    @staticmethod
    def defaultName(var):
        """
        Returns the variable name used in the default iconpack.
        @param var: the variable used in an iconpack
        @type var:str
        @return: the variable name
        @rtype: str
        """
        if var == "3D_SOUND_OTHER":
            var = "3D_SOUND_USER"
        elif var == "CHANNEL_COLLAPSE_SUB":
            var = "CHANNEL_COLLAPSE_ALL"
        elif var == "CHANNEL_EXPAND_SUB":
            var = "CHANNEL_EXPAND_ALL"
        elif var == "COMPLAINT_LIST":
            var = "SEND_COMPLAINT"
        elif var == "CLIENT_HIDE":
            var = "UNSUBSCRIBE_FROM_CHANNEL"
        elif var == "CLIENT_SHOW":
            var = "SUBSCRIBE_TO_CHANNEL"
        elif var == "CONNECT_NEW_TAB":
            var = "CONNECT"
        elif var == "CONTACT":
            var = "EDIT_FRIEND_FOE_STATUS"
        elif var == "COPY_URL":
            var = "COPY"
        elif var == "DELETE_AVATAR_OTHER":
            var = "DELETE_AVATAR"
        elif var == "DISCONNECT_ALL":
            var = "DISCONNECT"
        elif var == "DOWNLOAD":
            var = "DOWN"
        elif var == "FILTER_CLEAR":
            var = "ERROR"
        elif var == "ICONVIEWER":
            var = "GROUP_300"
        elif var == "INVITE_BUDDY":
            var = "NOTIFICATIONS"
        elif var == "MAKE_CURRENT_CHANNEL_DEFAULT":
            var = "CHECK_UPDATE"
        elif var == "MESSAGES":
            var = "AWAY"
        elif var == "MOVE_CLIENT_TO_OWN_CHANNEL":
            var = "CHECK_UPDATE"
        elif var == "SEARCH_CLEAR":
            var = "ERROR"
        elif var == "SEARCH_HIDE":
            var = "ERROR"
        elif var == "SUBSCRIBE_MODE":
            var = "SUBSCRIBE_TO_ALL_CHANNELS"
        elif var == "SUBSCRIBE_TO_CHANNEL_FAMILY":
            var = "SUBSCRIBE_TO_ALL_CHANNELS"
        elif var == "SWITCH_ADVANCED" or var == "SWITCH_STANDARD":
            var = "SETUP_WIZARD"
        elif var == "TALK_POWER_REQUEST":
            var = "REQUEST_TALK_POWER"
        elif var == "TALK_POWER_CANCEL_REQUEST":
            var = "REQUEST_TALK_POWER_CANCEL"
        elif var == "TALK_POWER_GRANT":
            var = "IS_TALKER"
        elif var == "TALK_POWER_REVOKE":
            var = "REVOKE_TALKER"
        elif var == "UNSUBSCRIBE_MODE":
            var = "UNSUBSCRIBE_FROM_ALL_CHANNELS"
        elif var == "UNSUBSCRIBE_TO_CHANNEL_FAMILY":
            var = "UNSUBSCRIBE_FROM_ALL_CHANNELS"
        elif var == "UPLOAD":
            var = "UP"
        elif var == "TOKEN_USE":
            var = "TOKEN"
        elif var == "WHISPER":
            var = "WHISPERLISTS"
        elif var == "WHISPERHISTORY":
            var = "WHISPERLISTS"
        elif var in IconPack._varNames:
            return var
        else:
            return ""

        return var

        """
not matched (not used anymore?):
APPLY
BOOKMARK_SET_FOR_ALL
CHANNEL_CREATE_SPACER
CLOSE_BUTTON
FOLDER
FOLDER_CREATE
REFRESH
RENAME
SEARCH
SORT_BY_NAME
SOUNDPACK
TALK_POWER_GRANT_NEXT
TALK_POWER_REVOKE_ALL_GRANT_NEXT
        """

    def fallback(self, var):
        """
        Returns the fallback icon for a variable according to the
        iconpack's settings.
        @param var: the variable name
        @type var: str
        @return: the resulting pixmap
        @rtype: QPixmap
        """
        if self.info == "default":
            return QPixmap()

        if (self.settings.has_option("options", "FALLBACK") and
           self.settings.getboolean("options", "FALLBACK")):
            try:
                with IconPack(info="default") as defpack:
                    return defpack.icon(var)
            except:
                return QPixmap()
        elif (self.settings.has_option("options", "PLACEHOLDER") and
              self.settings.getint("options", "PLACEHOLDER") == 2):
            ret = QPixmap(32, 32)
            painter = QPainter(ret)
            painter.setPen(QPen(Qt.blue, 2))
            painter.drawRect(0, 0, 128, 128)
            painter.delete()

            return ret
        else:
            return QPixmap()

    def icons(self):
        """
        Returns the list of variables used in the iconpack (excluding
        fallback mechanisms).
        @return: a list of variable names
        @rtype: list[str]
        """
        return self.settings.options("gfxfiles")

    def _findDefaultFilename(self, var):
        if self.zip:
            for p in self._prefixes:
                n = "%s%s.png" % (p, var.lower())
                if n in self.cont.namelist():
                    return n
        else:
            for p in self._prefixes:
                n = "%s%s.png" % (p, var.lower())
                if os.path.isfile(os.path.join(self.path, n)):
                    return n
        return ""

    def icon(self, var):
        """
        Returns the icon representing a variable used in the iconpack.
        If the icon cannot be found, the iconpack's fallback mechanisms are
        used. If everything fails, an empty pixmap is returned.
        @param var: the variable name
        @type var: str
        @return: the resulting pixmap
        @rtype: QPixmap
        """
        if self.info == "default":
            path = self._findDefaultFilename(self.defaultName(var))
            if path == "":
                return QPixmap()
        else:
            if not self.settings.has_option("gfxfiles", var):
                return self.fallback(var)
            path = self.settings.get("gfxfiles", var)
            if path == "":
                return self.fallback(var)

        if self.zip:
            if path in self.cont.namelist():
                with self.cont.open(path) as f:
                    ret = QPixmap()
                    if ret.loadFromData(f.read()):
                        return ret
                    else:
                        return QPixmap()
            else:
                return self.fallback(var)
        else:
            if os.path.isfile(os.path.join(self.path, path)):
                return QPixmap(os.path.join(self.path, path))
            else:
                return self.fallback(var)

    def emoticons(self):
        """
        Returns the list of emoticon replacements used in the iconpack.
        @return: a list of emoticon strings
        @rtype: list[str]
        """
        return self.emos.keys()

    def emoticon(self, text):
        """
        Returns the icon replacing the emoticon string.
        @param text: the emoticon as string
        @type text: str
        @return: the resulting pixmap
        @rtype: QPixmap
        """
        if text in self.emos:
            empath = os.path.join("emoticons", self.emos[text])
            if self.zip:
                if empath in self.cont.namelist():
                    with self.cont.open(empath) as f:
                        ret = QPixmap()
                        if ret.loadFromData(f.read()):
                            return ret
            else:
                if os.path.isfile(os.path.join(self.path, empath)):
                    return QPixmap(os.path.join(self.path, empath))

        return QPixmap()


class ServerCache:
    """
    Offers an interface to the cached data of a TeamSpeak 3 server.
    """

    def __init__(self, schid):
        """
        Instantiates a new ServerCache object referenced by the server
        connection handler id (an Exception is raised if the path in the
        filesystem could not be located).
        @param schid: the ID of the serverconnection
        @type schid: int
        """
        err, uid = (ts3lib.getServerVariableAsString(schid,
                    ts3defines.VirtualServerProperties.
                    VIRTUALSERVER_UNIQUE_IDENTIFIER))

        if err != ts3defines.ERROR_ok:
            raise Exception("Error getting Server unique identifier: %s" % err)

        self.path = os.path.join(ts3lib.getConfigPath(), "cache",
                                 base64.b64encode(
                                 uid.encode("utf-8")).decode("utf-8"))
        if not os.path.isdir(self.path):
            raise Exception("No such file or directory: %s" % self.path)

    def icon(self, iconid):
        """
        Returns an icon cached on disk.
        @param iconid: ID of the icon
        @type iconid: int
        @return: the icon
        @rtype: QPixmap
        """
        return QPixmap(os.path.join(self.path, "icons", "icon_%s" % iconid))


class CountryFlags:
    """
    Offers an interface to get the client's country flags. CountryFlags is
    also a context manager.
    """

    def __init__(self):
        """
        Instantiates a new object. This will raise an exception, if the
        Zipfile could not be located.
        """
        self.path = os.path.join(ts3lib.getResourcesPath(), "gfx",
                                 "countries.zip")
        if not os.path.isfile(self.path):
            raise Exception("Could not locate countries.zip")

    def open(self):
        """
        Opens the Zipfile for reading. This must be called before any flag is
        requested with flag.
        """
        self.zip = ZipFile(self.path)

    def close(self):
        """
        Closes the Zipfile.
        """
        self.zip.close()

    def flag(self, code):
        """
        Returns a QPixmap containing the flag of the given country code if
        exist.
        @param code: the country code
        @type code: str
        @returns: the flag
        @rtype: QPixmap
        """
        fname = "%s.png" % code.lower()
        if fname in self.zip.namelist():
            with self.zip.open(fname) as f:
                ret = QPixmap()
                if ret.loadFromData(f.read()):
                    return ret

        return QPixmap()

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, type, value, traceback):
        self.close()
