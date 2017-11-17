"""
Because the plugin sdk does not emit the sender of a plugin command, we
have to bypass it with some PublicKey infrastructure to add sender
integrity to plugincmds.
To achieve that, each schid has a (temporary) RSA key. The public key can
be requested per plugincommand. This is all done by this class internally,
so script developers can use sendPluginCommand in the usual way.
There are some drawbacks:
* PluginCommandTarget_CURRENT_CHANNEL_SUBSCRIBED_CLIENTS can't be used
anymore, the receiver list is generated on the server, so we can't send
or to be more exact receive commands with this targetmode. The id of the
receiving client has to be in the signed part of the command, otherwise
one could store the raw (signed) command and resend it to another client.
* Same goes for PluginCommandTarget_SERVER, the receiver list is generated
on the server as well. So if someone uses this targetmode here, it's more
like a convenience function, because the clientlist is called and the
command is sent to each clientlist. So the command is only sent to all
clients on the server which are in the sending client's view.
* Because of the above two points there is more traffic on the sending
client's side and hence on the serverside

Furthermore long plugincommands are splitted in parts.

A simplified procedure of the keyexchange between client A and client B:
A: sendPluginCommand normal sender.id receiver.id data sign(hash(ALL))
B: if I have the public key, handle normally (possibly wait for all parts)
B: if not, queue the command,
sendPluginCommand RequestKey sender.id receiver.id AnswerToken
A: sendPluginCommand SendKey sender.id receiver.id AnswerToken publicKey
B: process the queue of A
"""

import _ts3lib
import ts3defines
from ts3defines import PluginTargetMode

import math
import string
from Crypto.PublicKey import RSA
from Crypto.Signature import PKCS1_v1_5
from Crypto.Hash import SHA256
from base64 import b64encode, b64decode
from Crypto.Random import random

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


# Overflow prevention:
MAX_CMD_STORE = 1000
MAX_MERGES = 1000
MAX_REQUESTS = 1000
MAX_REQUESTS_ID = 20


class _PluginCommandType(object):
    unknown = 0
    normal = 1
    requestKey = 2
    sendKey = 3


class _GenericPluginCommand(object):
    """
    Baseclass for all bypassed plugincommands. Handles too long messages
    based on MAX_CMD_SIZE and splits the data to multiple packages.
    """
    pkgtype = _PluginCommandType.unknown

    @classmethod
    def pack(cls, sender, receiver, data):
        """
        Packs some string data to one or more PluginCommands
        @param sender: the own client id
        @type sender: int
        @param receiver: id of the target client
        @type receiver: int
        @param data: data to send
        @type data: str
        @return: a list of plugincommands
        @rtype: list(_GenericPluginCommand)
        """
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
        """
        Parses some a raw data string to a PluginCommand
        @param s:
        @type s: str
        @return: the created PluginCommand or None if parsing failed
        @rtype: _GenericPluginCommand
        """
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
        """
        Instantiates a new object. This should not be invoked directly, use
        pack instead.
        @param sender: id of the sending client
        @type sender: int
        @param receiver: id of the receiving client
        @type receiver: int
        @param data: data of the PluginCommand
        @type data: str
        @param part: number of the splitted package, defaults to 1
        @type part: int
        @param count: count of splitted packages, defaults to 1
        @type count: int
        @param pkgkey: key of the package, defaults to None, then a random key
        is generated
        @type pkgkey: str
        """
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
        """
        List of data fields to pack in serialized string. Each field must be
        convertable to string
        @return: the datalist
        @rtype: list(str-convertable)
        """
        return [self.pkgtype, self.pkgkey, self.sender,
                self.receiver, self.part, self.count, self.data]

    def toString(self):
        """
        Returns the package as serialized string
        @return: the serialized PluginCommand
        @rtype: str
        """
        return " ".join(map(str, self.fields()))

    def isComplete(self):
        """
        @return: Returns True if the command is complete and not part of a
        splitted package
        @rtype: bool
        """
        return self.part == self.count == 1


