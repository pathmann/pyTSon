from ts3plugin import ts3plugin

import ts3, ts3defines

import time

class processcmdtest(ts3plugin):
    name = "processCommandTest"
    requestAutoload = False
    version = "1.0"
    apiVersion = 21
    author = "Thomas \"PLuS\" Pathmann"
    description = "This plugin shows how to use commands, run \"/py pt greet\" or \"/py pt time\""
    offersConfigure = False
    commandKeyword = "pt"
    infoTitle = None
    menuItems = []
    hotkeys = []

    def __init__(self):
        #plugin was started
        pass

    def stop(self):
        #plugin was stopped
        pass

    def processCommand(self, schid, command):
        tokens = command.split(' ')

        if len(tokens) == 0:
            return True

        (err, myid) = ts3.getClientID(schid)
        if err != ts3defines.ERROR_ok:
            ts3.printMessageToCurrentTab("Error getting client id: (%s, %s)" % (err, ts3.getErrorMessage(err)[1]))
            return True

        (err, cid) = ts3.getChannelOfClient(schid, myid)
        if err != ts3defines.ERROR_ok:
            ts3.printMessageToCurrentTab("Error getting channel id: (%s, %s)" % (err, ts3.getErrorMessage(err)[1]))
            return True

        if tokens[0] == "time":
            err = ts3.requestSendChannelTextMsg(schid, "It's %s" % time.asctime(time.localtime(time.time())), cid)
            if err != ts3defines.ERROR_ok:
                ts3.printMessageToCurrentTab("Error sending txt message: (%s, %s)" % (err, ts3.getErrorMessage(err)[1]))
            return True
        elif tokens[0] == "greet":
            (err, svname) = ts3.getServerVariableAsString(schid, ts3defines.VirtualServerProperties.VIRTUALSERVER_NAME)
            if err != ts3defines.ERROR_ok:
                ts3.printMessageToCurrentTab("Error getting server name: (%s, %s)" % (err, ts3.getErrorMessage(err)[1]))
                return True

            (err, cname) = ts3.getChannelVariableAsString(schid, cid, ts3defines.ChannelProperties.CHANNEL_NAME)
            if err != ts3defines.ERROR_ok:
                ts3.printMessageToCurrentTab("Error getting channel name: (%s, %s)" % (err, ts3.getErrorMessage(err)[1]))
                return True

            err = ts3.requestSendChannelTextMsg(schid, "Welcome in %s on %s" % (cname, svname), cid)
            if err != ts3defines.ERROR_ok:
                ts3.printMessageToCurrentTab("Error sending txt message: (%s, %s)" % (err, ts3.getErrorMessage(err)[1]))
            return True

