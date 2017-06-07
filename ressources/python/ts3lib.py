import _ts3lib
import ts3defines
from ts3defines import (ClientProperties, ClientPropertiesRare,
                        ChannelProperties, ChannelPropertiesRare,
                        VirtualServerProperties, VirtualServerPropertiesRare,
                        PluginTargetMode)

import math
import string
from Crypto.PublicKey import RSA
from Crypto.Signature import PKCS1_v1_5
from Crypto.Hash import SHA256
from base64 import b64encode, b64decode
from Crypto.Random import random


def getChannelVariable(schid, channelID, flag):
    if flag in [ChannelProperties.CHANNEL_CODEC,
                ChannelProperties.CHANNEL_CODEC_QUALITY,
                ChannelProperties.CHANNEL_FLAG_PERMANENT,
                ChannelProperties.CHANNEL_FLAG_SEMI_PERMANENT,
                ChannelProperties.CHANNEL_FLAG_DEFAULT,
                ChannelProperties.CHANNEL_FLAG_PASSWORD,
                ChannelProperties.CHANNEL_CODEC_LATENCY_FACTOR,
                ChannelProperties.CHANNEL_CODEC_IS_UNENCRYPTED,
                ChannelPropertiesRare.CHANNEL_FLAG_MAXCLIENTS_UNLIMITED,
                ChannelPropertiesRare.CHANNEL_FLAG_MAXFAMILYCLIENTS_UNLIMITED,
                ChannelPropertiesRare.CHANNEL_FLAG_MAXFAMILYCLIENTS_INHERITED,
                ChannelPropertiesRare.CHANNEL_FLAG_ARE_SUBSCRIBED,
                ChannelPropertiesRare.CHANNEL_FORCED_SILENCE,
                ChannelPropertiesRare.CHANNEL_FLAG_PRIVATE,
                ]:
        return _ts3lib.getChannelVariableAsInt(schid, channelID, flag)
    elif flag in [ChannelProperties.CHANNEL_ORDER,
                  ChannelProperties.CHANNEL_DELETE_DELAY,
                  ChannelPropertiesRare.CHANNEL_NEEDED_TALK_POWER,
                  ChannelPropertiesRare.CHANNEL_ICON_ID,
                  ]:
        return _ts3lib.getChannelVariableAsUInt64(schid, channelID, flag)
    elif flag in [ChannelProperties.CHANNEL_MAXCLIENTS,
                  ChannelProperties.CHANNEL_MAXFAMILYCLIENTS,
                  ]:
        err, ret = _ts3lib.getChannelVariableAsString(schid, channelID, flag)
        if err == ts3defines.ERROR_ok:
            return (err, int(ret))
        else:
            return (err, None)
    else:
        return _ts3lib.getChannelVariableAsString(schid, channelID, flag)