class _PluginCommand(_GenericPluginCommand):
    """
    Class to abstract user-created PluginCommands. The data can be signed
    with PKCS1 v1.5
    """

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
        """
        Signs the data. The signature is appended to the serialized form
        returned by toString
        @param signer: with privatekey initialized PKCS1 v1.5 signer
        @type signer: Crypto.Signature.PKCS1_v1_5
        """
        self.signature = ""
        h = SHA256.new("".join(map(str, self.fields()[:-1])).encode("utf-8"))
        self.signature = b64encode(signer.sign(h)).decode("utf-8")

    def verify(self, pubkey):
        """
        Verifies that the signature is created by the corresponding privatekey
        @param pubkey: the PEM-encoded publickey
        @type pubkey: bytes
        """
        try:
            rsapub = RSA.importKey(pubkey)
            verifier = PKCS1_v1_5.new(rsapub)
        except:
            return False

        h = SHA256.new("".join(map(str, self.fields()[:-1])).encode("utf-8"))
        return verifier.verify(h, b64decode(self.signature.encode("utf-8")))


class _RequestKey(_GenericPluginCommand):
    """
    Class to send a keyrequest to another client. The answer has to contain
    the requesttoken (send with each request)
    """
    pkgtype = _PluginCommandType.requestKey

    @classmethod
    def pack(cls, sender, receiver):
        """
        Creates a new keyrequest
        @param sender: the id of the sending client
        @type sender: int
        @param receiver: the id of the target client
        @type receiver: int
        @return: the created keyrequest command
        @rtype: _RequestKey
        """
        return _RequestKey(sender, receiver,
                           "".join(random.sample(PKGKEY_ALPHABET, 4)))


class _SendKey(_GenericPluginCommand):
    """
    Class to answer a keyrequest of _RequestKey
    """
    pkgtype = _PluginCommandType.sendKey

    @classmethod
    def pack(cls, reqcmd, publickey):
        """
        Creates a new answer to a keyrequest
        @param reqcmd: the keyrequest command
        @type reqcmd: _RequestKey
        @param publickey: the PEM-encoded publickey
        @type publickey: bytes
        @return: the created keyrequest answer
        @rtype: _SendKey
        """
        return _SendKey(reqcmd.receiver, reqcmd.sender,
                        reqcmd.data + " " + publickey.decode("utf-8"))

    @property
    def token(self):
        """
        Returns the request token of the corresponding keyrequest
        @return: the token
        @rtype: str
        """
        return self.data.split(" ")[0]

    @property
    def publickey(self):
        """
        Returns the publickey of the sending client
        @return: the PEM-encoded publickey
        @rtype: bytes
        """
        return "".join(self.data.split(" ")[1:]).encode("utf-8")


class _PluginCommandMerge(object):
    """
    Class to merge splitted parts of a PluginCommand
    """
    def __init__(self):
        super().__init__()
        self._content = None

    def add(self, cmd):
        """
        Adds a plugincommand to the merge
        @param cmd: the command to add
        @type cmd: _PluginCommand
        @return: returns False on failure, True otherwise
        @rtype: bool
        """
        if self._content is None:
            self._content = [None] * cmd.count
        elif cmd.part > len(self._content):
            return False

        if self._content[cmd.part - 1] is None:
            self._content[cmd.part - 1] = cmd.data
            return True
        else:
            return False

    def isComplete(self):
        """
        @return: Returns True, if all parts are contained in the merge, False
        otherwise
        @rtype: bool
        """
        if self._content:
            return all(self._content)
        else:
            return False

    @property
    def content(self):
        """
        @return: Returns the aggregated data of all splitted parts
        @rtype: str
        """
        return "".join(self._content)


