import sys, os

from PythonQt.QtGui import *
from PythonQt.QtCore import Qt, QFile, QIODevice, QUrl
from PythonQt.QtUiTools import QUiLoader
from PythonQt.QtNetwork import QNetworkAccessManager, QNetworkRequest, QNetworkReply
from PythonQt import BoolResult

from rlcompleter import Completer
import traceback, re
from itertools import takewhile

import json
from tempfile import gettempdir

from zipfile import ZipFile
import io
import devtools

import ts3lib as ts3
import ts3defines, pytson, ts3client


def _ts3print(msg, level, channel, aid):
    err = ts3.logMessage(msg, level, channel, aid)
    if err != ts3defines.ERROR_ok:
        print(msg)


def setIcon(obj, iconpack):
    """
    Sets the icon of a QWidget (if it has a property Icon) to an icon in the iconpack represented by a variable which is acquired by the property 'pytsonicon' of the object. If the property instead contains a string formated as "octicons:filename.png", the icon is set to filename.png of the octicons pack.
    If no such property is available, nothing is done.
    @param obj: the widget
    @type obj: QWidget
    @param iconpack: the iconpack
    @type iconpack: ts3client.IconPack
    """
    if iconpack:
        if hasattr(obj, "setIcon") and hasattr(obj, "pytsonicon"):
            var = obj.pytsonicon

            if var.startswith("octicons:"):
                fname = os.path.join(pytson.getPluginPath("ressources", "octicons", var.split(":")[1]))
                if os.path.isfile(fname):
                    obj.setIcon(QIcon(fname))
            else:
                obj.setIcon(QIcon(iconpack.icon(var)))


def connectSignalSlotsByName(sender, receiver):
    """
    Connects pythonqt signals by name (receiver.on_<sender.objectname>_<signalname>)
    @param sender: the sender of signals
    @type sender: QObject
    @param receiver: the receiver which has slots as callables defined
    @type receiver: object
    """
    for sig in dir(sender):
        if type(getattr(sender, sig)).__name__ == "builtin_qt_signal":
            if hasattr(receiver, "on_%s_%s" % (sender.objectName, sig.split('(')[0])):
                getattr(sender, sig).connect(getattr(receiver, "on_%s_%s" % (sender.objectName, sig.split('(')[0])))


def retrieveWidgets(obj, parent, widgets, seticons=True, iconpack=None):
    """
    Retrieves widgets from a list and adds them as attribute to another object. If defined, signals from widgets are connected by name to methods in obj.
    @param obj: the object which will get the attributes added
    @type obj: object
    @param parent: the toplevel widget
    @type parent: QWidget
    @param widgets: a recursive (parent-relation of widgets) list of tuples, defining which widgets should be added as attributes to obj. The elements must be children of parent. First element of tuple must held the widget's objectname. If second element is True, the widget will be added as property (by objectname) to obj. Third element of the tuple are the child widgets, which should be handled by setupui
    @type widgets: list[tuple(str, bool, list(...))]
    @param seticons: if True, icons will be set according to the widgets 'pytsonicon' attribute
    @type seticons: bool
    @param iconpack: the iconpack
    @type iconpack: ts3client.IconPack
    """
    if type(parent) is QTabWidget:
        childs = [parent.widget(i) for i in range(0, parent.count)]
    else:
        childs = parent.children()

    root = False
    if seticons and not iconpack:
        try:
            iconpack = ts3client.IconPack.current()
            iconpack.open()
            root = True
        except Exception as e:
            iconpack = None
            seticons = False
            _ts3print("Error loading iconpack: %s" % e, ts3defines.LogLevel.LogLevel_ERROR, "pytsonui.retrieveWidgets.%s" % obj.objectName, 0)

    names = [w[0] for w in widgets]
    stores = [w[1] for w in widgets]
    grchilds = [w[2] for w in widgets]

    for c in childs:
        if c.objectName in names:
            i = names.index(c.objectName)
            if stores[i]:
                if hasattr(obj, names[i]):
                    raise Exception("Widget objectName %s is not unique" % names[i])
                setattr(obj, names[i], c)

            connectSignalSlotsByName(c, obj)
            setIcon(c, iconpack)

            retrieveWidgets(obj, c, grchilds[i], seticons, iconpack)

            names.pop(i)
            stores.pop(i)
            grchilds.pop(i)

        if len(names) == 0:
            if root:
                iconpack.close()
            return

    if root:
        iconpack.close()

    if len(names) != 0:
        raise Exception("Malformed uifile, widgets not found: %s" % names)


def retrieveAllWidgets(obj, parent, seticons=True, iconpack=None):
    """
    Retrieves all child widgets from a parent widget and adds them as attribute to another object. If defined, signals from widgets are connected by name to methods in obj.
    @param obj: the object which will get the attributes added
    @type obj: object
    @param parent: the toplevel widget
    @type parent: QWidget
    @param seticons: if True, icons will be set according to the widgets 'pytsonicon' attribute
    @type seticons: bool
    @param iconpack: the iconpack
    @type iconpack: ts3client.IconPack
    """
    root = False
    if seticons and not iconpack:
        try:
            iconpack = ts3client.IconPack.current()
            iconpack.open()
            root = True
        except Exception as e:
            iconpack = None
            seticons = False
            _ts3print("Error loading iconpack: %s" % e, ts3defines.LogLevel.LogLevel_ERROR, "pytsonui.retrieveAllWidgets.%s" % obj.objectName, 0)

    for c in parent.children():
        if c.isWidgetType() and c.objectName != "" and type(c) not in [QSplitterHandle] and c.objectName not in ["qt_scrollarea_viewport", "qt_scrollarea_hcontainer", "qt_scrollarea_vcontainer"]:
            if hasattr(obj, c.objectName):
                raise Exception("Widget objectName %s is not unique %s" % (c.objectName, type(c)))
            setattr(obj, c.objectName, c)

            connectSignalSlotsByName(c, obj)
            setIcon(c, iconpack)

            retrieveAllWidgets(obj, c, seticons, iconpack)

    if root:
        iconpack.close()