def getClientVariable(schid, clientID, flag):
    if flag in [ClientProperties.CLIENT_FLAG_TALKING,
                ClientProperties.CLIENT_INPUT_MUTED,
                ClientProperties.CLIENT_OUTPUT_MUTED,
                ClientProperties.CLIENT_OUTPUTONLY_MUTED,
                ClientProperties.CLIENT_INPUT_HARDWARE,
                ClientProperties.CLIENT_OUTPUT_HARDWARE,
                ClientProperties.CLIENT_INPUT_DEACTIVATED,
                ClientProperties.CLIENT_IS_MUTED,
                ClientProperties.CLIENT_IS_RECORDING,
                ClientPropertiesRare.CLIENT_AWAY,
                ClientPropertiesRare.CLIENT_TYPE,
                ClientPropertiesRare.CLIENT_TALK_REQUEST,
                ClientPropertiesRare.CLIENT_IS_TALKER,
                ClientPropertiesRare.CLIENT_IS_PRIORITY_SPEAKER,
                ClientPropertiesRare.CLIENT_IS_CHANNEL_COMMANDER,
                ]:
        return _ts3lib.getlientVariableAsInt(schid, clientID, flag)
    elif flag in [ClientProperties.CLIENT_IDLE_TIME,
                  ClientPropertiesRare.CLIENT_LAST_VAR_REQUEST,
                  ClientPropertiesRare.CLIENT_DATABASE_ID,
                  ClientPropertiesRare.CLIENT_CHANNEL_GROUP_ID,
                  ClientPropertiesRare.CLIENT_CREATED,
                  ClientPropertiesRare.CLIENT_LASTCONNECTED,
                  ClientPropertiesRare.CLIENT_TOTALCONNECTIONS,
                  ClientPropertiesRare.CLIENT_TALK_POWER,
                  ClientPropertiesRare.CLIENT_MONTH_BYTES_UPLOADED,
                  ClientPropertiesRare.CLIENT_MONTH_BYTES_DOWNLOADED,
                  ClientPropertiesRare.CLIENT_TOTAL_BYTES_UPLOADED,
                  ClientPropertiesRare.CLIENT_TOTAL_BYTES_DOWNLOADED,
                  ClientPropertiesRare.CLIENT_UNREAD_MESSAGES,
                  ClientPropertiesRare.CLIENT_NEEDED_SERVERQUERY_VIEW_POWER,
                  ClientPropertiesRare.CLIENT_ICON_ID,
                  ClientPropertiesRare.CLIENT_CHANNEL_GROUP_INHERITED_CHANNEL_ID,
                  ]:
        return _ts3lib.getClientVariableAsUint64(schid, clientID, flag)
    elif flag in [ClientProperties.CLIENT_VOLUME_MODIFICATOR]:
        err, ret = _ts3lib.getClientVariableAsString(schid, clientID, flag)
        if err == ts3defines.ERROR_ok:
            return (err, float(ret))
        else:
            return (err, None)
    elif flag in [ClientPropertiesRare.CLIENT_SERVERGROUPS]:
        err, ret = _ts3lib.getClientVariableAsString(schid, clientID, flag)
        if err == ts3defines.ERROR_ok:
            return (err, map(int, ret.split(',')))
        else:
            return (err, None)
    else:
        return _ts3lib.getClientVariableAsString(schid, clientID, flag)


def getClientSelfVariable(schid, flag):
    if flag in [ClientProperties.CLIENT_FLAG_TALKING,
                ClientProperties.CLIENT_INPUT_MUTED,
                ClientProperties.CLIENT_OUTPUT_MUTED,
                ClientProperties.CLIENT_OUTPUTONLY_MUTED,
                ClientProperties.CLIENT_INPUT_HARDWARE,
                ClientProperties.CLIENT_OUTPUT_HARDWARE,
                ClientProperties.CLIENT_INPUT_DEACTIVATED,
                ClientProperties.CLIENT_IS_MUTED,
                ClientProperties.CLIENT_IS_RECORDING,
                ClientPropertiesRare.CLIENT_AWAY,
                ClientPropertiesRare.CLIENT_TYPE,
                ClientPropertiesRare.CLIENT_TALK_REQUEST,
                ClientPropertiesRare.CLIENT_IS_TALKER,
                ClientPropertiesRare.CLIENT_IS_PRIORITY_SPEAKER,
                ClientPropertiesRare.CLIENT_IS_CHANNEL_COMMANDER,
                ]:
        return _ts3lib.getlientSelfVariableAsInt(schid, flag)
    elif flag in [ClientProperties.CLIENT_VOLUME_MODIFICATOR]:
        err, ret = _ts3lib.getClientSelfVariableAsString(schid, flag)
        if err == ts3defines.ERROR_ok:
            return (err, float(ret))
        else:
            return (err, None)
    elif flag in [ClientPropertiesRare.CLIENT_SERVERGROUPS]:
        err, ret = _ts3lib.getClientSelfVariableAsString(schid, flag)
        if err == ts3defines.ERROR_ok:
            return (err, map(int, ret.split(',')))
        else:
            return (err, None)
    else:
        return _ts3lib.getClientSelfVariableAsString(schid, flag)