class _PluginCommandStore(object):
    """
    Class to store all needed data by _PluginCommandHandler for one
    server connection
    """
    def __init__(self):
        self.privkey = RSA.generate(KEYBYTE_SIZE * 8)
        self.pubkeys = dict()
        self.sndretcodes = list()
        self.cmdstore = dict()
        self.merges = dict()
        self.requests = dict()

    @property
    def signer(self):
        return PKCS1_v1_5.new(self.privkey)

    def publickey(self, clid=None):
        if clid is None:
            return self.privkey.publickey().exportKey()
        elif clid in self.pubkeys:
            return self.pubkeys[clid]
        return None

    def addPublickey(self, clid, key):
        self.pubkeys[clid] = key

    def removePublickey(self, clid):
        if clid in self.pubkeys:
            del self.pubkeys[clid]

    def storeCommand(self, cmd):
        if len(self.cmdstore) > MAX_CMD_STORE:
            # this is a security measure, if someone is trying to
            # overflow us with unverified cmds
            _ts3lib.logMessage("Plugincommand store cleared",
                               ts3defines.LogLevel.LogLevel_WARNING,
                               "pyTSon._PluginCommandStore", 0)
            self.cmdstore.clear()

        if cmd.sender not in self.cmdstore:
            self.cmdstore[cmd.sender] = [cmd]
        else:
            self.cmdstore[cmd.sender].append(cmd)

    def popStoredCommands(self, clid):
        if clid:
            if clid in self.cmdstore:
                return self.cmdstore.pop(clid)

        return []

    def getMerge(self, pkgkey):
        if pkgkey not in self.merges:
            if len(self.merges) > MAX_MERGES:
                # this is a security measure, if someone is trying to
                # overflow us with parts
                _ts3lib.logMessage("Plugincommand merges cleared",
                                   ts3defines.LogLevel.LogLevel_WARNING,
                                   "pyTSon._PluginCommandHandler", 0)
                self.merges.clear()

            self.merges[pkgkey] = _PluginCommandMerge()

        return self.merges[pkgkey]

    def removeMerge(self, pkgkey):
        if pkgkey in self.merges:
            del self.merges[pkgkey]

    @property
    def sendKeyReturnCodes(self):
        return self.sndretcodes

    def addRequest(self, clid, reqkey, returncode):
        # these are security measures to prevent overflowing
        if len(self.requests) > MAX_REQUESTS:
            self.requests.clear()

        if clid not in self.requests:
            self.requests[clid] = list()

        if len(self.requests[clid]) > MAX_REQUESTS_ID:
            self.requests[clid].clear()

        self.requests[clid].append((reqkey, returncode))

    def hasRequest(self, clid, reqkey):
        if clid in self.requests:
            for (key, returncode) in self.requests[clid]:
                if key == reqkey:
                    return True

        return False

    def getRequest(self, returncode):
        for clid in self.requests:
            for (reqkey, retcode) in self.requests[clid]:
                if retcode == returncode:
                    return (clid, reqkey)

        return (None, None)

    def removeRequest(self, clid, reqkey, returncode):
        self.requests[clid].remove((reqkey, returncode))

    def removeRequests(self, clid):
        del self.requests[clid]

    def clientLeft(self, clid):
        if clid in self.pubkeys:
            del self.pubkeys[clid]
        if clid in self.cmdstore:
            del self.cmdstore[clid]
        if clid in self.requests:
            del self.requests[clid]
        self.merges = {key: cmd for key, cmd in self.merges.items()
                       if cmd.sender != clid}


