from ts3plugin import ts3plugin

import ts3
from ts3defines import ERROR_ok, ClientPropertiesRare, TextMessageTargetMode

from PythonQt.QtGui import *
from PythonQt.QtCore import Qt
from PythonQt.QtSql import QSqlDatabase

import os
from configparser import ConfigParser


import traceback
ct = ts3.printMessageToCurrentTab

CONF_DEFAULTS = {'onlyfriends': 'True', 'sendaway': 'False', 'custommsg': '', 'onlyonce': 'True'}

class AutoReplyConfig(QDialog):
    def __init__(self, cfg, parent=None):
        super(QDialog, self).__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)

        self.lay = QFormLayout(self)

        self.friendscb = QCheckBox("Only to friends", self)
        self.lay.addRow(self.friendscb)

        self.msggrp = QGroupBox("Message", self)
        self.lay.addRow(self.msggrp)
        self.grplayout = QFormLayout(self.msggrp)

        self.awaymsgradio = QRadioButton("Send away msg", self)
        self.grplayout.addRow(self.awaymsgradio)

        self.custommsgradio = QRadioButton("Send custom msg", self)
        self.grplayout.addRow(self.custommsgradio)

        self.custommsgedit = QLineEdit(self)
        self.grplayout.addRow(self.custommsgedit)

        self.onlyoncecb = QCheckBox("Only once per conversation", self)
        self.lay.addRow(self.onlyoncecb)

        self.buttonbox = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel, self)
        self.lay.addRow(self.buttonbox)

        self.connect("finished(int)", self.onClose)
        self.awaymsgradio.connect("toggled(bool)", self.onRadioChanged)
        self.custommsgradio.connect("toggled(bool)", self.onRadioChanged)
        self.buttonbox.connect("accepted()", self.accept)
        self.buttonbox.connect("rejected()", self.reject)

        self.cfg = cfg['general']
        self.initValues()

    def initValues(self):
        self.friendscb.setChecked(self.cfg['onlyfriends'] == "True")
        if self.cfg['sendaway'] == "True":
            self.awaymsgradio.setChecked(True)
        else:
            self.custommsgradio.setChecked(True)
        self.custommsgedit.setText(self.cfg['custommsg'])
        self.onlyoncecb.setChecked(self.cfg['onlyonce'] == "True")

    def onClose(self, r):
        if r == QDialog.Accepted:
            self.cfg['onlyfriends'] = "True" if self.friendscb.isChecked() else "False"
            self.cfg['sendaway'] = "True" if self.awaymsgradio.isChecked() else "False"
            self.cfg['custommsg'] = self.custommsgedit.text
            self.cfg['onlyonce'] = "True" if self.onlyoncecb.isChecked() else "False"

    def onRadioChanged(self):
        self.custommsgedit.setEnabled(self.custommsgradio.isChecked())

def isFriend(uid):
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

    return ret == 0


class autoreply(ts3plugin):
    name = "AutoReplyAway"
    requestAutoload = False
    version = "1.0"
    apiVersion = 21
    author = "Thomas \"PLuS\" Pathmann"
    description = "This plugin autoreplies if you are away. It also shows one way to create ui-configurable scripts."
    offersConfigure = True
    commandKeyword = ""
    infoTitle = None
    menuItems = []
    hotkeys = []

    def __init__(self):
        try:
            self.dlg = None
            self.handled = []

            self.cfg = ConfigParser(CONF_DEFAULTS)
            self.cfg.add_section("general")

            p = os.path.join(ts3.getConfigPath(), "pyTSon_autoreply.ini")
            if os.path.isfile(p):
                self.cfg.read(p)
        except:
            ct(traceback.format_exc())

    def stop(self):
        if self.dlg:
            self.dlg.close()
        self.dlg = None

        with open(os.path.join(ts3.getConfigPath(), "pyTSon_autoreply.ini"), "w") as f:
            self.cfg.write(f)

    def configure(self, qParentWidget):
        try:
            if not self.dlg:
                self.dlg = AutoReplyConfig(self.cfg, qParentWidget)

            self.dlg.show()
            self.dlg.raise_()
        except:
            ct(traceback.format_exc())

    def onTextMessageEvent(self, schid, targetMode, toID, fromID, fromName, fromUID, message, ffIgnored):
        (err, myid) = ts3.getClientID(schid)
        if err != ERROR_ok or fromID == myid:
            return False

        #only private msgs
        if targetMode != TextMessageTargetMode.TextMessageTarget_CLIENT:
            return False

        #only, if I'm away
        (err, away) = ts3.getClientSelfVariableAsInt(schid, ClientPropertiesRare.CLIENT_AWAY)
        if err != ERROR_ok or not away:
            return False

        #only to friends? is from friend?
        if self.cfg['general']['onlyfriends'] == "True" and not isFriend(fromUID):
            return False

        #only once per conversation? did we already sent him?
        if self.cfg['general']['onlyonce'] == "True" and (schid, fromUID) in self.handled:
            return False

        if self.cfg['general']['sendaway'] == "True":
            (err, msg) = ts3.getClientSelfVariableAsString(schid, ClientPropertiesRare.CLIENT_AWAY_MESSAGE)
        else:
            msg = self.cfg['general']['custommsg']

        if msg == "":
            return False

        err = ts3.requestSendPrivateTextMsg(schid, msg, fromID)
        if err == ERROR_ok:
            self.handled.append((schid, fromUID))

    def onClientChatClosedEvent(self, schid, clientID, clientUID):
        if (schid, clientUID) in self.handled:
            self.handled.remove((schid, clientUID))