def getServerVariable(schid, flag):
    if flag in [VirtualServerProperties.VIRTUALSERVER_CODEC_ENCRYPTION_MODE,
                VirtualServerPropertiesRare.VIRTUALSERVER_HOSTMESSAGE_MODE,
                VirtualServerPropertiesRare.VIRTUALSERVER_FLAG_PASSWORD,
                VirtualServerPropertiesRare.VIRTUALSERVER_AUTOSTART,
                VirtualServerPropertiesRare.VIRTUALSERVER_LOG_CLIENT,
                VirtualServerPropertiesRare.VIRTUALSERVER_LOG_QUERY,
                VirtualServerPropertiesRare.VIRTUALSERVER_LOG_CHANNEL,
                VirtualServerPropertiesRare.VIRTUALSERVER_LOG_PERMISSIONS,
                VirtualServerPropertiesRare.VIRTUALSERVER_LOG_SERVER,
                VirtualServerPropertiesRare.VIRTUALSERVER_LOG_FILETRANSFER,
                VirtualServerPropertiesRare.VIRTUALSERVER_WEBLIST_ENABLED,
                VirtualServerPropertiesRare.VIRTUALSERVER_ASK_FOR_PRIVILEGEKEY,
                VirtualServerPropertiesRare.VIRTUALSERVER_HOSTBANNER_MODE,
                ]:
        return _ts3lib.getServerVariableAsInt(schid, flag)
    elif flag in [VirtualServerProperties.VIRTUALSERVER_MAXCLIENTS,
                  VirtualServerProperties.VIRTUALSERVER_CLIENTS_ONLINE,
                  VirtualServerProperties.VIRTUALSERVER_CHANNELS_ONLINE,
                  VirtualServerProperties.VIRTUALSERVER_CREATED,
                  VirtualServerProperties.VIRTUALSERVER_UPTIME,
                  VirtualServerPropertiesRare.VIRTUALSERVER_DEFAULT_SERVER_GROUP,
                  VirtualServerPropertiesRare.VIRTUALSERVER_DEFAULT_CHANNEL_GROUP,
                  VirtualServerPropertiesRare.VIRTUALSERVER_DEFAULT_CHANNEL_ADMIN_GROUP,
                  VirtualServerPropertiesRare.VIRTUALSERVER_MAX_DOWNLOAD_TOTAL_BANDWIDTH,
                  VirtualServerPropertiesRare.VIRTUALSERVER_MAX_UPLOAD_TOTAL_BANDWIDTH,
                  VirtualServerPropertiesRare.VIRTUALSERVER_HOSTBANNER_GFX_INTERVAL,
                  VirtualServerPropertiesRare.VIRTUALSERVER_COMPLAIN_AUTOBAN_COUNT,
                  VirtualServerPropertiesRare.VIRTUALSERVER_COMPLAIN_AUTOBAN_TIME,
                  VirtualServerPropertiesRare.VIRTUALSERVER_COMPLAIN_REMOVE_TIME,
                  VirtualServerPropertiesRare.VIRTUALSERVER_MIN_CLIENTS_IN_CHANNEL_BEFORE_FORCED_SILENCE,
                  VirtualServerPropertiesRare.VIRTUALSERVER_ANTIFLOOD_POINTS_TICK_REDUCE,
                  VirtualServerPropertiesRare.VIRTUALSERVER_ANTIFLOOD_POINTS_NEEDED_COMMAND_BLOCK,
                  VirtualServerPropertiesRare.VIRTUALSERVER_ANTIFLOOD_POINTS_NEEDED_IP_BLOCK,
                  VirtualServerPropertiesRare.VIRTUALSERVER_CLIENT_CONNECTIONS,
                  VirtualServerPropertiesRare.VIRTUALSERVER_QUERY_CLIENT_CONNECTIONS,
                  VirtualServerPropertiesRare.VIRTUALSERVER_QUERYCLIENTS_ONLINE,
                  VirtualServerPropertiesRare.VIRTUALSERVER_DOWNLOAD_QUOTA,
                  VirtualServerPropertiesRare.VIRTUALSERVER_UPLOAD_QUOTA,
                  VirtualServerPropertiesRare.VIRTUALSERVER_MONTH_BYTES_DOWNLOADED,
                  VirtualServerPropertiesRare.VIRTUALSERVER_MONTH_BYTES_UPLOADED,
                  VirtualServerPropertiesRare.VIRTUALSERVER_TOTAL_BYTES_DOWNLOADED,
                  VirtualServerPropertiesRare.VIRTUALSERVER_TOTAL_BYTES_UPLOADED,
                  VirtualServerPropertiesRare.VIRTUALSERVER_PORT,
                  VirtualServerPropertiesRare.VIRTUALSERVER_NEEDED_IDENTITY_SECURITY_LEVEL,
                  VirtualServerPropertiesRare.VIRTUALSERVER_MIN_CLIENT_VERSION,
                  VirtualServerPropertiesRare.VIRTUALSERVER_ICON_ID,
                  VirtualServerPropertiesRare.VIRTUALSERVER_RESERVED_SLOTS,
                  VirtualServerPropertiesRare.VIRTUALSERVER_TOTAL_PACKETLOSS_SPEECH,
                  VirtualServerPropertiesRare.VIRTUALSERVER_TOTAL_PACKETLOSS_KEEPALIVE,
                  VirtualServerPropertiesRare.VIRTUALSERVER_TOTAL_PACKETLOSS_CONTROL,
                  VirtualServerPropertiesRare.VIRTUALSERVER_TOTAL_PACKETLOSS_TOTAL,
                  VirtualServerPropertiesRare.VIRTUALSERVER_TOTAL_PING,
                  VirtualServerPropertiesRare.VIRTUALSERVER_CHANNEL_TEMP_DELETE_DELAY_DEFAULT,
                  VirtualServerPropertiesRare.VIRTUALSERVER_MIN_ANDROID_VERSION,
                  VirtualServerPropertiesRare.VIRTUALSERVER_MIN_IOS_VERSION,
                  VirtualServerPropertiesRare.VIRTUALSERVER_MIN_WINPHONE_VERSION,
                  ]:
        return _ts3lib.getServerVariableAsUInt64(schid, flag)
    elif flag in [VirtualServerPropertiesRare.VIRTUALSERVER_PRIORITY_SPEAKER_DIMM_MODIFICATOR]:
        err, ret = _ts3lib.getServerVariableAsString(schid, flag)
        if err == ts3defines.ERROR_ok:
            return (err, float(ret))
        else:
            return (err, None)
    else:
        return _ts3lib.getServerVariableAsString(schid, flag)


