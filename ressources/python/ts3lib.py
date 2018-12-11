import _ts3lib
import ts3defines
from ts3defines import (ClientProperties, ClientPropertiesRare,
                        ChannelProperties, ChannelPropertiesRare,
                        VirtualServerProperties, VirtualServerPropertiesRare,
                        ConnectionProperties, ConnectionPropertiesRare)

from _plugincmd import _PluginCommandHandler

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
        return _ts3lib.getClientVariableAsInt(schid, clientID, flag)
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
        return _ts3lib.getClientVariableAsUInt64(schid, clientID, flag)
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
        return _ts3lib.getClientSelfVariableAsInt(schid, flag)
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


def getConnectionVariable(schid, clientID, flag):
    if flag in [ConnectionProperties.CONNECTION_CLIENT2SERVER_PACKETLOSS_CONTROL,
                ConnectionProperties.CONNECTION_CLIENT2SERVER_PACKETLOSS_KEEPALIVE,
                ConnectionProperties.CONNECTION_CLIENT2SERVER_PACKETLOSS_SPEECH,
                ConnectionProperties.CONNECTION_CLIENT2SERVER_PACKETLOSS_TOTAL,
                ConnectionProperties.CONNECTION_SERVER2CLIENT_PACKETLOSS_CONTROL,
                ConnectionProperties.CONNECTION_SERVER2CLIENT_PACKETLOSS_KEEPALIVE,
                ConnectionProperties.CONNECTION_SERVER2CLIENT_PACKETLOSS_SPEECH,
                ConnectionProperties.CONNECTION_SERVER2CLIENT_PACKETLOSS_TOTAL,
                ConnectionProperties.CONNECTION_PACKETLOSS_CONTROL,
                ConnectionProperties.CONNECTION_PACKETLOSS_KEEPALIVE,
                ConnectionProperties.CONNECTION_PACKETLOSS_SPEECH,
                ConnectionProperties.CONNECTION_PACKETLOSS_TOTAL,
                ConnectionProperties.CONNECTION_PING,
                ConnectionProperties.CONNECTION_PING_DEVIATION,
                ConnectionPropertiesRare.CONNECTION_FILETRANSFER_BANDWIDTH_RECEIVED,
                ConnectionPropertiesRare.CONNECTION_FILETRANSFER_BANDWIDTH_SENT,
                ConnectionPropertiesRare.CONNECTION_FILETRANSFER_BYTES_RECEIVED_TOTAL,
                ConnectionPropertiesRare.CONNECTION_FILETRANSFER_BYTES_SENT_TOTAL
                ]:
        return _ts3lib.getConnectionVariableAsDouble(schid, clientID, flag)
    elif flag in [ConnectionProperties.CONNECTION_BANDWIDTH_RECEIVED_LAST_MINUTE_CONTROL,
                ConnectionProperties.CONNECTION_BANDWIDTH_RECEIVED_LAST_MINUTE_KEEPALIVE,
                ConnectionProperties.CONNECTION_BANDWIDTH_RECEIVED_LAST_MINUTE_SPEECH,
                ConnectionProperties.CONNECTION_BANDWIDTH_RECEIVED_LAST_MINUTE_TOTAL,
                ConnectionProperties.CONNECTION_BANDWIDTH_RECEIVED_LAST_SECOND_CONTROL,
                ConnectionProperties.CONNECTION_BANDWIDTH_RECEIVED_LAST_SECOND_KEEPALIVE,
                ConnectionProperties.CONNECTION_BANDWIDTH_RECEIVED_LAST_SECOND_SPEECH,
                ConnectionProperties.CONNECTION_BANDWIDTH_RECEIVED_LAST_SECOND_TOTAL,
                ConnectionProperties.CONNECTION_BANDWIDTH_SENT_LAST_MINUTE_CONTROL,
                ConnectionProperties.CONNECTION_BANDWIDTH_SENT_LAST_MINUTE_KEEPALIVE,
                ConnectionProperties.CONNECTION_BANDWIDTH_SENT_LAST_MINUTE_SPEECH,
                ConnectionProperties.CONNECTION_BANDWIDTH_SENT_LAST_MINUTE_TOTAL,
                ConnectionProperties.CONNECTION_BANDWIDTH_SENT_LAST_SECOND_CONTROL,
                ConnectionProperties.CONNECTION_BANDWIDTH_SENT_LAST_SECOND_KEEPALIVE,
                ConnectionProperties.CONNECTION_BANDWIDTH_SENT_LAST_SECOND_SPEECH,
                ConnectionProperties.CONNECTION_BANDWIDTH_SENT_LAST_SECOND_TOTAL,
                ConnectionProperties.CONNECTION_BYTES_RECEIVED_CONTROL,
                ConnectionProperties.CONNECTION_BYTES_RECEIVED_KEEPALIVE,
                ConnectionProperties.CONNECTION_BYTES_RECEIVED_SPEECH,
                ConnectionProperties.CONNECTION_BYTES_RECEIVED_TOTAL,
                ConnectionProperties.CONNECTION_BYTES_SENT_CONTROL,
                ConnectionProperties.CONNECTION_BYTES_SENT_KEEPALIVE,
                ConnectionProperties.CONNECTION_BYTES_SENT_SPEECH,
                ConnectionProperties.CONNECTION_BYTES_SENT_TOTAL,
                ConnectionProperties.CONNECTION_CLIENT_PORT,
                ConnectionProperties.CONNECTION_SERVER_PORT,
                ConnectionProperties.CONNECTION_CONNECTED_TIME,
                ConnectionProperties.CONNECTION_IDLE_TIME,
                ConnectionProperties.CONNECTION_PACKETS_RECEIVED_CONTROL,
                ConnectionProperties.CONNECTION_PACKETS_RECEIVED_KEEPALIVE,
                ConnectionProperties.CONNECTION_PACKETS_RECEIVED_SPEECH,
                ConnectionProperties.CONNECTION_PACKETS_RECEIVED_TOTAL,
                ConnectionProperties.CONNECTION_PACKETS_SENT_CONTROL,
                ConnectionProperties.CONNECTION_PACKETS_SENT_KEEPALIVE,
                ConnectionProperties.CONNECTION_PACKETS_SENT_SPEECH,
                ConnectionProperties.CONNECTION_PACKETS_SENT_TOTAL
                  ]:
        return _ts3lib.getConnectionVariableAsUInt64(schid, clientID, flag)
    else:
        return _ts3lib.getConnectionVariableAsString(schid, clientID, flag)


def getPluginID():
    """
    Returns pyTSon's plugin id
    @return: the plugin id
    @rtype: string
    """
    return ""


def acquireCustomPlaybackData(deviceName, samples):
    """
    Retrieves playback data from the clientlib
    @param deviceName: the name of the playback device previously registered with registerCustomDevice
    @type deviceName: string
    @param samples: specifies how long the resultbuffer should be, which is passed to the clientlib
    @type samples: int
    @return: a tuple containing the errorcode and the buffer as list of ints
    @rtype: tuple(int, list[int])
    """
    return 0,[0]


def activateCaptureDevice(serverConnectionHandlerID):
    """
    Activates the capture device on a serverconnection.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def banadd(serverConnectionHandlerID, ipRegExp, nameRegexp, uniqueIdentity, timeInSeconds, banReason, returnCode):
    """
    Adds a new ban.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param ipRegExp: regular expression to match IPs, pass an empty string to ignore IPs
    @type ipRegExp: string
    @param nameRegexp: regular expression to match client nicknames, pass an empty string to ignore nicknames
    @type nameRegexp: string
    @param uniqueIdentity: client UID to ban, pass an empty string to ignore UIDs
    @type uniqueIdentity: string
    @param timeInSeconds: the time, the client should be banned for, pass 0 to add a permanent ban
    @type timeInSeconds: int
    @param banReason: the reason for the ban
    @type banReason: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def banclient(serverConnectionHandlerID, clientID, timeInSeconds, banReason, returnCode):
    """
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @param timeInSeconds: the time, the client should be banned for, pass 0 to add a permanent ban
    @type timeInSeconds: int
    @param banReason: the reason for the ban
    @type banReason: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def banclientdbid(serverConnectionHandlerID, clientDBID, timeInSeconds, banReason, returnCode):
    """
    Bans a user defined by his database ID.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientDBID: the database ID of the user
    @type clientDBID: int
    @param timeInSeconds: the time, the client should be banned for, pass 0 to add a permanent ban
    @type timeInSeconds: int
    @param banReason: the reason for the ban
    @type banReason: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def bandel(serverConnectionHandlerID, banID, returnCode):
    """
    Deletes a ban.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param banID: the ID of the ban
    @type banID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def bandelall(serverConnectionHandlerID, returnCode):
    """
    Deletes all bans on a server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def channelPropertyStringToFlag(channelPropertyString):
    """
    Converts a channel property name used in strings (eg the serverquery) to the corresponding flag.
    @param channelPropertyString:
    @type channelPropertyString: string
    @return: a tuple, containing the errorcode and the flag (see ts3defines.ChannelProperties and ts3defines.ChannelPropertiesRare)
    @rtype: tuple (int, int)
    """
    return 0,0


