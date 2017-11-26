import _ts3lib
import ts3defines
from ts3defines import (ClientProperties, ClientPropertiesRare,
                        ChannelProperties, ChannelPropertiesRare,
                        VirtualServerProperties, VirtualServerPropertiesRare)

from _plugincmd import _PluginCommandHandler

def logMessage(logMessage, severity, channel, logID):
    return _ts3lib.logMessage(logMessage, severity.value, channel, logID)

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


"""
To be done (?):
def getConnectionVariable(schid, clientID, flag):
    pass
"""


def sendPluginCommand(schid, command, targetMode, targetIDs, *returnCode):
    _PluginCommandHandler.sendPluginCommand(schid, command, targetMode,
                                            targetIDs, *returnCode)


for func in dir(_ts3lib):
    if not func.startswith("__"):
        if func not in globals():
            globals()[func] = getattr(_ts3lib, func)