"""
To be done (?):
def getConnectionVariable(schid, clientID, flag):
    pass
"""


KEYBYTE_SIZE = 128
# b64encode length = 4 * roundup(n / 3)
SIG_SIZE = math.ceil(KEYBYTE_SIZE / 3) * 4
# The TS3 server allows 8192 bytes to be send, we use SIG_SIZE and some
# bytes for package management as overhead, so this should be always
# less than 8192-SIG_SIZE-overhead
MAX_CMD_SIZE = 4096

PKGKEY_ALPHABET = string.ascii_letters + string.punctuation + string.digits

if SIG_SIZE >= MAX_CMD_SIZE:
    # this is not failsafe, even with SIG_SIZE < MAX_CMD_SIZE, with the
    # overhead a plugincommand might be too long
    raise Exception("Plugincommand signature size is too long")


class _PluginCommandType(object):
    unknown = 0
    normal = 1
    requestKey = 2
    sendKey = 3


class _GenericPluginCommand(object):
    pkgtype = _PluginCommandType.unknown

    @classmethod
    def pack(cls, sender, receiver, data):
        pkgcount = math.ceil(len(data) / MAX_CMD_SIZE)
        key = "".join(random.sample(PKGKEY_ALPHABET, 4))

        pkgs = []
        for i in range(pkgcount):
            pkgs.append(cls(sender, receiver,
                        data[i * MAX_CMD_SIZE:(i + 1) * MAX_CMD_SIZE],
                        part=i + 1, count=pkgcount, pkgkey=key))
        return pkgs

    @classmethod
    def fromString(cls, s):
        tokens = s.split(" ")
        if len(tokens) < 7:
            return None

        try:
            if int(tokens[0]) != cls.pkgtype:
                return None

            cmd = cls(int(tokens[2]), int(tokens[3]),
                      " ".join(tokens[6:]), part=int(tokens[4]),
                      count=int(tokens[5]), pkgkey=tokens[1])
            return cmd
        except:
            return None

    def __init__(self, sender, receiver, data, *, part=1, count=1,
                 pkgkey=None):
        super().__init__()
        if pkgkey:
            self.pkgkey = pkgkey
        else:
            self.pkgkey = "".join(random.sample(PKGKEY_ALPHABET, 4))
        self.sender = sender
        self.receiver = receiver
        self.data = data
        self.part = part
        self.count = count

    def fields(self):
        return [self.pkgtype, self.pkgkey, self.sender,
                self.receiver, self.part, self.count, self.data]

    def toString(self):
        return " ".join(map(str, self.fields()))

    def isComplete(self):
        return self.part == self.count == 1