def channelset3DAttributes(serverConnectionHandlerID, clientID, position):
    """
    Adjusts a clients position and velocity in 3D space.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client to adjust
    @type clientID: int
    @param position: a tuple defining the position of the clientID
    @type position: tuple (float, float, float)
    @return: the errorcode
    @rtype: int
    """
    return 0


def cleanUpConnectionInfo(serverConnectionHandlerID, clientID):
    """
    //FIXME:
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def clientChatClosed(serverConnectionHandlerID, clientUniqueIdentifier, clientID, returnCode):
    """
    Sends the client chat closed command to a client the own client is currently chatting with.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientUniqueIdentifier: the uid of the own chatting client
    @type clientUniqueIdentifier: string
    @param clientID: the ID of the client, the own client is chatting with
    @type clientID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def clientChatComposing(serverConnectionHandlerID, clientID, returnCode):
    """
    Sends the client chat composing command to a client the own client is currently chatting with.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client, the own client is chatting with
    @type clientID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def clientPropertyStringToFlag(clientPropertyString):
    """
    Converts a client property name used in strings (eg the serverquery) to the corresponding flag.
    @param clientPropertyString:
    @type clientPropertyString: string
    @return: a tuple, containing the errorcode and the flag (see ts3defines.ClientProperties and ts3defines.ClientPropertiesRare)
    @rtype: tuple (int, int)
    """
    return 0,0


def closeCaptureDevice(serverConnectionHandlerID):
    """
    Closes a capture device on a serverconnection.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def closePlaybackDevice(serverConnectionHandlerID):
    """
    Closes a playback device on a serverconnection.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def closeWaveFileHandle(serverConnectionHandlerID, waveHandle):
    """
    Closes a wavefile sound handle previously returned by playWaveFileHandle.
    @param serverConnectionHandlerID: the ID of the serverConnection the sound was played on
    @type serverConnectionHandlerID: int
    @param waveHandle: the handle returned by playWaveFileHandle
    @type waveHandle: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def createBookmark(bookmarkuuid, serverLabel, serverAddress, serverPassword, nickname, channel, channelPassword,
                   captureProfile, playbackProfile, hotkeyProfile, soundProfile, uniqueUserId, oneTimeKey,
                   phoneticName):
    """
    Creates a new bookmark.
    @param serverLabel: the label of the connection
    @type serverLabel: string
    @param serverAddress: host or ip address
    @type serverAddress: string
    @param serverPassword: password to the server, pass an empty string if the server is not password protected
    @type serverPassword: string
    @param nickname: the user's nickname
    @type nickname: string
    @param channel: complete path to the channel to connect to
    @type channel: string
    @param channelPassword: password to the channel, pass an empty string if the channel is not password protected
    @type channelPassword: string
    @param captureProfile: the name of the capture profile to use; pass an empty string to always use the default one
    @type captureProfile: string
    @param playbackProfile: the name of the playback profile to use; pass an empty string to always use the default one
    @type playbackProfile: string
    @param hotkeyProfile: the name of the hotkey profile to use; pass an empty string to always use the default one
    @type hotkeyProfile: string
    @param soundProfile: the name of the sound profile to use; pass an empty string to always use the default one
    @type soundProfile: string
    @param uniqueUserId: identity (name) to use; pass an empty string to always use the default one
    @type uniqueUserId: string
    @param oneTimeKey: privilege key to use on connect
    @type oneTimeKey: string
    @param phoneticName: phonetic nickname
    @type phoneticName: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def createReturnCode(maxLen=128):
    """
    Creates a returnCode which can be passed to the other functions and will be passed to the event onServerErrorEvent.
    @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
    @type maxLen: int
    @return: the created returnCode
    @rtype: string
    """
    return ""


def destroyServerConnectionHandler(serverConnectionHandlerID):
    """
    Destroys a server connection handler.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def flushChannelCreation(serverConnectionHandlerID, channelParentID, returnCode):
    """
    Flushes the channel creation made by the setChannelVariable-functions to the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelParentID: the ID of the parent channel of the new channel
    @type channelParentID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def flushChannelUpdates(serverConnectionHandlerID, channelID, returnCode):
    """
    Flushes the changes made by the setChannelVariable-functions to the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channelID
    @type channelID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def flushClientSelfUpdates(serverConnectionHandlerID, returnCode):
    """
    Flushes the changes made by the setClientSelfVariable-functions to the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def getAppPath(maxLen=256):
    """
    Returns the ts3 application path.
    @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
    @type maxLen: int
    @return: the application path
    @rtype: string
    """
    return 0


def getAvatar(serverConnectionHandlerID, clientID, maxLen=256):
    """
    Returns the path on the system to the avatar image file of a client.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
    @type maxLen: int
    @return: a tuple, containing the errorcode and the path to the avatar
    @rtype: tuple (int, string)
    """
    return 0,""


def getAverageTransferSpeed(transferID):
    """
    Returns the average transfer speed of a filetransfer.
    @param transferID: the ID of the filetransfer
    @type transferID: int
    @return: a tuple, containing the errorcode and the speed
    @rtype: tuple (int, float)
    """
    return 0,0.0


def getBookmarkList():
    """
    Returns the list of bookmarks.
    @return: a tuple, containing the errorcode and a list of tuples (name, isFolder, uid, childs)
    @rtype: tuple (int, [tuple (string, int or bool, string or None, [childs])])
    """
    return 0, ("",0, "", [])


def getCaptureDeviceList(modeID):
    """
    Queries all available capture devices.
    @param modeID: Defines the capture mode to use.
    @type modeID: string
    @return: A tuple, containing the errorcode and the list of capture devices as tuple (devicename, deviceid)
    @rtype: tuple (int, [(string, string)])
    """
    return 0,[("","")]


def getCaptureModeList():
    """
    Queries all available capture modes.
    @return: A tuple, containing the errorcode and the list of capture modes
    @rtype: tuple (int, [string])
    """
    return 0,[""]


def getChannelClientList(serverConnectionHandlerID, channelID):
    """
    Returns all clients in a specified channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @return: a tuple, containing the errorcode and a list of client IDs or None if the call failed
    @rtype: tuple (int, [int]) or tuple(int, None)
    """
    return 0,[0]

def getChannelConnectInfo(serverConnectionHandlerID, channelID, maxLen):
    """
    Returns the channel connect info (path and password) of a channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
    @type maxLen: int
    @return: a tuple, containing the errorcode, the path and the password of a channel
    @rtype: tuple (int, string, string)
    """
    return 0,"",""


def getChannelIDFromChannelNames(serverConnectionHandlerID, channelNameArray):
    """
    Returns the ID of a channel defined by its name.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelNameArray: list of strings, defining the position of the channel (['grandparent', 'parent', 'channel'])
    @type channelNameArray: list [string]
    @return: a tuple, containing the errorcode and the ID of the channel
    @rtype: tuple (int, int)
    """
    return 0,0


def getChannelList(serverConnectionHandlerID):
    """
    Returns all channels on the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: a tuple, containing the errorcode and a list of channel IDs
    @rtype: tuple (int, [int])
    """
    return 0,[0]


def getChannelOfClient(serverConnectionHandlerID, clientID):
    """
    Returns the channel of a client.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @return: a tuple, containing the errorcode and the channel
    @rtype: tuple (int, int)
    """
    return 0,0


def getChannelVariableAsInt(serverConnectionHandlerID, channelID, flag):
    """
    Returns a channel variable as int value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param flag: the flag to return
    @type flag: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, int)
    """
    return 0,0


def getChannelVariableAsString(serverConnectionHandlerID, channelID, flag):
    """
    Returns a channel variable as string value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param flag: the flag to return
    @type flag: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, int)
    """
    return 0,0


def getChannelVariableAsUInt64(serverConnectionHandlerID, channelID, flag):
    """
    Returns a channel variable as unsigned long long int value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param flag: the flag to return
    @type flag: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, int)
    """
    return 0,0


def getClientDisplayName(serverConnectionHandlerID, clientID, maxLen=128):
    """
    Returns the client display name receiving from the client's contacts settings.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 128
    @type maxLen: int
    @return: a tuple, containing the errorcode and the display name
    @rtype: tuple (int, string)
    """
    return 0,""


def getClientID(serverConnectionHandlerID):
    """
    Returns the own client ID on a given serverconnection.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: a tuple, containing the errorcode and the client ID
    @rtype: tuple (int, int)
    """
    return 0,0


def getClientLibVersion():
    """
    Returns the clientlib's version as string.
    @return: A tuple, containing the errorcode and the result
    @rtype: tuple (int, string)
    """
    return 0,""


def getClientLibVersionNumber():
    """
    Returns the clientlib's version number
    @return: A tuple, containing the errorcode and the result
    @rtype: tuple (int, int)
    """
    return 0,0


def getClientList(serverConnectionHandlerID):
    """
    Returns all clients in view on the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: a tuple, containing the errorcode and the list of client IDs
    @rtype: tuple (int, [int])
    """
    return 0,[0]


def getClientNeededPermission(serverConnectionHandlerID, permissionName):
    """
    Returns the value of the client's needed permission.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param permissionName: name of the permission
    @type permissionName: string
    @return: a tuple, containing the errorcode and the value of the permission
    @rtype: tuple (int, int)
    """
    return 0,0


def getClientSelfVariableAsInt(serverConnectionHandlerID, flag):
    """
    Returns the value of a given flag of the own client as int value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param flag: the flag to return
    @type flag: int
    @return: a tuple, containing the errorcode and the value of the queried flag
    @rtype: tuple (int, int)
    """
    return 0,0


def getClientSelfVariableAsString(serverConnectionHandlerID, flag):
    """
    Returns the value of a given flag of the own client as string.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param flag: the flag to return
    @type flag: int
    @return: a tuple, containing the errorcode and the value of the queried flag
    @rtype: tuple (int, string)
    """
    return 0,""


def getClientVariableAsInt(serverConnectionHandlerID, clientID, flag):
    """
    Returns the value of a given flag of a client as int value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @param flag: the flag to return
    @type flag: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, int)
    """
    return 0,0


def getClientVariableAsString(serverConnectionHandlerID, clientID, flag):
    """
    Returns the value of a given flag of a client as string value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @param flag: the flag to return
    @type flag: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, string)
    """
    return 0,""


def getClientVariableAsUInt64(serverConnectionHandlerID, clientID, flag):
    """
    Returns the value of a given flag of a client as unsigned long long int value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @param flag: the flag to return
    @type flag: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, int)
    """
    return 0,0


def getConfigPath(maxLen=256):
    """
    Returns the ts3 config path.
    @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
    @type maxLen: int
    @return: the config path
    @rtype: string
    """
    return ""


def getConnectionStatus(serverConnectionHandlerID):
    """
    Returns the current connection status of a serverconnection.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: a tuple, containing the errorcode and the connection status
    @rtype: tuple (int, int)
    """
    return 0,0


def getConnectionVariableAsDouble(serverConnectionHandlerID, clientID, flag):
    """
    Returns a client's connection variable as python floating point variable.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, double)
    """
    return 0,0.0


def getConnectionVariableAsString(serverConnectionHandlerID, clientID, flag):
    """
    Returns a client's connection variable as string variable.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, string)
    """
    return 0,""


def getConnectionVariableAsUInt64(serverConnectionHandlerID, clientID, flag):
    """
    Returns a client's connection variable as unsigned long long int variable.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, int)
    """
    return 0,0


def getCurrentCaptureDeviceName(serverConnectionHandlerID):
    """
    Queries the current playback device's name on a serverconnection.
    @param serverConnectionHandlerID: ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: A tuple, containing the errorcode, the capture device's name and the status, if it's default
    @rtype: tuple (int, string, int)
    """
    return 0,"",0


def getCurrentCaptureMode(serverConnectionHandlerID):
    """
    Queries the current capture mode on a serverconnection.
    @param serverConnectionHandlerID: ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: A tuple, containing the errorcode and the current capture mode
    @rtype: tuple (int, string)
    """
    return 0,""


def getCurrentPlaybackDeviceName(serverConnectionHandlerID):
    """
    Queries the current playback device's name on a serverconnection.
    @param serverConnectionHandlerID: ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: A tuple, containing the errorcode, the playback device's name and the status, if it's default
    @rtype: tuple (int, string, int)
    """
    return 0,"",0


def getCurrentPlayBackMode(serverConnectionHandlerID):
    """
    Queries the current playback mode on a serverconnection.
    @param serverConnectionHandlerID: ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: A tuple, containing the errorcode and the current playback mode
    @rtype: tuple (int, string)
    """
    return 0,""


def getCurrentServerConnectionHandlerID():
    """
    Returns the current serverconnection handler.
    @return: the ID of the current serverconnection handler
    @rtype: int
    """
    return 0


def getCurrentTransferSpeed(transferID):
    """
    Returns the current transfer speed of a filetransfer.
    @param transferID: the ID of the filetransfer
    @type transferID: int
    @return: a tuple, containing the errorcode and the speed
    @rtype: tuple (int, float)
    """
    return 0,0.0


def getDefaultCaptureDevice(modeID):
    """
    Queries the default capture device.
    @param modeID: Defines the capture mode to use
    @type modeID: string
    @return: A tuple, containing the errorcode and the default capture device as tuple (devicename, deviceid)
    @rtype: tuple (int, (string, string))
    """
    return 0,("","")


def getDefaultCaptureMode():
    """
    Queries the default capture mode.
    @return: A tuple, containing the errorcode and the default capture mode
    @rtype: tuple (int, string)
    """
    return 0,""


def getDefaultPlaybackDevice(modeID):
    """
    Queries the default playback device.
    @param modeID: Defines the playback mode to use
    @type modeID: string
    @return: A tuple, containing the errorcode and the default playback device as tuple (devicename, deviceid)
    @rtype: tuple (int, (string, string))
    """
    return 0,("","")


def getDefaultPlayBackMode():
    """
    Queries the default playback mode.
    @return: A tuple, containing the errorcode and the default playback mode
    @rtype: tuple (int, string)
    """
    return 0,""


def getDirectories(path, maxLen=256):
    """
    Returns a list of subdirectories of a path as space-separated string.
    @param path: the parent path
    @type path: str
    @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
    @type maxLen: int
    @return: the resulting path
    @rtype: string
    """
    return ""


def getEncodeConfigValue(serverConnectionHandlerID, ident):
    """
    Queries a speex encoder option.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param ident: the encoder option to be queried
    @type ident: string
    @return: A tuple, containing the errorcode and the flag's value
    @rtype: tuple (int, string)
    """
    return 0,""


def getErrorMessage(errorCode):
    """
    Queries a printable error string for a specific error code.
    @param errorCode: The error code returned from all Client Lib functions
    @type errorCode: int
    @return: A tuple, containing the errorcode and the resulting string
    @rtype: tuple (int, string)
    """
    return 0,""


def getHotkeyFromKeyword(keywords):
    """
    Returns a list of hotkeys by its keywords.
    @param keywords: a list of keywords
    @type keywords: list[str]
    @return: a tuple containing the errorcode and the list of hotkeys
    @rtype: tuple(int, list[str])
    """
    return 0,[""]


def getParentChannelOfChannel(serverConnectionHandlerID, channelID):
    """
    Returns the parent channel of another channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @return: a tuple, containing the errorcode and the ID of the parent channel
    @rtype: tuple (int, int)
    """
    return 0,0


def getPermissionIDByName(serverConnectionHandlerID, permissionName):
    """
    Returns the ID of a permission defined by its name.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param permissionName: name of the permission
    @type permissionName: string
    @return: a tuple, containing the errorcode and the ID of the permission
    @rtype: tuple (int, int)
    """
    return 0,0

def getPlaybackConfigValueAsFloat(serverConnectionHandlerID, ident):
    """
    Queries a playback option.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param ident: the playback option to be queried
    @type ident: string
    @return: A tuple, containing the errorcode and the flag's value
    @rtype: tuple (int, float)
    """
    return 0,0.0


def getPlaybackDeviceList(modeID):
    """
    Queries all available playback devices.
    @param modeID: Defines the playback mode to use.
    @type modeID: string
    @return: A tuple, containing the errorcode and the list of playback devices as tuple (devicename, deviceid)
    @rtype: tuple (int, [(string, string)])
    """
    return 0,[("","")]


def getPlaybackModeList():
    """
    Queries all available playback modes.
    @return: A tuple, containing the errorcode and the list of modes
    @rtype: tuple (int, [string])
    """
    return 0,[""]


def getPluginPath(maxLen):
    """
    Returns the TeamSpeak 3 client's pluginpath.
    @param maxLen: the size of the buffer passed to the clientlib. Optional, defaults to 256
    @type maxLen: int
    @return: the pluginpath
    @rtype: str
    """
    return ""


def getPreProcessorInfoValue(serverConnectionHandlerID, ident):
    """
    Querie a sound preprocessor flag and returns it as string.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param ident: the flag to be queried
    @type ident: string
    @return: A tuple, containing the errorcode and the value of the queried flag
    @rtype: tuple (int, string)
    """
    return 0,""


def getPreProcessorInfoValueFloat(serverConnectionHandlerID, ident):
    """
    Queries a sound preprocessor flag and returns it as float.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param ident: the flag to be queried
    @type ident: string
    @return: A tuple, containing the errorcode and the value of the queried flag
    @rtype: tuple (int, float)
    """
    return 0,0.0


def getProfileList(profile):
    """
    Returns a list of existing profiles and the default profile's index in list.
    @param profile: the profile type, see ts3defines.PluginGuiProfile
    @type profile: int
    @return: a tuple, containing the errorcode, the default profile's index and the profile list
    @rtype: tuple (int, int, [string])
    """
    return 0,0,[""]


def getResourcesPath(maxLen=256):
    """
    Returns the ts3 resources path.
    @param maxLen: length of the buffer, passed to the clientlib to store the path to. Optional, defaults to 256
    @type maxLen: int
    @return: the resources path
    @rtype: string
    """
    return ""


def getServerConnectInfo(serverConnectionHandlerID, maxLen=256):
    """
    Returns the connect info (host, port and password) of a serverconnection.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
    @type maxLen: int
    @return: a tuple, containing the errorcode, the host, the port and the password of the serverconnection
    @rtype: tuple (int, string, int, string)
    """
    return 0,"",0,""


def getServerConnectionHandlerList():
    """
    Returns a list of serverconnection handlers.
    @return: a tuple, containing the errorcode and the list of serverconnection handler IDs
    @rtype: tuple (int, [int])
    """
    return 0,[0]


def getServerVariableAsInt(serverConnectionHandlerID, flag):
    """
    Returns a server variable as int value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param flag: the flag to return
    @type flag: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, int)
    """
    return 0,0


def getServerVariableAsString(serverConnectionHandlerID, flag):
    """
    Returns a server variable as string value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param flag: the flag to return
    @type flag: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, string)
    """
    return 0,""


def getServerVariableAsUInt64(serverConnectionHandlerID, flag):
    """
    Returns a server variable as unsigned long long int value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param flag: the flag to return
    @type flag: int
    @return: a tuple, containing the errorcode and the value of the flag
    @rtype: tuple (int, int)
    """
    return 0,0


def getServerVersion(serverConnectionHandlerID):
    """
    Returns the server version.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: the server version
    @rtype: int
    """
    return 0


def getTransferFileName(transferID):
    """
    Returns the filename of a filetransfer.
    @param transferID: the ID of the filetransfer
    @type transferID: int
    @return: a tuple, containing the errorcode and the filename
    @rtype: tuple (int, string)
    """
    return 0,""


def getTransferFilePath(transferID):
    """
    Returns the filepath of a filetransfer.
    @param transferID: the ID of the filetransfer
    @type transferID: int
    @return: a tuple, containing the errorcode and the filepath
    @rtype: tuple (int, string)
    """
    return 0,""


def getTransferFileSize(transferID):
    """
    Returns the total filesize (in Bytes) of a filetransfer.
    @param transferID: the ID of the filetransfer
    @type transferID: int
    @return: a tuple, containing the errorcode and the filesize
    @rtype: tuple (int, int)
    """
    return 0,0


def getTransferFileSizeDone(transferID):
    """
    Returns the already downloaded size (in Bytes) of a filetransfer.
    @param transferID: the ID of the filetransfer
    @type transferID: int
    @return: a tuple, containing the errorcode and the size
    @rtype: tuple (int, int)
    """
    return 0,0


def getTransferRunTime(transferID):
    """
    Returns the runtime of a filetransfer.
    @param transferID: the ID of the filetransfer
    @type transferID: int
    @return: a tuple, containing the errorcode and the runtime in seconds
    @rtype: tuple (int, int)
    """
    return 0,0


def getTransferStatus(transferID):
    """
    Returns the status of a filetransfer, whether if it is initialising, active or finished see ts3defines.FileTransferState
    @param transferID: the ID of the filetransfer
    @type transferID: int
    @return: a tuple, containing the errorcode and the status
    @rtype: tuple (int, int)
    """
    return 0,0


def guiConnect(connectTab, serverLabel, serverAddress, serverPassword, nickname, channel, channelPassword,
               captureProfile, playbackProfile, hotkeyProfile, userIdentity, oneTimeKey, phoneticName):
    """
    Connects to a server and displays it as tab in the client.
    @param connectTab: defines, which tab will be used, see ts3defines.PluginConnectTab
    @type connectTab: int
    @param serverLabel: the label of the connection
    @type serverLabel: string
    @param serverAddress: host or ip address
    @type serverAddress: string
    @param serverPassword: password to the server, pass an empty string if the server is not password protected
    @type serverPassword: string
    @param nickname: the user's nickname
    @type nickname: string
    @param channel: complete path to the channel to connect to
    @type channel: string
    @param channelPassword: password to the channel, pass an empty string if the channel is not password protected
    @type channelPassword: string
    @param captureProfile: the name of the capture profile to use
    @type captureProfile: string
    @param playbackProfile: the name of the playback profile to use
    @type playbackProfile: string
    @param hotkeyProfile: the name of the hotkey profile to use
    @type hotkeyProfile: string
    @param userIdentity: identity to use
    @type userIdentity: string
    @param oneTimeKey: privilege key to use on connect
    @type oneTimeKey: string
    @param phoneticName: phonetic nickname
    @type phoneticName: string
    @return: a tuple, containing the errorcode and the ID of the created serverconnection handler
    @rtype: tuple (int, int)
    """
    return 0,0


def guiConnectBookmark(connectTab, bookmarkuuid):
    """
    Connects to a server from a bookmark and displays it as tab in the client.
    @param connectTab: defines, which tab will be used, see ts3defines.PluginConnectTab
    @type connectTab: int
    @param bookmarkuuid: UID of the bookmark
    @type bookmarkuuid: string
    @return: a tuple, containing the errorcode and the ID of the created serverconnection handler
    @rtype: tuple (int, int)
    """
    return 0,0


def haltTransfer(serverConnectionHandlerID, transferID, deleteUnfinishedFile, returnCode):
    """
    Halts a currently running filetransfer.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param transferID: the ID of the filetransfer
    @type transferID: int
    @param deleteUnfinishedFile: if set to 1 (or True) and the file is not yet finished, it will be deleted; to prevent, pass 0 (or False)
    @type deleteUnfinishedFile: int or bool
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def initiateGracefulPlaybackShutdown(serverConnectionHandlerID):
    """
    Graceful shutdown the playback device on a serverconnection.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def isReceivingWhisper(serverConnectionHandlerID, clientID):
    """
    //FIXME:
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @return: a tuple, containing the errorcode and the status
    @rtype: tuple (int, int or bool)
    """
    return 0,0


def isTransferSender(transferID):
    """
    Returns the upload/download direction of a filetransfer.
    @param transferID: the ID of the filetransfer
    @type transferID: int
    @return: a tuple, containing the errorcode and 1 if it's an upload or 0 if it's a download
    @rtype: tuple (int, int or bool)
    """
    return 0,0


def isWhispering(serverConnectionHandlerID, clientID):
    """
    Returns the status of a client whether he is currently whispering to the own client.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: a tuple, containing the errorcode and the status
    @rtype: tuple (int, int or bool)
    """
    return 0,0


def logMessage(logMessage, severity, channel, logID):
    """
    Logs a string.
    @param logMessage: Text which should be logged
    @type logMessage: string
    @param severity: The level of the message, warning or error. Defined by the class LogLevel
    @type severity: int
    @param channel: Custom text to categorize the message channel
    @type channel: string
    @param logID: ID of the serverconnection to identify the current server connection when using multiple connections, 0 if unused
    @type logID: int
    @return: The errorcode
    @rtype: int
    """
    return 0


def openCaptureDevice(serverConnectionHandlerID, modeID, captureDevice):
    """
    Opens a playback device on a serverconnection.
    @param serverConnectionHandlerID: ID of the serverconnection on which the capture device should be initialized on
    @type serverConnectionHandlerID: int
    @param modeID: the playback mode to use
    @type modeID: string
    @param captureDevice: the id of the capture device
    @type captureDevice: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def openPlaybackDevice(serverConnectionHandlerID, modeID, playbackDevice):
    """
    Opens a playback device on a serverconnection.
    @param serverConnectionHandlerID: ID of the serverconnection on which the playback device should be initialized on
    @type serverConnectionHandlerID: int
    @param modeID: the playback mode to use
    @type modeID: string
    @param playbackDevice: the id of the playback device
    @type playbackDevice: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def pauseWaveFileHandle(serverConnectionHandlerID, waveHandle, pause):
    """
    Pauses a wavefile sound previously started with playWaveFileHandle.
    @param serverConnectionHandlerID: the ID of the serverConnection the sound is played on
    @type serverConnectionHandlerID: int
    @param waveHandle: the handle returned by playWaveFileHandle
    @type waveHandle: int
    @param pause: if set to 1 (or True), the sound will pause, 0 (or False) will unpause the sound
    @type pause: int or bool
    @return: the errorcode
    @rtype: int
    """
    return 0


def playWaveFile(serverConnectionHandlerID, path):
    """
    Plays a wavefile sound on a serverconnection.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param path: the path to the wavefile on the system
    @type path: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def playWaveFileHandle(serverConnectionHandlerID, path, loop):
    """
    Plays a wavefile sound on a serverconnection and returns a handle to it.
    @param serverConnectionHandlerID: the ID of the serverconnection on which the sound will be played on
    @type serverConnectionHandlerID: int
    @param path: the path to the wavefile on the system
    @type path: string
    @param loop: if set to 1 (or True), the sound will loop
    @type loop: int or bool
    @return: A tuple, containing the errorcode and the handle, with which the sound can be paused and unpaused
    @rtype: tuple (int, int)
    """
    return 0,0