def setupUi(obj, uipath, *, widgets=None, seticons=True, iconpack=None):
    """
    Loads a Qt designer file (.ui), creates the widgets defined in and adds them as property to a given object. This internally calls retrieveWidgets, so signals from widgets are connected by name to obj.
    @param obj: The object which will act as parent of the loaded ui (this object will receive a new layout)
    @type obj: QWidget
    @param uipath: the path to the Qt designer file
    @type uipath: str
    @param widgets: optional argument; a recursive (parent-relation of widgets) list of tuples, defining which widgets should be added as attributes to obj. See retrieveWidgets for details. If you omit this or pass None, recursively all child widgets will be stored
    @type widgets: list[tuple(str, bool, list(...))] or None
    @param seticons: if True, widgets containing a string-property called 'pytsonicon' will get the icon of a soundpack (value of property = variable in soundpack)
    @type seticons: bool
    @param iconpack: if set, the iconpack will be used, if None, the current iconpack is used
    @type iconpack: ts3client.IconPack
    """
    root = False
    if seticons and not iconpack:
        try:
            iconpack = ts3client.IconPack.current()
            iconpack.open()
            root = True
        except Exception as e:
            iconpack = None
            seticons = False
            _ts3print("Error loading iconpack: %s" % e, ts3defines.LogLevel.LogLevel_ERROR, "pytsonui.retrieveWidgets.%s" % obj.objectName, 0)

    if os.path.isfile(uipath):
        f = QFile(uipath)
        if f.open(QIODevice.ReadOnly):
            loader = QUiLoader()
            ui = loader.load(f, obj)
            f.close()

            if not ui:
                raise Exception("Error creating widget from uifile")
        else:
            raise Exception("Could not open uifile")
    else:
        raise Exception("Could not find uifile")

    if widgets:
        retrieveWidgets(obj, ui, widgets, seticons, iconpack)
    else:
        retrieveAllWidgets(obj, ui, seticons, iconpack)

    if root:
        iconpack.close()

    layout = QHBoxLayout(obj)
    layout.addWidget(ui)
    obj.setLayout(layout)