class _PluginCommand(_GenericPluginCommand):
    pkgtype = _PluginCommandType.normal

    @classmethod
    def fromString(cls, s):
        try:
            generic = super().fromString(s)

            data = generic.data.split(" ")
            if len(data) == 1:
                generic.data = ""
            else:
                generic.data = " ".join(data[:-1])
            generic.signature = data[-1]

            return generic
        except:
            return None

    def __init__(self, sender, receiver, data, *, part=1, count=1,
                 pkgkey=None):
        super().__init__(sender, receiver, data, part=part, count=count,
                         pkgkey=pkgkey)
        self.signature = ""

    def fields(self):
        return super().fields() + [self.signature]

    def sign(self, signer):
        self.signature = ""
        h = SHA256.new("".join(map(str, self.fields()[:-1])).encode("utf-8"))
        self.signature = b64encode(signer.sign(h)).decode("utf-8")

    def verify(self, pubkey):
        try:
            rsapub = RSA.importKey(pubkey)
            verifier = PKCS1_v1_5.new(rsapub)
        except:
            return False

        h = SHA256.new("".join(map(str, self.fields()[:-1])).encode("utf-8"))
        return verifier.verify(h, b64decode(self.signature.encode("utf-8")))


class _RequestKey(_GenericPluginCommand):
    pkgtype = _PluginCommandType.requestKey

    @classmethod
    def pack(cls, sender, receiver):
        return _RequestKey(sender, receiver,
                           "".join(random.sample(PKGKEY_ALPHABET, 4)))


class _SendKey(_GenericPluginCommand):
    pkgtype = _PluginCommandType.sendKey

    @classmethod
    def pack(cls, reqcmd, publickey):
        return _SendKey(reqcmd.receiver, reqcmd.sender,
                        reqcmd.data + " " + publickey.decode("utf-8"))

    @property
    def token(self):
        return self.data.split(" ")[0]

    @property
    def publickey(self):
        return "".join(self.data.split(" ")[1:]).encode("utf-8")


class _PluginCommandMerge(object):
    def __init__(self):
        super().__init__()
        self._content = None

    def add(self, cmd):
        if self._content is None:
            self._content = [None] * cmd.count
        elif cmd.part >= len(self._content):
            return False

        if self._content[cmd.part - 1] is None:
            self._content[cmd.part - 1] = cmd.data
            return True
        else:
            return False

    def isComplete(self):
        if self._content:
            return all(self._content)
        else:
            return False

    @property
    def content(self):
        return "".join(self._content)