def printMessage(serverConnectionHandlerID, message, messageTarget):
    """
    Prints a message to a specific client chat tab.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param message: the message to print
    @type message: string
    @param messageTarget: the target to send the message, see ts3defines.PluginMessageTarget
    @type messageTarget: int
    """
    pass


def printMessageToCurrentTab(message):
    """
    Prints a message to the currently visible tab.
    @param message: the message to send
    @type message: string
    """
    pass


def privilegeKeyUse(serverConnectionHandlerID, tokenKey, returnCode):
    """
    Uses a privilege key as the current client of the serverconnection.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param tokenKey: the token
    @type tokenKey: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def processCustomCaptureData(deviceName, buffer):
    """
    Sends captured data from a custom device to the client libg
    @param deviceName: the name of the device capturing the data, previously registered with registerCustomDevice
    @type deviceName: string
    @param buffer: a list containing the buffered data
    @type buffer: list [int]
    @return: the errorcode
    @rtype: int
    """
    return 0


def registerCustomDevice(deviceID, deviceDisplayName, capFrequency, capChannels, playFrequency, playChannels):
    """
    Registers a custom device, announcing the device ID and name to the Client Lib.
    @param deviceID: ID string of the custom device, under which the device can be later accessed
    @type deviceID: string
    @param deviceDisplayName: Displayed name of the custom device. Freely choose a name which identifies your device
    @type deviceDisplayName: string
    @param capFrequency: Frequency of the capture device
    @type capFrequency: int
    @param capChannels: Number of channels of the capture device. This value depends on if the used codec is a mono or stereo CodecEncryptionMode
    @type capChannels: int
    @param playFrequency: Frequency of the playback deviceDisplayName
    @type playFrequency: int
    @param playChannels: Number of channels of the playback device
    @type playChannels: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestBanList(serverConnectionHandlerID, returnCode):
    """
    Requests the banlist on a server. The event onBanListEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelAddPerm(serverConnectionHandlerID, channelID, permissionIDArray, permissionValueArray, returnCode):
    """
    Adds a list of permissions to a channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param permissionIDArray: a list of permission IDs
    @type permissionIDArray: list [int]
    @param permissionValueArray: list of permission values, in order of the permissions in permissionIDArray
    @type permissionValueArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelClientAddPerm(serverConnectionHandlerID, channelID, clientDatabaseID, permissionIDArray,
                                permissionValueArray, returnCode):
    """
    Adds a list of permissions on a channel to a user.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param clientDatabaseID: the database ID of the user
    @type clientDatabaseID: int
    @param permissionIDArray: a list of permission IDs
    @type permissionIDArray: list [int]
    @param permissionValueArray: list of permission values, in order of the permissions in permissionIDArray
    @type permissionValueArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelClientDelPerm(serverConnectionHandlerID, channelID, clientDatabaseID, permissionIDArray, returnCode):
    """
    Deletes a list of permissions of a user in a channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param clientDatabaseID: the database ID of the user
    @type clientDatabaseID: int
    @param permissionIDArray: a list of permission IDs
    @type permissionIDArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelClientPermList(serverConnectionHandlerID, channelID, clientDatabaseID, returnCode):
    """
    Requests the list of permissions of a user in a channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param clientDatabaseID: the database ID of the user
    @type clientDatabaseID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelDelete(serverConnectionHandlerID, channelID, force, returnCode):
    """
    Deletes a channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel to delete
    @type channelID: int
    @param force: if set to 1 (or True), the channel will be deleted even when it is not empty
    @type force: int or bool
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelDelPerm(serverConnectionHandlerID, channelID, permissionIDArray, returnCode):
    """
    Deletes a list of permissions from a channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param permissionIDArray: a list of permission IDs
    @type permissionIDArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelDescription(serverConnectionHandlerID, channelID, returnCode):
    """
    Requests the channel description of a channel. Afterwards, getChannelVariableAsString can return it.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelGroupAdd(serverConnectionHandlerID, groupName, groupType, returnCode):
    """
    Adds a channelgroup.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param groupName: the name of the channelgroup to create
    @type groupName: string
    @param groupType: type of the channelgroup, see ts3defines.GroupType
    @type groupType: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelGroupAddPerm(serverConnectionHandlerID, channelGroupID, continueonerror, permissionIDArray,
                               permissionValueArray, returnCode):
    """
    Adds a list of permissions to a channelgroup.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelGroupID: the ID of the channelgroup
    @type channelGroupID: int
    @param continueonerror: if set to True, if an error with a permission occurs, the other permissions will even though be handled
    @type continueonerror: bool
    @param permissionIDArray: a list of permission IDs
    @type permissionIDArray: list [int]
    @param permissionValueArray: list of permission values, in order of the permissions in permissionIDArray
    @type permissionValueArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelGroupDel(serverConnectionHandlerID, channelGroupID, force, returnCode):
    """
    Deletes a channelgroup.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelGroupID: the ID of the channelgroup
    @type channelGroupID: int
    @param force: if set to 1 (or True), even if there are users assigned to this channelgroup, it will be deleted
    @type force: int or bool
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelGroupDelPerm(serverConnectionHandlerID, channelGroupID, continueOnError, permissionIDArray,
                               returnCode):
    """
    Deletes a list of permissions from a channelgroup.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelGroupID: the ID of the channelgroup
    @type channelGroupID: int
    @param continueOnError: if set to 1 (or True), if an error with a permission occurs, the other permissions will even though be handled
    @type continueOnError: int or bool
    @param permissionIDArray: a list of permission IDs
    @type permissionIDArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelGroupList(serverConnectionHandlerID, returnCode):
    """
    Requests the list of channelgroups. The events onChannelGroupListEvent and onChannelGroupListEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelGroupPermList(serverConnectionHandlerID, channelGroupID, returnCode):
    """
    Requests the list of permissions assigned to a channelgroup. The events onChannelGroupPermListEvent and onChannelGroupPermListFinishedEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelGroupID: the ID of the channelGroupID
    @type channelGroupID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelMove(serverConnectionHandlerID, channelID, newChannelParentID, newChannelOrder, returnCode):
    """
    Moves a channel to a new parent channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel to move
    @type channelID: int
    @param newChannelParentID: the ID of the new parent channel
    @type newChannelParentID: int
    @param newChannelOrder: Channel order defining where the channel should be sorted under the new parent. Pass 0 to sort the channel right after the parent
    @type newChannelOrder: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelPermList(serverConnectionHandlerID, channelID, returnCode):
    """
    Requests the list of permissions assigned to a channel. The events onChannelPermListEvent and onChannelPermListFinishedEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelSubscribe(serverConnectionHandlerID, channelIDArray, returnCode):
    """
    Subscribes to a list of channels to get notifications of the clients in them.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelIDArray: a list of channel IDs
    @type channelIDArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelSubscribeAll(serverConnectionHandlerID, returnCode):
    """
    Subscribes to all channels on the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelUnsubscribe(serverConnectionHandlerID, channelIDArray, returnCode):
    """
    Unsubscribes from a list channels.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelIDArray: a list of channel IDs
    @type channelIDArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestChannelUnsubscribeAll(serverConnectionHandlerID, returnCode):
    """
    Unsubscribes from all channels on the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientAddPerm(serverConnectionHandlerID, clientDatabaseID, permissionIDArray, permissionValueArray,
                         permissionSkipArray, returnCode):
    """
    Adds a list of permissions to a user.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientDatabaseID: the database ID of the user
    @type clientDatabaseID: int
    @param permissionIDArray: a list of permission IDs
    @type permissionIDArray: list [int]
    @param permissionValueArray: list of permission values, in order of the permissions in permissionIDArray
    @type permissionValueArray: list [int]
    @param permissionSkipArray: list of permission skip values, in order of the permissions in permissionIDArray
    @type permissionSkipArray: list [int]
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientDBIDfromUID(serverConnectionHandlerID, clientUniqueIdentifier, returnCode):
    """
    Requests the database ID of a client defined by the UID. The event onClientDBIDfromUIDEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientUniqueIdentifier: the UID of the client
    @type clientUniqueIdentifier: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientDelPerm(serverConnectionHandlerID, clientDatabaseID, permissionIDArray, returnCode):
    """
    Deletes a list of permissions from a user.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientDatabaseID: the database ID of the user
    @type clientDatabaseID: int
    @param permissionIDArray: a list of permission IDs
    @type permissionIDArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientEditDescription(serverConnectionHandlerID, clientID, clientDescription, returnCode):
    """
    Sets the description of a client.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @param clientDescription: the description to set
    @type clientDescription: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientIDs(serverConnectionHandlerID, clientUniqueIdentifier, returnCode):
    """
    Requests the client IDs for a given UID. Will trigger the event onClientIDsEvent.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientUniqueIdentifier: the UID of the client
    @type clientUniqueIdentifier: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientKickFromChannel(serverConnectionHandlerID, clientID, kickReason, returnCode):
    """
    Kicks a client from its current channel to the default one.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client to kick
    @type clientID: int
    @param kickReason: the reason for the kick
    @type kickReason: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientKickFromServer(serverConnectionHandlerID, clientID, kickReason, returnCode):
    """
    Kicks a client from the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client to kick
    @type clientID: int
    @param kickReason: the reason for the kick
    @type kickReason: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientMove(serverConnectionHandlerID, clientID, newChannelID, password, returnCode):
    """
    Moves a client to a different channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client to be moved
    @type clientID: int
    @param newChannelID: the ID of the channel moving the client to
    @type newChannelID: int
    @param password: password of the channel, leave empty if channel is not password protected
    @type password: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientNamefromDBID(serverConnectionHandlerID, clientDatabaseID, returnCode):
    """
    Requests the name of a client defined by the database ID. The event onClientNamefromDBIDEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientDatabaseID: the database ID of the client
    @type clientDatabaseID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientNamefromUID(serverConnectionHandlerID, clientUniqueIdentifier, returnCode):
    """
    Requests the name of a client defined by the UID. The event onClientNamefromUIDEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientUniqueIdentifier: the UID of the client
    @type clientUniqueIdentifier: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientPermList(serverConnectionHandlerID, clientDatabaseID, returnCode):
    """
    Requests the list of permissions assigned to a user. The events
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientDatabaseID: the database ID of the user
    @type clientDatabaseID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientPoke(serverConnectionHandlerID, clientID, message, returnCode):
    """
    Pokes a client with a given message.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @param message: the message
    @type message: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientSetIsTalker(serverConnectionHandlerID, clientID, isTalker, returnCode):
    """
    Grants or revokes the talker flag of a client.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @param isTalker: if set to 1 (or True) grants talker flag, if 0 (or False) revokes talker flag
    @type isTalker: int or bool
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientSetWhisperList(serverConnectionHandlerID, clientID, targetChannelIDArray, targetClientIDArray,
                                returnCode):
    """
    Modifies the whisper list of a client.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client whose whisperlist is modified. If set to 0, the own whisper list is modified
    @type clientID: int
    @param targetChannelIDArray: a list of channel IDs the client will whisper to. Pass None to clear it
    @type targetChannelIDArray: list [int]
    @param targetClientIDArray: a list of client IDs the client will whisper to. Pass None to clear it
    @type targetClientIDArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestClientVariables(serverConnectionHandlerID, clientID, returnCode):
    """
    Requests latest data for a given client. The event onUpdateClientEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestComplainAdd(serverConnectionHandlerID, targetClientDatabaseID, complainReason, returnCode):
    """
    Adds a complain to a user defined by his database ID.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param targetClientDatabaseID: the database ID of the user
    @type targetClientDatabaseID: int
    @param complainReason: the reason for the complain
    @type complainReason: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestComplainDel(serverConnectionHandlerID, targetClientDatabaseID, fromClientDatabaseID, returnCode):
    """
    Deletes a complain to a user by a different user.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param targetClientDatabaseID: the database ID of the complained user
    @type targetClientDatabaseID: int
    @param fromClientDatabaseID: the database ID of the complaining user
    @type fromClientDatabaseID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestComplainDelAll(serverConnectionHandlerID, targetClientDatabaseID, returnCode):
    """
    Deletes all complains to a user.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param targetClientDatabaseID: the database ID of the user
    @type targetClientDatabaseID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestComplainList(serverConnectionHandlerID, targetClientDatabaseID, returnCode):
    """
    Requests the list of complains to a user. The event onComplainListEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param targetClientDatabaseID: the database ID of the user
    @type targetClientDatabaseID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestConnectionInfo(serverConnectionHandlerID, clientID, returnCode):
    """
    Requests the connection info of a client. The event onConnectionInfoEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the ID of the client
    @type clientID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestCreateDirectory(serverConnectionHandlerID, channelID, channelPW, directoryPath, returnCode):
    """
    Creates a directory in a channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param channelPW: the password of the channel, pass an empty string if channel is not password protected
    @type channelPW: string
    @param directoryPath: the complete path of the to be created directory
    @type directoryPath: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestDeleteFile(serverConnectionHandlerID, channelID, channelPW, files, returnCode):
    """
    Deletes a list of files in a channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param channelPW: the password of the channel, pass an empty string if channel is not password protected
    @type channelPW: string
    @param files: a list of complete pathes of the file to delete
    @type files: list [string]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestFile(serverConnectionHandlerID, channelID, channelPW, file, overwrite, resume, destinationDirectory,
                returnCode):
    """
    Starts a filedownload from the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel in which the file is placed in
    @type channelID: int
    @param channelPW: the password of the channel, pass an empty string if the channel is not password protected
    @type channelPW: string
    @param file: the complete path to the file in the channel
    @type file: string
    @param overwrite: if set to 1 (or True) and a file with that name already exists will be overwritten
    @type overwrite: int or bool
    @param resume: if set to 1 (or True), a previously started filetransfer can be resumed
    @type resume: int or bool
    @param destinationDirectory: the path to the directory, where the downloaded fill will be placed in
    @type destinationDirectory: strin
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: a tuple, containing the errorcode and the ID of the filetransfer
    @rtype: tuple (int, int)
    """
    return 0,0