class _PluginCommandHandler(object):
    """
    Class to handle the raw plugincommand data strings to verify the sender
    and strip the used overhead to emit only the userdata
    """
    stores = dict()

    @classmethod
    def getStore(cls, schid):
        if schid not in cls.stores:
            cls.stores[schid] = _PluginCommandStore()
        return cls.stores[schid]

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

        signer = cls.getStore(schid).signer

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

        cls.getStore(schid).addRequest(target, req.data, retcode)

        _ts3lib.sendPluginCommand(schid, req.toString(),
                                  PluginTargetMode.PluginCommandTarget_CLIENT,
                                  [target], retcode)

    @classmethod
    def _sendSendKey(cls, schid, reqcmd):
        retcode = _ts3lib.createReturnCode()
        cls.getStore(schid).sendKeyReturnCodes.append(retcode)
        snd = _SendKey.pack(reqcmd, cls.getStore(schid).publickey())

        _ts3lib.sendPluginCommand(schid, snd.toString(),
                                  PluginTargetMode.PluginCommandTarget_CLIENT,
                                  [snd.receiver], retcode)

    @classmethod
    def _processCommands(cls, schid, cmds):
        if not cmds:
            return (False, None, None)

        store = cls.getStore(schid)
        pubkey = store.publickey(cmds[0].sender)
        junkkeys = []
        firecmds = []

        for cmd in cmds:
            if cmd.pkgkey in junkkeys:
                continue

            if not cmd.verify(pubkey):
                junkkeys.append(cmd.pkgkey)
                store.removeMerge(cmd.pkgkey)
            elif cmd.isComplete():
                firecmds.append(cmd.data)
            else:
                merge = store.getMerge(cmd.pkgkey)
                if not merge.add(cmd):
                    store.removeMerge(cmd.pkgkey)
                    _ts3lib.logMessage("Plugincommand merge junked",
                                       ts3defines.LogLevel.LogLevel_DEBUG,
                                       "pyTSon._PluginCommandHandler", 0)
                elif merge.isComplete():
                    store.removeMerge(cmd.pkgkey)
                    firecmds.append(merge.content)

        if len(firecmds) > 0:
            return (True, cmds[0].sender, firecmds)
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

        store = cls.getStore(schid)

        pubkey = store.publickey(cmd.sender)
        if pubkey is None:
            store.storeCommand(cmd)
            cls._sendRequestKey(schid, myid, cmd.sender)
            return (False, None, None)

        return cls._processCommands(schid, [cmd])

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

        store = cls.getStore(schid)
        if store.hasRequest(snd.sender, snd.token):
            store.removeRequests(snd.sender)
            store.addPublickey(snd.sender, snd.publickey)
            return cls._processCommands(schid,
                                        store.popStoredCommands(snd.sender))

        return (False, None, None)

    @classmethod
    def handlePluginCommand(cls, schid, cmd):
        """
        This is called by the PluginHost with the raw plugincommand received
        by pyTSon's core. It will request public keys from the sender
        accordingly and might queue commands to fire the event in a later
        call.
        @param schid: the id of the server connection handler
        @type schid: int
        @param cmd: the plugincommand
        @type cmd: str
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
            if schid in cls.stores:
                del cls.stores[schid]

    @classmethod
    def onClientMoveEvent(cls, schid, clientID, oldChannelID, newChannelID,
                          visibility, moveMessage):
        if visibility == ts3defines.Visibility.LEAVE_VISIBILITY:
            if schid in cls.stores:
                cls.stores[schid].clientLeft(clientID)

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
        if schid not in cls.stores:
            return False

        store = cls.getStore(schid)
        if returnCode in store.sendKeyReturnCodes:
            if error != ts3defines.ERROR_ok:
                _ts3lib.logMessage("Sendkey failed: %s" % errorMessage,
                                   ts3defines.LogLevel.LogLevel_DEBUG,
                                   "pyTSon._PluginCommandHandler", 0)

            store.sendKeyReturnCodes.remove(returnCode)
            return True

        (clid, reqkey) = store.getRequest(returnCode)
        if clid:
            if error != ts3defines.ERROR_ok:
                store.removeRequest(clid, reqkey, returnCode)
                _ts3lib.logMessage("Requestkey failed: %s" % errorMessage,
                                   ts3defines.LogLevel.LogLevel_DEBUG,
                                   "pyTSon._PluginCommandHandler", 0)
            return True

        return False