class _PluginCommandHandler(object):
    """
    Because the plugin sdk does not emit the sender of a plugin command, we
    have to bypass it with some PublicKey infrastructure to add sender
    integrity to plugincmds.
    To achieve that, each uid has a (temporary) RSA key. The public key can
    be requested per plugincommand. We could store the public key in the
    client's meta data, but I don't like occupying it.
    Furthermore long plugincommands are splitted in parts.
    A simplified procedure between client A and client B:
    A: sendPluginCommand normal sender.id receiver.id data sign(hash(ALL))
    B: if have the public key, handle normally (possibly wait for all parts)
    B: if not, queue the command,
    sendPluginCommand RequestKey sender.id receiver.id AnswerToken
    A: sendPluginCommand SendKey sender.id receiver.id AnswerToken publicKey
    B: process the queue of A
    """

    MAX_MERGES = 1000
    MAX_STORE = 1000
    MAX_REQUESTS = 1000
    MAX_REQUESTS_ID = 20

    privkeys = dict()
    pubkeys = dict()
    requests = dict()
    sends = list()
    merges = dict()
    cmdstore = dict()

    @classmethod
    def sendPluginCommand(cls, schid, command, targetMode, targetIDs,
                          *returnCode):
        err, myid = _ts3lib.getClientID(schid)
        if err != ts3defines.ERROR_ok:
            return err

        # we need to send the command to each client separately, otherwise
        # an attacker could copy the complete cmd ("header", data and
        # signature) and resend the complete package to the other receiving
        # clients
        # because of this it's not possible to use
        # PluginCommandTarget_CURRENT_CHANNEL_SUBSCRIBED_CLIENTS
        if targetMode == PluginTargetMode.PluginCommandTarget_CURRENT_CHANNEL:
            err, mychan = _ts3lib.getChannelOfClient(schid, myid)
            if err != ts3defines.ERROR_ok:
                return err

            err, clids = _ts3lib.getChannelClientList(schid, mychan)
            if err != ts3defines.ERROR_ok:
                return err
        elif targetMode == PluginTargetMode.PluginCommandTarget_SERVER:
            err, clids = _ts3lib.getClientList(schid)
            if err != ts3defines.ERROR_ok:
                return err
        elif targetMode == PluginTargetMode.PluginCommandTarget_CLIENT:
            clids = targetIDs
        elif targetMode == PluginTargetMode.PluginCommandTarget_CURRENT_CHANNEL_SUBSCRIBED_CLIENTS:
            return ts3defines.ERROR_parameter_invalid

        if schid not in cls.privkeys:
            cls.privkeys[schid] = RSA.generate(KEYBYTE_SIZE * 8)

        signer = PKCS1_v1_5.new(cls.privkeys[schid])

        for client in clids:
            pkgs = _PluginCommand.pack(myid, client, command)

            for p in pkgs:
                p.sign(signer)
                _ts3lib.sendPluginCommand(schid, p.toString(),
                                          PluginTargetMode.PluginCommandTarget_CLIENT,
                                          [client], *returnCode)

        return ts3defines.ERROR_ok

    @classmethod
    def _sendRequestKey(cls, schid, sender, target):
        retcode = _ts3lib.createReturnCode()
        req = _RequestKey.pack(sender, target)

        if schid not in cls.requests:
            cls.requests[schid] = dict()

        if len(cls.requests[schid]) > cls.MAX_REQUESTS:
            # this is a security measure to prevent overflowing
            cls.requests[schid].clear()

        if target in cls.requests[schid]:
            # this is a security measure to prevent overflowing
            if len(cls.requests[schid][target] > cls.MAX_REQUESTS_ID):
                cls.requests[schid][target].clear()

            cls.requests[schid][target].append((retcode, req.data))
        else:
            cls.requests[schid][target] = [(retcode, req.data)]

        _ts3lib.sendPluginCommand(schid, req.toString(),
                                  PluginTargetMode.PluginCommandTarget_CLIENT,
                                  [target], retcode)

    @classmethod
    def _sendSendKey(cls, schid, reqcmd):
        if schid not in cls.privkeys:
            cls.privkeys[schid] = RSA.generate(KEYBYTE_SIZE * 8)

        retcode = _ts3lib.createReturnCode()
        cls.sends.append(retcode)
        snd = _SendKey.pack(reqcmd,
                            cls.privkeys[schid].publickey().exportKey())

        _ts3lib.sendPluginCommand(schid, snd.toString(),
                                  PluginTargetMode.PluginCommandTarget_CLIENT,
                                  [snd.receiver], retcode)

    @classmethod
    def _storeCommand(cls, schid, cmd):
        if schid not in cls.cmdstore:
            cls.cmdstore[schid] = dict()

        if len(cls.cmdstore[schid]) > cls.MAX_STORE:
            # this is a security measure, if someone is trying to
            # overflow us with unverified cmds
            _ts3lib.logMessage("Plugincommand store cleared",
                               ts3defines.LogLevel.LogLevel_WARNING,
                               "pyTSon._PluginCommandHandler", 0)
            cls.cmdstore[schid].clear()

        if cmd.sender not in cls.cmdstore[schid]:
            cls.cmdstore[schid][cmd.sender] = [cmd]
        else:
            cls.cmdstore[schid][cmd.sender].append(cmd)

    @classmethod
    def _processCommandstore(cls, schid, clid):
        pubkey = cls.pubkeys[schid][clid]
        firecmds = []

        if schid not in cls.cmdstore:
            return (False, None, None)

        if clid not in cls.cmdstore[schid]:
            return (False, None, None)

        junkkeys = []
        for cmd in cls.cmdstore[schid][clid]:
            if cmd.pkgkey in junkkeys:
                continue

            if not cmd.verify(pubkey):
                # seems to be a faked sender
                if cmd.pkgkey in cls.merges:
                    del cls.merges[cmd.pkgkey]
                junkkeys.append(cmd.pkgkey)
            else:
                if cmd.isComplete():
                    firecmds.append(cmd.data)
                else:
                    if cmd.pkgkey not in cls.merges:
                        cls.merges[cmd.pkgkey] = _PluginCommandMerge()
                    merge = cls.merges[cmd.pkgkey]
                    if not merge.add(cmd):
                        junkkeys.append(cmd.pkgkey)
                        del cls.merges[cmd.pkgkey]
                        _ts3lib.logMessage("Plugincommand merge junked",
                                           ts3defines.LogLevel.LogLevel_DEBUG,
                                           "pyTSon._PluginCommandHandler", 0)
                    elif merge.isComplete():
                        firecmds.append(merge.content)

        if len(firecmds) > 0:
            return (True, clid, firecmds)
        else:
            return (False, None, None)

    @classmethod
    def _handleCommand(cls, schid, command):
        cmd = _PluginCommand.fromString(command)

        if not cmd:
            return (False, None, None)

        err, myid = _ts3lib.getClientID(schid)
        if err != ts3defines.ERROR_ok:
            return (False, None, None)

        if myid != cmd.receiver:
            return (False, None, None)

        if schid not in cls.pubkeys:
            cls.pubkeys[schid] = dict()
        if cmd.sender not in cls.pubkeys[schid]:
            cls._storeCommand(schid, cmd)
            cls._sendRequestKey(schid, myid, cmd.sender)
            return (False, None, None)

        pubkey = cls.pubkeys[schid][cmd.sender]

        if not cmd.verify(pubkey):
            # seems to be a faked sender
            if cmd.pkgkey in cls.merges:
                del cls.merges[cmd.pkgkey]
            return (False, None, None)

        if cmd.isComplete():
            return (True, cmd.sender, [cmd.content])
        else:
            if cmd.pkgkey not in cls.merges:
                if len(cls.merges) > cls.MAX_MERGES:
                    # this is a security measure, if someone is trying to
                    # overflow us with parts
                    _ts3lib.logMessage("Plugincommand merges cleared",
                                       ts3defines.LogLevel.LogLevel_WARNING,
                                       "pyTSon._PluginCommandHandler", 0)
                    cls.merges.clear()

                cls.merges[cmd.pkgkey] = _PluginCommandMerge()
            if not cls.merges[cmd.pkgkey].add(cmd):
                del cls.merges[cmd.pkgkey]
                _ts3lib.logMessage("Plugincommand merge junked",
                                   ts3defines.LogLevel.LogLevel_DEBUG,
                                   "pyTSon._PluginCommandHandler", 0)
                return (False, None, None)
            elif cls.merges[cmd.pkgkey].isComplete():
                merge = cls.merges[cmd.pkgkey]
                del cls.merges[cmd.pkgkey]

                return (True, cmd.sender, [merge.content])

            return (False, None, None)

    @classmethod
    def _handleRequestKey(cls, schid, cmd):
        req = _RequestKey.fromString(cmd)
        if not req:
            return

        err, myid = _ts3lib.getClientID(schid)
        if err != ts3defines.ERROR_ok or req.receiver != myid:
            return

        cls._sendSendKey(schid, req)

    @classmethod
    def _handleSendKey(cls, schid, cmd):
        snd = _SendKey.fromString(cmd)
        if not snd:
            return (False, None, None)

        err, myid = _ts3lib.getClientID(schid)
        if err != ts3defines.ERROR_ok or snd.receiver != myid:
            return

        r = None
        if schid in cls.requests:
            if snd.sender in cls.requests[schid]:
                for (retcode, key) in cls.requests[schid][snd.sender]:
                    if key == snd.token:
                        r = retcode
                        break

        if r:
            cls.requests[schid][snd.sender].remove((r, snd.token))

            if schid not in cls.pubkeys:
                cls.pubkeys[schid] = dict()

            cls.pubkeys[schid][snd.sender] = snd.publickey
            return cls._processCommandstore(schid, snd.sender)

        return (False, None, None)

    @classmethod
    def handlePluginCommand(cls, schid, cmd):
        """

        @return: a tuple containing a boolean which is true, if the events
        should be fired; the id of the server connection handler; the sending
        client and the full cmd (possibly merged with other splitted parts)
        @rtype: tuple(bool, list(int), list(int), list(str))
        """
        try:
            pkgtype = int(cmd[0])
        except ValueError:
            return (False, None, None)

        if pkgtype == _PluginCommandType.normal:
            return cls._handleCommand(schid, cmd)
        elif pkgtype == _PluginCommandType.requestKey:
            cls._handleRequestKey(schid, cmd)
            return (False, None, None)
        elif pkgtype == _PluginCommandType.sendKey:
            return cls._handleSendKey(schid, cmd)

        return (False, None, None)

    @classmethod
    def onConnectStatusChangeEvent(cls, schid, newStatus, errorNumber):
        if newStatus == ts3defines.ConnectStatus.STATUS_DISCONNECTED:
            if schid in cls.pubkeys:
                del cls.pubkeys[schid]

    @classmethod
    def onClientMoveEvent(cls, schid, clientID, oldChannelID, newChannelID,
                          visibility, moveMessage):
        if visibility == ts3defines.Visibility.LEAVE_VISIBILITY:
            if schid in cls.pubkeys:
                if clientID in cls.pubkeys[schid]:
                    del cls.pubkeys[schid][clientID]

    @classmethod
    def onClientMoveMovedEvent(cls, schid, clientID, oldChannelID,
                               newChannelID, visibility, moverID, moverName,
                               moverUniqueIdentifier, moveMessage):
        cls.onClientMoveEvent(schid, clientID, oldChannelID, newChannelID,
                              visibility, "")

    @classmethod
    def onClientMoveSubscriptionEvent(cls, schid, clientID, oldChannelID,
                                      newChannelID, visibility):
        cls.onClientMoveEvent(schid, clientID, oldChannelID, newChannelID,
                              visibility, "")

    @classmethod
    def onClientMoveTimeoutEvent(cls, schid, clientID, oldChannelID,
                                 newChannelID, visibility, timeoutMessage):
        cls.onClientMoveEvent(schid, clientID, oldChannelID, newChannelID,
                              visibility, "")

    @classmethod
    def onClientKickFromServerEvent(cls, schid, clientID, oldChannelID,
                                    newChannelID, visibility, kickerID,
                                    kickerName, kickerUniqueIdentifier,
                                    kickMessage):
        cls.onClientMoveEvent(schid, clientID, oldChannelID, newChannelID,
                              visibility, "")

    @classmethod
    def onServerErrorEvent(cls, schid, errorMessage, error, returnCode,
                           extraMessage):
        if returnCode in cls.sends:
            if error != ts3defines.ERROR_ok:
                _ts3lib.logMessage("Sendkey failed: %s" % errorMessage,
                                   ts3defines.LogLevel.LogLevel_DEBUG,
                                   "pyTSon._PluginCommandHandler", 0)

            cls.sends.remove(returnCode)
            return
        elif error == ts3defines.ERROR_ok:
            return

        if schid in cls.requests:
            reqs = [(receiver, key) for receiver in cls.requests[schid]
                    for (retcode, key) in cls.requests[schid][receiver]
                    if retcode == returnCode]
            # len(reqs) should be always in [0, 1]
            for (receiver, reqkey) in reqs:
                cls.requests[schid][receiver].remove((returnCode, reqkey))
                _ts3lib.logMessage("Requestkey failed: %s" % errorMessage,
                                   ts3defines.LogLevel.LogLevel_DEBUG,
                                   "pyTSon._PluginCommandHandler", 0)


def sendPluginCommand(schid, command, targetMode, targetIDs, *returnCode):
    _PluginCommandHandler.sendPluginCommand(schid, command, targetMode,
                                            targetIDs, *returnCode)


for func in dir(_ts3lib):
    if not func.startswith("__"):
        if func not in globals():
            globals()[func] = getattr(_ts3lib, func)