def requestFileInfo(serverConnectionHandlerID, channelID, channelPW, file, returnCode):
    """
    Requests the info to a file in a channel. The event onFileInfoEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param channelPW: the password of the channel, pass an empty string if the channel is not password protected
    @type channelPW: string
    @param file: the complete path to the file
    @type file: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestFileList(serverConnectionHandlerID, channelID, channelPW, path, returnCode):
    """
    Requests the filelist of a channel. The events onFileListEvent and onFileListFinishedEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param channelPW: the password of the channel, pass an empty string if the channel is not password protected
    @type channelPW: string
    @param path: the path of the directory to be listed, pass '/' for the root path
    @type path: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestHotkeyInputDialog(keyword, isDown, qParentWindow):
    """
    Shows the hotkeyinputdialog to set the hotkey for a hotkey keyword. When finished ts3plugin.onHotkeyRecordedEvent will be called with the recorded hotkey.
    @param keyword: the global keyword (see PluginHost.globalHotkeyKeyword)
    @type keyword: str
    @param isDown: if True, the hotkey will be triggered on keypress, on keyrelease otherwise
    @type isDown: bool
    @param qParentWindow: the window on which the dialog is shown modal to, optional
    @type qParentWindow: QWidget
    """
    pass


def requestInfoUpdate(serverConnectionHandlerID, itemType, itemID):
    """
    Requests to update the info data.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param itemType: specifies, which info data update is requested, see ts3defines.PluginItemType
    @type itemType: int
    @param itemID: the ID of the item to update (only usefull if itemType != ts3defines.PluginItemType.PLUGIN_MENU_TYPE_GLOBAL)
    @type itemID: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestIsTalker(serverConnectionHandlerID, isTalkerRequest, isTalkerRequestMessage, returnCode):
    """
    Requests talk power or revokes the talk power request.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param isTalkerRequest: if set to 1 (or True) requests talk power, if 0 (or False) revokes the talk power request
    @type isTalkerRequest: int or bool
    @param isTalkerRequestMessage: the message of the request
    @type isTalkerRequestMessage: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestMessageAdd(serverConnectionHandlerID, toClientUID, subject, message, returnCode):
    """
    Sends an offline message to another user.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param toClientUID: the UID of the user
    @type toClientUID: string
    @param subject: the subject of the message
    @type subject: string
    @param message: the message
    @type message: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestMessageDel(serverConnectionHandlerID, messageID, returnCode):
    """
    Deletes an offline message.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param messageID: the ID of the message
    @type messageID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestMessageGet(serverConnectionHandlerID, messageID, returnCode):
    """
    Requests an offline message defined by its ID. The event onMessageGetEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param messageID: the ID of the message
    @type messageID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestMessageList(serverConnectionHandlerID, returnCode):
    """
    Requests the list of offline messages. The event onMessageListEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestMessageUpdateFlag(serverConnectionHandlerID, messageID, flag, returnCode):
    """
    Sets the message read/unread flag of an offline message
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param messageID: the ID of the message
    @type messageID: int
    @param flag: set to 0 to set message as unread, set to 1 to set message as read
    @type flag:
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestMuteClients(serverConnectionHandlerID, clientIDArray, returnCode):
    """
    Mutes a list of clients.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientIDArray: a list of client IDs
    @type clientIDArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestPermissionList(serverConnectionHandlerID, returnCode):
    """
    Requests the list of permissions available on the server. The events onPermissionListEvent and onPermissionListFinishedEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestPermissionOverview(serverConnectionHandlerID, clientDBID, channelID, returnCode):
    """
    Requests the permission overview of a user in a channel. The events onPermissionOverviewEvent and onPermissionOverviewFinishedEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientDBID: the database ID of the user
    @type clientDBID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestRenameFile(serverConnectionHandlerID, fromChannelID, channelPW, toChannelID, toChannelPW, oldFile, newFile,
                      returnCode):
    """
    Renames a file or moves it to another channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param fromChannelID: the ID of the channel, the file is currently placed in
    @type fromChannelID: int
    @param channelPW: the password of the channel, the file is currently placed in, pass an empty string if channel is not password protected
    @type channelPW: string
    @param toChannelID: the ID of the channel, the file should be placed in after, pass 0, if just renaming, not moving
    @type toChannelID: int
    @param toChannelPW: the password of the channel, to which the file should move to, pass an empty string if channel is not password protected; this is ignored, if just renaming, not moving
    @type toChannelPW: string
    @param oldFile: the complete path to the file
    @type oldFile: string
    @param newFile: the complete path to the new filename
    @type newFile: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestSendChannelTextMsg(serverConnectionHandlerID, message, targetChannelID, returnCode):
    """
    Sends a text message to all clients in a channel.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param message: the message to send
    @type message: string
    @param targetChannelID: the ID of the channel
    @type targetChannelID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestSendClientQueryCommand(serverConnectionHandlerID, command, returnCode):
    """
    Requests to execute a clientquery command.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param command: the command to execute
    @type command: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestSendPrivateTextMsg(serverConnectionHandlerID, message, targetClientID, returnCode):
    """
    Sends a private text message to a client.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param message: the message to send
    @type message: string
    @param targetClientID: the ID of the client to send the message to
    @type targetClientID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestSendServerTextMsg(serverConnectionHandlerID, message, returnCode):
    """
    Sends a text message to all clients on the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param message: the message to send
    @type message: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerGroupAdd(serverConnectionHandlerID, groupName, groupType, returnCode):
    """
    Adds a servergroup.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param groupName: the name of the group to create
    @type groupName: string
    @param groupType: type of the servergroup, see ts3defines.GroupType
    @type groupType: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerGroupAddClient(serverConnectionHandlerID, serverGroupID, clientDatabaseID, returnCode):
    """
    Adds a user defined by his database ID to a servergroup.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param serverGroupID: the ID of the servergroup
    @type serverGroupID: int
    @param clientDatabaseID: the database ID of the user
    @type clientDatabaseID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerGroupAddPerm(serverConnectionHandlerID, serverGroupID, continueonerror, permissionIDArray,
                              permissionValueArray, permissionNegatedArray, permissionSkipArray, returnCode):
    """
    Adds a list of permissions to a servergroup.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param serverGroupID: the ID of the servergroup
    @type serverGroupID: int
    @param continueonerror: if set to 1 (or True), if an error with a permission occurs, the other permissions will even though be handled
    @type continueonerror: int or bool
    @param permissionIDArray: list of permission IDs
    @type permissionIDArray: list [int]
    @param permissionValueArray: list of permission values, in order of the permissions in permissionIDArray
    @type permissionValueArray: list [int]
    @param permissionNegatedArray: list of permission negated values, in order of the permissions in permissionIDArray
    @type permissionNegatedArray: list [int]
    @param permissionSkipArray: list of permission skip values, in order of the permissions in permissionIDArray
    @type permissionSkipArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerGroupClientList(serverConnectionHandlerID, serverGroupID, withNames, returnCode):
    """
    Requests the list of clients assigned to a servergroup. The event onServerGroupClientListEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param serverGroupID: the ID of the servergroup
    @type serverGroupID: int
    @param withNames: if set to 1 (or True), the event will contain the nick and uid of the user instead of empty strings
    @type withNames: int or bool
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerGroupDel(serverConnectionHandlerID, serverGroupID, force, returnCode):
    """
    Deletes a servergroup.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param serverGroupID: the ID of the servergroup
    @type serverGroupID: int
    @param force: if set to 1 (or True), even if there are users assigned to this servergroup, it will be deleted
    @type force: int or bool
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerGroupDelClient(serverConnectionHandlerID, serverGroupID, clientDatabaseID, returnCode):
    """
    Deletes a user defined by his database ID from a servergroup.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param serverGroupID: the ID of the servergroup
    @type serverGroupID: int
    @param clientDatabaseID: the database ID of the user
    @type clientDatabaseID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerGroupDelPerm(serverConnectionHandlerID, serverGroupID, continueOnError, permissionIDArray, returnCode):
    """
    Deletes a list of permissions from a servergroup.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param serverGroupID: the ID of the servergroup
    @type serverGroupID: int
    @param continueOnError: if set to 1 (or True), if an error with a permission occurs, the other permissions will even though be handled
    @type continueOnError: int or bool
    @param permissionIDArray: list of permission IDs
    @type permissionIDArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerGroupList(serverConnectionHandlerID, returnCode):
    """
    Requests the list of servergroups. The events onServerGroupListEvent and onServerGroupListFinishedEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerGroupPermList(serverConnectionHandlerID, serverGroupID, returnCode):
    """
    Requests the list of permissions assigned to a servergroup. The events onServerGroupPermListEvent and onServerGroupPermListFinishedEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param serverGroupID: the ID of the servergroup
    @type serverGroupID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerGroupsByClientID(serverConnectionHandlerID, clientDatabaseID, returnCode):
    """
    Requests all servergroups of a user defined by his database ID. The event onServerGroupByClientIDEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientDatabaseID: the database ID of the user
    @type clientDatabaseID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerTemporaryPasswordAdd(serverConnectionHandlerID, password, description, duration, targetChannelID,
                                      targetChannelPW, returnCode):
    """
    Adds a temporary password to the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param password: the temporary password
    @type password: string
    @param description: the description of the temporary password
    @type description: string
    @param duration: the duration in seconds
    @type duration: int
    @param targetChannelID: the ID of the channel to which the accessing clients will join by default
    @type targetChannelID: int
    @param targetChannelPW: the password of the targetChannel, pass an empty string, if the channel is not password protected
    @type targetChannelPW: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerTemporaryPasswordDel(serverConnectionHandlerID, password, returnCode):
    """
    Deletes an existing temporary password.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param password: the password to delete
    @type password: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerTemporaryPasswordList(serverConnectionHandlerID, returnCode):
    """
    Requests a list of existing temporary passwords. The event onServerTemporaryPasswordListEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestServerVariables(serverConnectionHandlerID):
    """
    Requests all server variables of a serverconnection. The event onServerUpdatedEvent will be triggered.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestSetClientChannelGroup(serverConnectionHandlerID, channelGroupIDArray, channelIDArray, clientDatabaseIDArray,
                                 returnCode):
    """
    Adds a list of users to a list of channelgroups.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelGroupIDArray: a list of channelgroup IDs
    @type channelGroupIDArray: list [int]
    @param channelIDArray: a list of channel IDs
    @type channelIDArray: list [int]
    @param clientDatabaseIDArray: a list of client database IDs
    @type clientDatabaseIDArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def requestUnmuteClients(serverConnectionHandlerID, clientIDArray, returnCode):
    """
    Unmutes a list of clients.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientIDArray: a list of client IDs
    @type clientIDArray: list [int]
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def sendFile(serverConnectionHandlerID, channelID, channelPW, file, overwrite, resume, sourceDirectory, returnCode):
    """
    Starts a fileupload to the server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel in which the file will be placed in
    @type channelID: int
    @param channelPW: the password of the channel, pass an empty string if the channel is not password protected
    @type channelPW: string
    @param file: the complete path to the file in the channel
    @type file: string
    @param overwrite: if set to 1 (or True) and a file with that name already exists will be overwritten
    @type overwrite: int or bool
    @param resume: if set to 1 (or True), a previously started filetransfer can be resumed
    @type resume: int or bool
    @param sourceDirectory: the directory on the system, where the original file is placed in
    @type sourceDirectory: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: a tuple, containing the errorcode and the ID of the filetransfer
    @rtype: tuple (int, int)
    """
    return 0,0


def sendPluginCommand(serverConnectionHandlerID, command, targetMode, targetIDs, returnCode):
    """
    Sends a plugin command to other users.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param command: the command string
    @type command: string
    @param targetMode: specifies, to whom the command will be send, see ts3defines.PluginTargetMode
    @type targetMode: int
    @param targetIDs: a list of client IDs, only needed if targetMode == ts3defines.PluginTargetMode.PluginCommandTarget_CLIENT
    @type targetIDs: list [int]
    @return: the errorcode (as this is wrapped in python, there could be errors)
    @rtype: int
    """
    return 0


def serverPropertyStringToFlag(serverPropertyString):
    """
    Converts a server property name used in strings (eg the serverquery) to the corresponding flag.
    @param serverPropertyString: the lowercase string representation
    @type serverPropertyString: str
    @return: a tuple, containing the errorcode and the flag (see ts3defines.ClientProperties and ts3defines.ClientPropertiesRare)
    @rtype: tuple (int, int)
    """
    return 0,0


def set3DWaveAttributes(serverConnectionHandlerID, waveHandle, position):
    """
    Positions a wave file that was opened previously with playWaveFileHandle in 3D space.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param waveHandle: handle of the played wavefile sound returned by playWaveFileHandle
    @type waveHandle: int
    @param position: A tuple defining the 3D position of the sound
    @type position: tuple (float, float, float)
    @return: the errorcode
    @rtype: int
    """
    return 0


def setChannelVariableAsInt(serverConnectionHandlerID, channelID, flag, value):
    """
    Sets a channel variable to a new int value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel, pass 0 to set a new channel's variables
    @type channelID: int
    @param flag: the flag to set
    @type flag: int
    @param value: the new value
    @type value: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def setChannelVariableAsString(serverConnectionHandlerID, channelID, flag, value):
    """
    Sets a channel variable to a new string value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel, pass 0 to set a new channel's variables
    @type channelID: int
    @param flag: the flag to set
    @type flag: int
    @param value: the new value
    @type value: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def setChannelVariableAsUInt64(serverConnectionHandlerID, channelID, flag, value):
    """
    Sets a channel variable to a new unsigned long long int value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel, pass 0 to set a new channel's variables
    @type channelID: int
    @param flag: the flag to set
    @type flag: int
    @param value: the new value
    @type value: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def setClientSelfVariableAsInt(serverConnectionHandlerID, flag, value):
    """
    Sets a variable of the own client to a new int value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param flag: the flag to set
    @type flag: int
    @param value: the new value
    @type value: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def setClientSelfVariableAsString(serverConnectionHandlerID, flag, value):
    """
    Sets a variable of the own client to a new string value.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param flag: the flag to set
    @type flag: int
    @param value: the new value
    @type value: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def setClientVolumeModifier(serverConnectionHandlerID, clientID, value):
    """
    Sets the volume modifier of a client.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param clientID: the client's ID
    @type clientID: int
    @param value: the value to set
    @type value: float
    @return: the errorcode
    @rtype: int
    """
    return 0


