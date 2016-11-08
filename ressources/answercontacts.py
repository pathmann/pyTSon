from ts3plugin import ts3plugin

import ts3, ts3defines

from PythonQt.QtSql import QSqlDatabase

class answercontacts(ts3plugin):
    name = "answercontacts"
    requestAutoload = False
    version = "1.0"
    apiVersion = 21
    author = "Thomas \"PLuS\" Pathmann"
    description = "Autoanswer contact status (friend, neutral, blocked)"
    offersConfigure = False
    commandKeyword = ""
    infoTitle = None
    menuItems = []
    hotkeys = []

    def __init__(self):
        pass

    def contactStatus(self, uid):
        """
        checks contact status of a given uid. Returns friend=0, blocked=1, neutral=2
        """
        db = QSqlDatabase.addDatabase("QSQLITE","pyTSon_example")
        db.setDatabaseName(ts3.getConfigPath() + "settings.db")

        if not db.isValid():
            raise Exception("Database not valid")

        if not db.open():
            raise Exception("Database could not be opened")

        q = db.exec_("SELECT * FROM contacts WHERE value LIKE '%%IDS=%s%%'" % uid)
        ret = 2

        if q.next():
            val = q.value("value")

            for l in val.split('\n'):
                if l.startswith('Friend='):
                    ret = int(l[-1])

        q.delete()
        db.close()
        db.delete()
        QSqlDatabase.removeDatabase("pyTSon_example")

        return ret

    def onTextMessageEvent(self, schid, targetMode, toID, fromID, fromName, fromUniqueIdentifier, message, ffIgnored):
        (err, myid) = ts3.getClientID(schid)

        if err == ts3defines.ERROR_ok:
            #only in private messages
            if toID == myid:
                f = self.contactStatus(fromUniqueIdentifier)
                if f == 0:
                    err = ts3.requestSendPrivateTextMsg(schid, "Hello, my friend!", fromID)
                elif f == 1:
                    err = ts3.requestSendPrivateTextMsg(schid, "I don't like you!", fromID)
                else:
                    err = ts3.requestSendPrivateTextMsg(schid, "Do I know you?", fromID)