class ConfigurationDialog(QDialog):
    #[(objectName, store, [children])]
    CONF_WIDGETS = [("tabWidget", False, [
                      ("pluginsTab", False, [
                          ("differentApiButton", True, []),
                          ("pluginsTable", True, []),
                          ("reloadButton", True, []),
                          ("repositoryButton", True, []),
                          ("versionEdit", True, []),
                          ("nameEdit", True, []),
                          ("authorEdit", True, []),
                          ("descriptionEdit", True, []),
                          ("keywordEdit", True, []),
                          ("apiEdit", True, []),
                          ("requiredApiEdit", True, [])
                      ]),
                      ("generalTab", False, [
                          ("loadMenusButton", True, []),
                      ]),
                      ("consoleTab", False, [
                          ("colorsBox", False, [
                              ("backgroundColorButton", True, []),
                              ("textColorButton", True, []),
                          ]),
                          ("fontBox", False, [
                              ("fontFamilyCombo", True, []),
                              ("fontSizeSpin", True, []),
                          ]),
                          ("tabsBox", False, [
                              ("tabcompleteButton", True, []),
                              ("spacesButton", True, []),
                              ("tabwidthSpin", True, []),
                              ("tabwidthLabel", True, []),
                          ]),
                          ("startupBox", False, [
                              ("scriptEdit", True, []),
                              ("scriptButton", True, []),
                              ("silentButton", True, []),
                          ]),
                      ]),
                   ])]

    def __init__(self, cfg, host, parent=None):
        super().__init__(parent)

        self.setAttribute(Qt.WA_DeleteOnClose)
        self.setWindowTitle("pyTSon - Settings")

        self.cfg = cfg
        self.host = host
        self.rpd = None

        try:
            setupUi(self, os.path.join(ts3.getPluginPath(), "pyTSon", "ressources", "pyTSon-configdialog.ui"), widgets=self.CONF_WIDGETS)
            self.pluginsTable.horizontalHeader().setSectionResizeMode(0, QHeaderView.Stretch)

            self.setupValues()
            self.setupSlots()
        except Exception as e:
            self.delete()
            raise e

    def setupList(self):
        self.pluginsTable.clear()
        self.pluginsTable.setRowCount(len(self.host.plugins))

        try:
            ico = ts3client.IconPack.current()
            ico.open()
        except Exception as e:
            ico = None
            _ts3print("Error loading iconpack: %s" % e, ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.ConfigurationDialog.setupList", 0)

        row = 0
        diffapi = self.cfg.getboolean("general", "differentApi", fallback=False)
        for key, p in self.host.plugins.items():
            if diffapi or p.apiVersion == 21:
                item = QTableWidgetItem(p.name)
                item.setFlags(Qt.ItemIsSelectable | Qt.ItemIsUserCheckable | Qt.ItemIsEnabled)
                item.setCheckState(Qt.Checked if key in self.host.active else Qt.Unchecked)
                item.setData(Qt.UserRole, key)
                self.pluginsTable.setItem(row, 0, item)

                if p.offersConfigure:
                    setbutton = QToolButton()
                    setbutton.connect("clicked()", lambda n=p.name: self.onSettingsButtonClicked(n))
                    setbutton.setToolTip("Configure")
                    if ico:
                        setbutton.setIcon(QIcon(ico.icon("SETTINGS")))
                    self.pluginsTable.setCellWidget(row, 1, setbutton)

                    if p.name not in self.host.active:
                        setbutton.setEnabled(False)

                rembutton = QToolButton()
                if ico:
                    rembutton.setIcon(QIcon(ico.icon("DELETE")))
                rembutton.connect("clicked()", lambda n=p.name: self.onRemoveButtonClicked(n))
                rembutton.setToolTip("Remove")
                self.pluginsTable.setCellWidget(row, 2, rembutton)

                row += 1

        if ico:
            ico.close()

        self.pluginsTable.setRowCount(row)
        self.pluginsTable.sortItems(0)

    def setupValues(self):
        self.differentApiButton.setChecked(Qt.Checked if self.cfg.getboolean("general", "differentApi", fallback=False) else Qt.Unchecked)
        self.requiredApiEdit.setText(21)

        self.loadMenusButton.setChecked(Qt.Checked if self.cfg.getboolean("general", "loadAllMenus", fallback=False) else Qt.Unchecked)

        self.backgroundColorButton.setStyleSheet("background-color: %s;" % QColor(self.cfg.get("console", "backgroundColor")).name())
        self.textColorButton.setStyleSheet("background-color: %s;" % QColor(self.cfg.get("console", "textColor")).name())

        f = QFont(self.cfg.get("console", "fontFamily"))
        self.fontFamilyCombo.setCurrentFont(f)
        self.fontSizeSpin.setValue(self.cfg.getint("console", "fontSize"))

        if self.cfg.getboolean("console", "tabcomplete"):
            self.tabcompleteButton.setChecked(True)
            self.spacesButton.setEnabled(False)
            self.tabwidthSpin.setEnabled(False)
            self.tabwidthLabel.setEnabled(False)
        else:
            self.tabcompleteButton.setChecked(False)

        self.spacesButton.setChecked(self.cfg.getboolean("console", "spaces"))
        self.tabwidthSpin.setValue(self.cfg.getint("console", "tabwidth"))

        self.scriptEdit.setText(self.cfg.get("console", "startup"))
        self.silentButton.setChecked(self.cfg.getboolean("console", "silentStartup"))

        self.setupList()

    def setupSlots(self):
        #you can of course connect signal-slots manually, scriptEdit.textEdited for example is connected by setupui
        self.differentApiButton.connect("stateChanged(int)", self.onDifferentApiButtonChanged)
        self.pluginsTable.connect("currentCellChanged(int, int, int, int)", self.onPluginsTableCurrentItemChanged)
        self.pluginsTable.connect("itemChanged(QTableWidgetItem*)", self.onPluginsTableItemChanged)
        self.reloadButton.connect("clicked()", self.onReloadButtonClicked)

        self.loadMenusButton.connect("stateChanged(int)", self.onLoadMenusButtonChanged)

        self.backgroundColorButton.connect("clicked()", self.onBackgroundColorButtonClicked)
        self.textColorButton.connect("clicked()", self.onTextColorButtonClicked)
        self.fontFamilyCombo.connect("currentFontChanged(QFont)", self.onFontFamilyComboChanged)
        self.fontSizeSpin.connect("valueChanged(int)", self.onFontSizeSpinChanged)
        self.tabcompleteButton.connect("stateChanged(int)", self.onTabcompleteButtonChanged)
        self.spacesButton.connect("stateChanged(int)", self.onSpacesButtonChanged)
        self.tabwidthSpin.connect("valueChanged(int)", self.onTabwidthSpinChanged)

    def onLoadMenusButtonChanged(self, state):
        self.cfg.set("general", "loadAllMenus", "True" if state == Qt.Checked else "False")
        QMessageBox.information(self, "Restart required", "Changes only take effect after a restart.")

    def onDifferentApiButtonChanged(self, state):
        self.cfg.set("general", "differentApi", "True" if state == Qt.Checked else "False")
        self.host.reload()
        self.host.start()
        self.setupList()

    def onPluginsTableCurrentItemChanged(self, currow, curcol, prevrow, prevcol):
        cur = self.pluginsTable.item(currow, 0)
        if not cur:
            self.nameEdit.clear()
            self.versionEdit.clear()
            self.authorEdit.clear()
            self.descriptionEdit.clear()
            self.keywordEdit.clear()
            self.apiEdit.clear()
        else:
            p = self.host.plugins[cur.data(Qt.UserRole)]

            self.nameEdit.setText(p.name)
            self.versionEdit.setText(p.version)
            self.authorEdit.setText(p.author)
            self.descriptionEdit.setPlainText(p.description)
            self.keywordEdit.setText(p.commandKeyword)
            self.apiEdit.setText(p.apiVersion)

    def onPluginsTableItemChanged(self, item):
        checked = item.checkState() == Qt.Checked
        name = item.data(Qt.UserRole)

        if checked and name not in self.host.active:
            if self.host.activate(name):
                if self.host.active[name].offersConfigure:
                    self.pluginsTable.cellWidget(item.row(), 1).setEnabled(True)
            else:
                item.setCheckState(Qt.Unchecked)
                QMessageBox.critical(self, "Activation failed", "Error starting plugin, check your client log for more information")
        elif not checked and name in self.host.active:
            if self.host.active[name].offersConfigure:
                self.pluginsTable.cellWidget(item.row(), 1).setEnabled(False)
            self.host.deactivate(name)

    def onRemoveButtonClicked(self, pluginname):
        if pluginname in self.host.plugins:
            if self.cfg.getboolean("general", "uninstallQuestion"):
                def cbox_state_changed(state):
                    self.cfg.set("general", "uninstallQuestion", "False" if state == Qt.Checked else "True")

                cbox = QCheckBox("Don't ask me again")
                cbox.connect("stateChanged(int)", cbox_state_changed)

                msgbox = QMessageBox(self)
                msgbox.setInformativeText("Do you really want to delete plugin %s?\nThis will erase all script data of the plugin from disk." % pluginname)
                msgbox.setIcon(4) #QMessageBox::Icon::Question = 4
                msgbox.setStandardButtons(QMessageBox.Ok | QMessageBox.Cancel)
                msgbox.setDefaultButton(QMessageBox.Cancel)
                msgbox.setCheckBox(cbox)

                if msgbox.exec_() != QMessageBox.Ok:
                    return

            if pluginname in self.host.active:
                self.host.deactivate(pluginname)

            devtools.PluginInstaller.removePlugin(pluginname)
            self.host.reload()
            self.host.start()
            self.setupList()

    def onReloadButtonClicked(self):
        self.host.reload()
        self.host.start()
        self.setupList()

    def onSettingsButtonClicked(self, pluginname):
        if pluginname in self.host.active:
            self.host.active[pluginname].configure(self)

    def onBackgroundColorButtonClicked(self):
        c = QColorDialog.getColor(QColor(self.cfg.get("console", "backgroundColor")), self, "Console Background Color")
        if c.isValid():
            self.cfg.set("console", "backgroundColor", c.name())
            self.backgroundColorButton.setStyleSheet("background-color: %s;" % c.name())

    def onTextColorButtonClicked(self):
        c = QColorDialog.getColor(QColor(self.cfg.get("console", "textColor")), self, "Console Text Color")
        if c.isValid():
            self.cfg.set("console", "textColor", c.name())
            self.textColorButton.setStyleSheet("background-color: %s;" % c.name())

    def onFontFamilyComboChanged(self, font):
        self.cfg.set("console", "fontFamily", font.family())

    def onFontSizeSpinChanged(self, size):
        self.cfg.set("console", "fontSize", str(size))

    def onTabcompleteButtonChanged(self, state):
        checked = state == Qt.Checked
        self.cfg.set("console", "tabcomplete", str(checked))
        self.spacesButton.setEnabled(not checked)
        self.tabwidthSpin.setEnabled(not checked)
        self.tabwidthLabel.setEnabled(not checked)

    def onSpacesButtonChanged(self, state):
        self.cfg.set("console", "spaces", str(state == Qt.Checked))

    def onTabwidthSpinChanged(self, width):
        self.cfg.set("console", "tabwidth", str(width))

    def on_scriptButton_clicked(self):
        fname = QFileDialog.getOpenFileName(self, "Startup script", os.path.dirname(self.cfg.get("console", "startup")), "Python scripts (*.py)")
        if fname != "":
            self.scriptEdit.setText(fname)
            self.scriptEdit.setStyleSheet("border: 1px solid black")

            self.cfg.set("console", "startup", fname)

    def on_scriptEdit_textEdited(self, text):
        if os.path.exists(text) and os.path.isfile(text):
            self.scriptEdit.setStyleSheet("border: 1px solid black")

            self.cfg.set("console", "startup", text)
        else:
            self.scriptEdit.setStyleSheet("border: 1px solid red")

    def on_silentButton_toggled(self, act):
        self.cfg.set("console", "silentStartup", str(act))

    def on_repositoryButton_clicked(self):
        if not self.rpd:
            self.rpd = RepositoryDialog(self.host, self)

        self.rpd.show()
        self.rpd.raise_()


class StdRedirector:
    def __init__(self, callback):
        self.callback = callback
        self.no = False

    def write(self, text):
        if not self.no:
            self.callback(text.rstrip())
        self.no = not self.no

def defaultFont():
    if sys.platform.startswith("win"):
        return QFont("Courier", 12)
    else:
        return QFont("Monospace", 12)


class PythonConsole(QPlainTextEdit):
    def __init__(self, tabcomplete=True, spaces=True, tabwidth=2, font=defaultFont(), bgcolor=Qt.black, textcolor=Qt.white, width=800, height=600, startup="", silentStartup=False, parent=None):
        super().__init__(parent)

        self.setAttribute(Qt.WA_DeleteOnClose)
        self.setWindowTitle("pyTSon Console")

        self.tabcomplete = tabcomplete
        self.spaces = spaces
        self.tabwidth = tabwidth
        self.setFont(font)

        self.resize(width, height)

        self.setContextMenuPolicy(Qt.PreventContextMenu)

        p = self.palette
        p.setColor(QPalette.Base, bgcolor)
        p.setColor(QPalette.Text, textcolor)
        self.palette = p

        self.history = []
        self.curhistory = -1
        self.lastinp = ""
        self.locked = False
        self.globals = {}
        self.inmulti = False
        self.multi = ""

        self.comp = Completer(self.globals)

        self.selformat = self.currentCharFormat()
        self.selformat.setBackground(Qt.white)
        self.selformat.setForeground(Qt.black)

        self.norformat = self.currentCharFormat()

        if os.path.isfile(startup):
            with open(startup, "r") as f:
                self.runCommand(f.read(), silentStartup)

        self.writePrompt(self.plainText != "")

    def setFont(self, f):
        QPlainTextEdit.setFont(self, f)

        fm = QFontMetrics(f)
        self.setCursorWidth(fm.width("X"))
        self.setTabStopWidth(fm.width("X") * self.tabwidth)

    def prompt(self):
        return ">>> " if not self.inmulti else "... "

    def promptLength(self):
        return len(self.prompt())

    def writePrompt(self, newline):
        if newline:
            self.textCursor().insertBlock()

        self.textCursor().insertText(self.prompt())

    def promptCursor(self):
        retcur = QTextCursor(self.textCursor())

        retcur.movePosition(QTextCursor.End)
        retcur.movePosition(QTextCursor.StartOfLine)
        retcur.movePosition(QTextCursor.Right, QTextCursor.MoveAnchor, self.promptLength())

        return retcur

    def keyPressEvent(self, e):
        if self.locked:
            return

        if self.textCursor() < self.promptCursor():
            self.moveCursor(QTextCursor.End)

        key = e.key()
        mods = e.modifiers()

        if key == Qt.Key_C and mods & Qt.ControlModifier:
            self.doKeyboardInterrupt()
        elif key == Qt.Key_D and mods & Qt.ControlModifier:
            self.doEndFile()
        elif key == Qt.Key_Backspace and mods == Qt.NoModifier:
            if self.textCursor() > self.promptCursor():
                QPlainTextEdit.keyPressEvent(self, e)
        elif key in [Qt.Key_Enter, Qt.Key_Return] and mods == Qt.NoModifier:
            self.doExecuteCommand()
        elif key == Qt.Key_Up and mods == Qt.NoModifier:
            self.doHistoryUp()
        elif key == Qt.Key_Down and mods == Qt.NoModifier:
            self.doHistoryDown()
        elif key == Qt.Key_Left and mods == Qt.NoModifier:
            if self.textCursor().positionInBlock() > self.promptLength():
                QPlainTextEdit.keyPressEvent(self, e)
        elif key == Qt.Key_Right:
            QPlainTextEdit.keyPressEvent(self, e)
        elif key == Qt.Key_Delete and mods == Qt.NoModifier:
            QPlainTextEdit.keyPressEvent(self, e)
        elif key == Qt.Key_End and mods == Qt.NoModifier:
            self.moveCursor(QTextCursor.EndOfLine)
        elif key == Qt.Key_Tab and mods == Qt.NoModifier:
            self.doTab()
        elif (key == Qt.Key_Tab and mods == Qt.ShiftModifier) or key == Qt.Key_Backtab:
            self.doUntab()
        elif key == Qt.Key_Home and mods == Qt.NoModifier:
            self.setTextCursor(self.promptCursor())
        else:
            QPlainTextEdit.keyPressEvent(self, e)

        self.ensureCursorVisible()

    def mousePressEvent(self, e):
        if e.button() == Qt.LeftButton:
            self.seltext = ""
            cur = QTextCursor(self.document())
            cur.select(QTextCursor.Document)
            cur.setCharFormat(self.norformat)

            self.selcursor = self.textCursor()
            QPlainTextEdit.mousePressEvent(self, e)
        else:
            QPlainTextEdit.mousePressEvent(self, e)
        self.ensureCursorVisible()

    def mouseReleaseEvent(self, e):
        QPlainTextEdit.mouseReleaseEvent(self, e)
        if e.button() == Qt.LeftButton:
            self.textCursor().setCharFormat(self.selformat)
            self.seltext = self.textCursor().selectedText()
            self.setTextCursor(self.selcursor)
        elif e.button() == Qt.RightButton:
            if self.seltext == "":
                self.textCursor().insertText(QApplication.clipboard().text())
            else:
                self.textCursor().insertText(self.seltext)
                QApplication.clipboard().setText(self.seltext)

    def doKeyboardInterrupt(self):
        self.moveCursor(QTextCursor.End)

        self.textCursor().insertBlock()
        self.textCursor().insertText("KeyboardInterrupt")

        self.inmulti = False
        self.multi = ""

        self.writePrompt(True)

    def doEndFile(self):
        cur = self.textCursor()
        self.moveCursor(QTextCursor.Right)

        #delete key to the right if there are
        if cur != self.textCursor():
            self.textCursor().deletePreviousChar()
        elif self.textCursor() == self.promptCursor():
            #if no chars in cur command, close
            self.close()

    def currentLine(self):
        return self.textCursor().block().text()[self.promptLength():]

    def removeCurrentLine(self):
        self.moveCursor(QTextCursor.End, QTextCursor.MoveAnchor)
        self.moveCursor(QTextCursor.StartOfLine, QTextCursor.MoveAnchor)
        self.moveCursor(QTextCursor.End, QTextCursor.KeepAnchor)
        self.textCursor().removeSelectedText()

    def addHistory(self, cmd):
        if len(self.history) > 0:
            if cmd in self.history:
                self.history.pop(self.history.index(cmd))

        self.history.append(cmd)

    def doHistoryUp(self):
        if len(self.history) == 0:
            return

        if self.curhistory == -1:
            self.lastinp = self.currentLine()
            self.curhistory = len(self.history)

        self.removeCurrentLine()
        self.writePrompt(False)

        self.curhistory -= 1

        if self.curhistory == -1:
            self.textCursor().insertText(self.lastinp)
            self.curhistory = -1
        else:
            self.textCursor().insertText(self.history[self.curhistory])

    def doHistoryDown(self):
        if len(self.history) == 0 or self.curhistory == -1:
            return

        self.removeCurrentLine()
        self.writePrompt(False)

        self.curhistory += 1

        if self.curhistory == len(self.history):
            self.textCursor().insertText(self.lastinp)
            self.curhistory = -1
        else:
            self.textCursor().insertText(self.history[self.curhistory])

    def doTab(self):
        if self.tabcomplete:
            cmd = self.currentLine()
            tokens = list(filter(None, re.split("[, \t\-\+\*\[\]\{\}:\(\)]+", cmd)))
            if tokens[-1] != "":
                state = 0
                cur = self.comp.complete(tokens[-1], state)

                if cur:
                    l = [cur]

                    while cur:
                        state += 1
                        cur = self.comp.complete(tokens[-1], state)

                        if cur:
                            l.append(cur)

                    if len(l) == 1:
                        self.removeCurrentLine()
                        self.writePrompt(False)
                        before = cmd[:-len(tokens[-1])]
                        if before == "":
                            self.textCursor().insertText(l[0])
                        else:
                            self.textCursor().insertText(cmd[:-len(tokens[-1])] + l[0])
                    else:
                        self.appendLine("\t\t".join(l))
                        self.writePrompt(True)

                        prefix = ''.join(c[0] for c in takewhile(lambda x: all(x[0] == y for y in x), zip(*l)))
                        if prefix != '':
                            self.textCursor().insertText(cmd[:-len(tokens[-1])] + prefix)
                        else:
                            self.textCursor().insertText(cmd)
        else:
            if self.spaces:
                self.textCursor().insertText(" " * self.tabwidth)
            else:
                self.textCursor().insertText("\t")

    def doUntab(self):
        if self.tabcomplete:
            return

        sel = self.textCursor()

        tab = " " * self.tabwidth if self.spaces else "\t"
        sel.movePosition(QTextCursor.Left, QTextCursor.KeepAnchor, len(tab))
        if sel >= self.promptCursor() and sel.selectedText() == tab:
            sel.removeSelectedText()
            return

    def appendLine(self, text):
        self.appendPlainText(text)

    def runCommand(self, cmd, silent):
        if not silent:
            tmp = sys.stdout
            sys.stdout = StdRedirector(self.appendLine)

        try:
            try:
                res = eval(cmd, self.globals)
                if res != None:
                    self.appendLine(repr(res))
            except SyntaxError:
                exec(cmd, self.globals)
        except:
            if not silent:
                self.appendLine(traceback.format_exc())

        if not silent:
            sys.stdout = tmp

    def doExecuteCommand(self):
        cmd = self.currentLine().rstrip()

        if cmd == "":
            if self.inmulti:
                cmd = self.multi
                self.multi = ""
                self.inmulti = False
        else:
            self.addHistory(cmd)
            self.curhistory = -1

            if self.inmulti:
                self.multi += "\n" + cmd
                self.writePrompt(True)
                return

        if cmd.endswith(":"):
            self.inmulti = True
            self.multi = cmd
            self.writePrompt(True)
            return

        self.moveCursor(QTextCursor.End)

        self.runCommand(cmd, False)
        self.writePrompt(True)

        self.ensureCursorVisible()


class MultiInputDialog(QDialog):
    def __init__(self, title, label1, label2, parent=None):
        super().__init__(parent)
        self.setWindowTitle(title)
        self.lay = QVBoxLayout(self)

        self.lay1 = QHBoxLayout()
        self.label1 = QLabel(label1, self)
        self.lay1.addWidget(self.label1)
        self.input1 = QLineEdit(self)
        self.lay1.addWidget(self.input1)
        self.lay.addLayout(self.lay1)

        self.lay2 = QHBoxLayout()
        self.label2 = QLabel(label2, self)
        self.lay2.addWidget(self.label2)
        self.input2 = QLineEdit(self)
        self.lay2.addWidget(self.input2)
        self.lay.addLayout(self.lay2)

        bbox = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel, self)
        bbox.connect("accepted()", self.accept)
        bbox.connect("rejected()", self.reject)
        self.lay.addWidget(bbox)

    def cleanup(self):
        self.lay1.delete()
        self.lay2.delete()

    @staticmethod
    def getTexts(title, label1, label2, text1="", text2="", parent=None):
        dlg = MultiInputDialog(title, label1, label2, parent)

        dlg.label1.setText(label1)
        dlg.input1.setText(text1)
        dlg.label2.setText(label2)
        dlg.input2.setText(text2)

        if dlg.exec_() == QDialog.Accepted:
            ret1 = dlg.input1.text
            ret2 = dlg.input2.text

            dlg.cleanup()
            dlg.delete()

            return (True, ret1, ret2)
        else:
            return (False, "", "")