def setPlaybackConfigValue(serverConnectionHandlerID, ident, value):
    """
    Sets a playback option.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param ident: the playback option to reset
    @type ident: string
    @param value: the value to set
    @type value: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def setPluginMenuEnabled(menuID, enabled):
    """
    Enables or disables a menuitem. The menuID must be the global id, not the local id plugin developers set in menuItems. Retrieve it with PluginHost.globalMenuID.
    @param menuID: global id of the menuitem
    @type menuID: int
    @param enabled: set to True to enable it, False otherwise
    @type enabled: bool
    """
    pass


def setPreProcessorConfigValue(serverConnectionHandlerID, ident, value):
    """
    Sets a sound preprocessor flag.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param ident: the flag to be set
    @type ident: string
    @param value: the value to set the flag to
    @type value: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def showHotkeySetup():
    """
    Opens the hotkey settings in the TeamSpeak 3 client's settings dialog.
    """
    pass


def spawnNewServerConnectionHandler(port):
    """
    Creates a new server connection handler and receive its ID.
    @param port: Port the client should bind on. Specify zero to let the operating system chose any free port
    @type port: int
    @return: A tuple, containig the errorcode and the resulting ID
    @rtype: tuple (int, int)
    """
    return 0,0


def startConnection(serverConnectionHandlerID, identity, ip, port, nickname, defaultChannelArray,
                    defaultChannelPassword, serverPassword):
    """
    Starts a connection to the given server.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param identity: the client's identity
    @type identity: string
    @param ip: hostname or ip of the server
    @type ip: string
    @param port: port of the server
    @type port: int
    @param nickname: the client's nickname
    @type nickname: string
    @param defaultChannelArray: list of strings defining the path to a channel on the server, pass empty list to join in server's default channel
    @type defaultChannelArray: list [string]
    @param defaultChannelPassword: password of the default channel, pass an empty string if not using defaultChannelArray or channel is not password protected
    @type defaultChannelPassword: string
    @param serverPassword: password of the server, pass an empty string if the server is not password protected
    @type serverPassword: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def startVoiceRecording(serverConnectionHandlerID):
    """
    Sets the voice recording flag of the own client on a serverconnection.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def stopConnection(serverConnectionHandlerID, quitMessage):
    """
    Stops the connection of a serverconnection.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param quitMessage: a message displayed when leaving the server encoded in UTF-8
    @type quitMessage: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def stopVoiceRecording(serverConnectionHandlerID):
    """
    Removes the voice recording flag of the own client on a serverconnection
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @return: the errorcode
    @rtype: int
    """
    return 0


def systemset3DListenerAttributes(serverConnectionHandlerID, position, forward, up):
    """
    Sets the position, velocity and orientation of the own client in 3D space
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param position: A tuple defining the 3D position, pass None to ignore
    @type position: tuple (float, float, float)
    @param forward: A tuple defining the forward orientation of the listener. The vector must be of unit length and perpendicular to the up vector. Pass None to ignore.
    @type forward: tuple (float, float, float)
    @param up: A tuple defining the upward orientation of the listener. The vector must be of unit length and perpendicular to the forward vector. Pass None to ignore.
    @type up: tuple (float, float, float)
    @return: the errorcode
    @rtype: int
    """
    return 0


def systemset3DSettings(serverConnectionHandlerID, distanceFactor, rolloffScale):
    """
    Adjust 3D sound system settings.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param distanceFactor: relative distance factor. Default is 1.0 = 1 meter
    @type distanceFactor: float
    @param rolloffScale: Scaling factor for 3D sound rolloff. Defines how fast sound volume will attenuate. As higher the value, as faster the sound is toned with increasing distance.
    @type rolloffScale: float
    @return: the errorcode
    @rtype: int
    """
    return 0


def unregisterCustomDevice(deviceID):
    """
    Unregisters a custom device, previously registered with registerCustomDevice.
    @param deviceID: the ID of the device, used in registerCustomDevice
    @type deviceID: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def urlsToBB(text, maxLen=256):
    """
    Converts an url to the BB-code respresentation.
    @param text: the url
    @type text: string
    @param maxLen: length of the buffer, passed to the clientlib to store the path to, default value is 256
    @type maxLen: int
    @return: the BB-code representation
    @rtype: string
    """
    pass


def verifyChannelPassword(serverConnectionHandlerID, channelID, channelPassword, returnCode):
    """
    Verifies the password to a channel. The result can be checked in onServerErrorEvent.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param channelID: the ID of the channel
    @type channelID: int
    @param channelPassword: the password to be verified
    @type channelPassword: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


def verifyServerPassword(serverConnectionHandlerID, serverPassword, returnCode):
    """
    Verifies the password to a server. The result can be checked in onServerErrorEvent.
    @param serverConnectionHandlerID: the ID of the serverconnection
    @type serverConnectionHandlerID: int
    @param serverPassword: the password to be verified
    @type serverPassword: string
    @param returnCode: returnCode passed to onServerErrorEvent or onServerPermissionErrorEvent. Optional.
    @type returnCode: string
    @return: the errorcode
    @rtype: int
    """
    return 0


for func in dir(_ts3lib):
    if not func.startswith("__"):
        # if func not in globals():
        globals()[func] = getattr(_ts3lib, func)

def sendPluginCommand(schid, command, targetMode, targetIDs, *returnCode):
    _PluginCommandHandler.sendPluginCommand(schid, command, targetMode,
                                            targetIDs, *returnCode)
