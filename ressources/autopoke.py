from ts3plugin import ts3plugin

import ts3, ts3defines

class autopoke(ts3plugin):
    name = "autopokeback"
    requestAutoload = False
    version = "1.0"
    apiVersion = 21
    author = "Thomas \"PLuS\" Pathmann"
    description = "Ignore pokes and automatically poke back"
    offersConfigure = False
    commandKeyword = ""
    infoTitle = None
    menuItems = []
    hotkeys = []

    def __init__(self):
        self.lastpoker = (0, 0)

    def onTextMessageEvent(self, schid, targetMode, toID, fromID, fromName, fromUniqueIdentifier, message, ffIgnored):
        if (schid, fromID) == self.lastpoker and message == "Received your poke!":
            self.lastpoker = (0, 0)
            return 1
        else:
            return 0

    def onClientPokeEvent(self, serverConnectionHandlerID, fromClientID, pokerName, pokerUID, message, ffIgnored):
        #is it me?
        (err, schids) = ts3.getServerConnectionHandlerList()
        if err == ts3defines.ERROR_ok:
            for schid in schids:
                (err, myid) = ts3.getClientID(schid)
                if err == ts3defines.ERROR_ok:
                    if fromClientID == myid:
                        return 0
                else:
                    ts3.printMessageToCurrentTab("error getting own client id")
        else:
            ts3.printMessageToCurrentTab("error getting schids")

        (err, name) = ts3.getClientVariableAsString(serverConnectionHandlerID, fromClientID, ts3defines.ClientProperties.CLIENT_NICKNAME)
        if err != ts3defines.ERROR_ok:
            err = ts3.requestClientPoke(serverConnectionHandlerID, fromClientID, "Ne!")
        else:
            err = ts3.requestClientPoke(serverConnectionHandlerID, fromClientID, "%s? Ne!" % name)

        self.lastpoker = (serverConnectionHandlerID, fromClientID)

        if err != ts3defines.ERROR_ok:
            ts3.printMessageToCurrentTab("poke error: %d" % err)
            return 0
        else:
            return 1