class RepositoryDialog(QDialog):
    master_url = QUrl("https://raw.githubusercontent.com/pathmann/pyTSon_repository/master/repositorymaster.json")

    def __init__(self, host, parent=None):
        super().__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)
        self.setWindowTitle("pyTSon - Online repository")

        self.host = host

        try:
            with open(pytson.getConfigPath("repositorymaster.json"), "r") as f:
                repmaster = json.loads(f.read())
                self.replist = {x["name"]:x for x in repmaster}
        except:
            _ts3print("Error opening repositorymaster", ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.RepositoryDialog", 0)
            raise Exception("Error opening repositorymaster")

        try:
            setupUi(self, pytson.getPluginPath("ressources", "repository.ui"))
            self.updateMasterlist()

            movie = QMovie(pytson.getPluginPath("ressources", "loading.gif"), "", self)
            movie.start()
            self.loadingLabel.setMovie(movie)

            self.masterloadingLabel.setMovie(movie)
            self.masterloadingLabel.hide()

            self.nwm = QNetworkAccessManager(self)
            self.nwm.connect("finished(QNetworkReply*)", self.onNetworkReply)

            self.updateRepositories()

            self.connect("finished(int)", self.onClosed)
        except Exception as e:
            self.delete()
            raise e

    def onClosed(self):
        with open(pytson.getConfigPath("repositorymaster.json"), "w") as f:
            json.dump(list(self.replist.values()), f)

    def updatePendingButtons(self):
        if self.pending == 0:
            self.reloadButton.setEnabled(True)
            self.updateButton.setEnabled(True)
            self.loadingLabel.hide()
            self.masterloadingLabel.hide()
        else:
            self.reloadButton.setEnabled(False)
            self.updateButton.setEnabled(False)
            self.loadingLabel.show()
            self.masterloadingLabel.show()

    def updateRepositories(self):
        self.addons = {}
        self.pluginsList.clear()

        self.pending = sum(x["active"] for x in self.replist.values())
        for rep in self.replist.values():
            if all(x in rep for x in ['name', 'url', 'origin', 'active']):
                if rep["active"]:
                    self.nwm.get(QNetworkRequest(QUrl(rep["url"])))
            else:
                self.pending -= 1
                _ts3print("Invalid repository in list, ignoring", ts3defines.LogLevel.LogLevel_WARNING, "pyTSon.RepositoryDialog.updateRepositories", 0)

        self.updatePendingButtons()

    def updateMaster(self):
        self.pending = 1

        self.masterloadingLabel.show()
        self.updateButton.setEnabled(False)

        self.nwm.get(QNetworkRequest(self.master_url))
        self.updatePendingButtons()

    def onNetworkReply(self, reply):
        self.pending -= 1

        if reply.url() == self.master_url:
            if reply.error() == QNetworkReply.NoError:
                try:
                    repos = json.loads(reply.readAll().data().decode('utf-8'))

                    for r in repos:
                        if all(x in r for x in ["name", "url", "active", "origin"]):
                            self.addRepository(r)
                        else:
                            _ts3print("Invalid entry in repositorymaster", ts3defines.LogLevel.LogLevel_WARNING, "pyTSon.RepositoryManager.onNetworkReply", 0)
                except:
                    _ts3print("Error reading repositorymaster: %s" % traceback.format_exc(), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.RepositoryManager.onNetworkReply", 0)

            self.updateMasterlist()
        else:
            repo = None
            for r in self.replist.values():
                if reply.url().url() == r["url"]:
                    repo = r
                    break

            if not repo:
                _ts3print("Error matching answer from %s to a repository" % reply.url().url(), ts3defines.LogLevel.LogLevel_ERROR, "pyTSon.RepositoryDialog.onNetworkReply", 0)
            elif reply.error() == QNetworkReply.NoError:
                try:
                    addons = json.loads(reply.readAll().data().decode('utf-8'))

                    for a in addons:
                        if all(x in a for x in ["name", "author", "version", "apiVersion", "description", "url"]):
                            a["repository"] = repo["name"]
                            if not a["name"] in self.addons:
                                self.addons[a["name"]] = a
                        else:
                            _ts3print("Invalid entry in repository %s: %s" % (repo["name"], str(addon)), ts3defines.LogLevel.LogLevel_WARNING, "pyTSon.RepositoryDialog.onNetworkReply", 0)
                            break
                except:
                    _ts3print("Error parsing repository %s: %s" % (repo["name"], traceback.format_exc()), ts3defines.LogLevel.LogLevel_WARNING, "pyTSon.RepositoryDialog.onNetworkReply", 0)
            else:
                _ts3print("Network error updating repository %s: %s" % (repo["name"], reply.error()), ts3defines.LogLevel.LogLevel_WARNING, "pyTSon.RepositoryDialog.onNetworkReply", 0)

            if self.pending == 0:
                self.updateAddonlist()

        reply.deleteLater()

        self.updatePendingButtons()

    def addRepository(self, r):
        name = r["name"]
        if name in self.replist:
            if self.replist[name]["origin"] == "online":
                if self.replist[name]["url"] != r["url"]:
                    self.replist[name]["url"] = r["url"]
                    _ts3print("url for repository %s updated" % name, ts3defines.LogLevel.LogLevel_INFO, "pyTSon.RepositoryManager.addRepository", 0)
            else:
                _ts3print("Ignoring online repository %s, got a local one with that name" % name, ts3defines.LogLevel.LogLevel_INFO, "pyTSon.RepositoryManager.addRepository", 0)
        else:
            self.replist[name] = r

    def updateMasterlist(self):
        self.repositoryList.clear()

        for name, r in self.replist.items():
            item = QListWidgetItem(name)
            item.setFlags(Qt.ItemIsSelectable | Qt.ItemIsUserCheckable | Qt.ItemIsEnabled)
            item.setCheckState(Qt.Checked if r["active"] else Qt.Unchecked)
            item.setData(Qt.UserRole, name)
            self.repositoryList.addItem(item)

    def updateAddonlist(self):
        cur = self.pluginsList.currentItem().text() if self.pluginsList.currentItem() else None

        self.pluginsList.clear()

        for a in self.addons.values():
            if self.replist[a["repository"]]["active"] and ("platforms" not in a or pytson.platformstr() in a["platforms"]):
                item = QListWidgetItem(a["name"], self.pluginsList)
                item.setFlags(Qt.ItemIsSelectable | Qt.ItemIsUserCheckable | Qt.ItemIsEnabled)
                item.setData(Qt.UserRole, a["name"])

                if a["name"] in self.host.plugins:
                    if a["version"] > self.host.plugins[a["name"]].version:
                        item.setForeground(Qt.red)
                        item.setToolTip("Update available")
                    elif a["version"] == self.host.plugins[a["name"]].version:
                        item.setForeground(Qt.green)
                        item.setToolTip("You have this plugin installed, no update available")
                    elif a["version"] < self.host.plugins[a["name"]].version:
                        item.setForeground(Qt.gray)
                        item.setToolTip("Your local version has a greater version number")

                if cur and a["name"] == cur:
                    self.pluginsList.setCurrentItem(item)

        self.pluginsList.sortItems()

    def on_updateButton_clicked(self):
        self.updateMaster()

    def on_addButton_clicked(self):
        (res, name, url) = MultiInputDialog.getTexts("Add repository", "Name:", "URL:", "", "", self)

        if res:
            qurl = QUrl(url)
            if qurl.isValid() and not qurl.isLocalFile():
                rep = dict()
                rep["name"] = name
                rep["url"] = url
                rep["origin"] = "local"
                rep["active"] = True

                self.replist[name] = rep

                item = QListWidgetItem(name)
                item.setFlags(Qt.ItemIsSelectable | Qt.ItemIsUserCheckable | Qt.ItemIsEnabled)
                item.setCheckState(Qt.Checked)
                item.setData(Qt.UserRole, name)
                self.repositoryList.addItem(item)
            else:
                QMessageBox.critical("Error", "The URL %s is not valid" % url)

    def on_deleteButton_clicked(self):
        cur = self.repositoryList.currentItem()
        if cur:
            name = cur.data(Qt.UserRole)
            if not name in self.replist:
                QMessageBox.critical("Internal error", "Can't find repository %s in list" % name)
                return

            del self.replist[name]
            cur.delete()

    def on_repositoryList_doubleClicked(self, item):
        name = item.data(Qt.UserRole)
        try:
            rep = self.replist[name]
        except:
            QMessageBox.critical(self, "Internal error", "Can't find repository %s in list" % name)
            return

        ok = BoolResult()
        newurl = QInputDialog.getText(self, "Change url of repository %s" % name, "Url:", QLineEdit.Normal, rep["url"], ok)

        if ok:
            rep["url"] = newurl
            rep["origin"] = "local"

    def on_repositoryList_currentItemChanged(self, cur, prev):
        if cur:
            name = cur.data(Qt.UserRole)
            if not name in self.replist:
                self.deleteButton.setEnabled(False)
                QMessageBox.critical(self, "Internal error", "Can't find repository %s in list" % name)
                return

            self.deleteButton.setEnabled(True)
        else:
            self.deleteButton.setEnabled(False)

    def on_repositoryList_itemChanged(self, item):
        if item:
            name = item.data(Qt.UserRole)

            if not name in self.replist:
                QMessageBox.critical(self, "Internal error", "Can't find repository %s in list" % name)
                return
            if self.replist[name]["active"] != (item.checkState() == Qt.Checked):
                self.replist[name]["active"] = (item.checkState() == Qt.Checked)
                self.updateAddonlist()

    def on_pluginsList_currentItemChanged(self, cur, prev):
        if cur:
            name = cur.data(Qt.UserRole)
            if not name in self.addons:
                QMessageBox.critical("Internal error", "Can't find addon %s in list" % name)
                return

            p = self.addons[name]
            self.nameEdit.setText(p["name"])
            self.authorEdit.setText(p["author"])
            self.versionEdit.setText(p["version"])
            self.descriptionEdit.setPlainText(p["description"])
            self.apiEdit.setText(p["apiVersion"])
            self.repositoryEdit.setText(p["repository"])

            if name in self.host.plugins:
                if p["version"] > self.host.plugins[name].version:
                    self.installButton.setEnabled(True)
                    self.installButton.setText("Update")
                else:
                    self.installButton.setEnabled(False)
                    self.installButton.setText("Install")
            else:
                self.installButton.setEnabled(True)
                self.installButton.setText("Install")
        else:
          self.nameEdit.clear()
          self.authorEdit.clear()
          self.versionEdit.clear()
          self.descriptionEdit.clear()
          self.apiEdit.clear()
          self.repositoryEdit.clear()

    def on_reloadButton_clicked(self):
        self.updateRepositories()

    def on_installButton_clicked(self):
        item = self.pluginsList.currentItem()
        if not item:
            return

        name = item.data(Qt.UserRole)
        if not name in self.addons:
            QMessageBox.critical(self, "Internal error", "Can't find addon %s in list" % name)
            return

        p = self.addons[name]

        #update?, so remove the local one first
        if name in self.host.plugins:
            if p["version"] > self.host.plugins[name].version:
                devtools.PluginInstaller.removePlugin(name)
            else:
                #should not happen (ui restricted)
                QMessageBox.critical(self, "This plugin is already installed")
                return

        self.installer = InstallDialog(self.host, self)
        self.installer.show()
        self.installer.install(p)


class InstallDialog(QDialog):
    def __init__(self, host, parent=None):
        super().__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)
        self.setModal(True)

        self.host = host
        self.addon = None

        try:
            setupUi(self, os.path.join(ts3.getPluginPath(), "pyTSon", "ressources", "installer.ui"))

            self.pip = devtools.PluginInstaller(self.consoleEdit.append)
            self.nwm = QNetworkAccessManager(self)
            self.nwm.connect("finished(QNetworkReply*)", self.onNetworkReply)
        except Exception as e:
            self.delete()
            raise e

    def install(self, addon):
        self.addon = addon

        self.nwm.get(QNetworkRequest(QUrl(addon["url"])))
        self.consoleEdit.append("Downloading %s ..." % addon["url"])

    def onNetworkReply(self, reply):
        if reply.error() == QNetworkReply.NoError:
            self.consoleEdit.append("Download finished.")

            if reply.header(QNetworkRequest.ContentTypeHeader) != "application/zip":
                self.pip.installPlugin(self.addon, reply.readAll().data().decode('utf-8'))
            else:
                self.pip.installPlugin(self.addon, io.BytesIO(reply.readAll()))
        else:
            self.consoleEdit.append("Network error: %s" % reply.error())

        reply.deleteLater()
