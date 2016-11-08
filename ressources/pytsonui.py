import sys, os

from enum import Enum, unique

from PythonQt.QtGui import *
from PythonQt.QtCore import Qt, QFile, QIODevice
from PythonQt.QtUiTools import QUiLoader

from rlcompleter import Completer
import traceback, re
from itertools import takewhile

import ts3

@unique
class ValueType(Enum):
    boolean = 1
    integer = 2
    double = 3
    string = 4
    listitem = 5

def _createReturnDict(widgets):
    ret = {}

    for key, w in widgets.items():
        if not key in ["dialog", "buttonbox"]:
            if type(w) is QCheckBox:
                ret[key] = w.checked
            elif type(w) is QSpinBox or type(w) is QDoubleSpinBox:
                ret[key] = w.value
            elif type(w) is QLineEdit:
                ret[key] = w.text
            elif type(w) is QPlainTextEdit:
                ret[key] = w.plainText
            elif type(w) is QGroupBox:
                counter = 0
                for c in w.children():
                    if type(c) is QRadioButton:
                        if c.checked:
                            ret[key] = counter

                        counter += 1
            elif type(w) is QComboBox:
                ret[key] = w.currentIndex
            elif type(w) is QListWidget:
                ret[key] = [w.row(item) for item in w.selectedItems()]

    return ret

def getValues(parent, title, params, doneclb):
    """
    Convenience function to open a dialog to get multiple input values from the user.
    @param parent: the dialog's parent, pass None to ignore
    @type parent: QWidget (or derived type)
    @param title: the dialog's title
    @type title: str
    @param params: a dict definining the user input type {'key': (ValueType, label, startingValue, minimum, maximum)}. Potential types are defined in the enum ValueType. The label will be displayed right next to the input widget. All other elements in this tuple are dependent on the ValueType. startingValue defines a predefined value of input. Minimum and maximum define input limits.
    boolean: startingValue is bool, minimum and maximum are not used; the widget used is a QCheckBox without an extra QLabel
    integer: startingValue, minimum and maximum are int; the widget used is a QSpinBox with an extra QLabel
    double: startingValue, minimum; the widget used is a QDoubleSpinBox with an extra QLabel
    string: startingValue is str, minimum is not used, if maximum == 1 the widget used is a QLineEdit, otherwise a QPlainTextEdit with a maximumBlockCount of maximum, each with an extra QLabel
    listitem: startingValue is a tuple([str], [int]) defining the listitems in the first element, the second element is a list with prechecked item indexes, minimum is an int defining how much items the user at least has to choose, maximum is an int defining if the user can choose more than one item (maximum != 1), depending on minimum and maximum the used widget is a QGroupBox and multiple QRadioButtons, a QComboBox with an extra QLabel or a QListWidget with an extra QLabel
    @type params: dict{str: tuple(ValueType, str, int/double/str/tuple(list[str], list[int]), int/double, int/double)}
    @param doneclb: a callable which gets the dialogs return code (see QDialog.DialogCode) and on success, a dict with the resulting values, referenced by the key.
    @type doneclb: callable(int, dict{str: int/str/bool/[str]})
    @return: Returns a dict containing the used input widgets plus the dialog and the QDialogButtonBox
    @rtype: dict{str: QWidget}

    """
    ret = {}

    dlg = ret['dialog'] = QDialog(parent)
    dlg.setWindowTitle(title)

    dlg.connect("finished(int)", lambda r: (doneclb(r, _createReturnDict(ret)) if r == QDialog.Accepted else doneclb(r, {}), dlg.delete()))

    form = QFormLayout()
    box = ret['buttonbox'] = QDialogButtonBox(QDialogButtonBox.Cancel | QDialogButtonBox.Ok, Qt.Horizontal, dlg)
    box.connect("accepted()", dlg.accept)
    box.connect("rejected()", dlg.reject)

    vlayout = QVBoxLayout(dlg)
    vlayout.addLayout(form)
    vlayout.addWidget(box)

    dlg.setLayout(vlayout)

    for key, (t, label, start, minimum, maximum) in params.items():
        if key in ["dialog", "buttonbox"]:
            dlg.delete()
            raise Exception("Keys dialog and buttonbox are forbidden")

        if t is ValueType.boolean:
            w = ret[key] = QCheckBox(label, dlg)
            w.setChecked(start)

            form.addRow(w)
        elif t is ValueType.integer:
            l = QLabel(label, dlg)
            w = ret[key] = QSpinBox(dlg)
            w.setMinimum(minimum)
            w.setMaximum(maximum)
            w.setValue(start)

            form.addRow(l, w)
        elif t is ValueType.double:
            l = QLabel(label, dlg)
            w = ret[key] = QDoubleSpinBox(dlg)
            w.setMinimum(minimum)
            w.setMaximum(maximum)
            w.setValue(start)

            form.addRow(l, w)
        elif t is ValueType.string:
            l = QLabel(label, dlg)
            if maximum == 1:
                w = ret[key] = QLineEdit(start, dlg)
            else:
                w = ret[key] = QPlainTextEdit(start, dlg)
                w.setMaximumBlockCount(maximum)

            form.addRow(l, w)
        elif t is ValueType.listitem:
            if minimum == maximum == 1:
                grp = ret[key] = QGroupBox(label, dlg)
                layout = QVBoxLayout(grp)
                for i, s in enumerate(start[0]):
                    b = QRadioButton(s, grp)
                    b.setChecked(i in start[1])

                    layout.addWidget(b)

                form.addRow(grp)
            elif maximum == 1:
                l = QLabel(label, dlg)
                w = QComboBox(dlg)
                w.addItems(start[0])
                if len(start[1]) > 0:
                    w.setCurrentIndex(start[1][0])

                form.addRow(l, w)
            else:
                l = QLabel(label, dlg)
                w = QListWidget(dlg)
                for i, s in enumerate(start[0]):
                    item = QListWidgetItem(s, w)

                    item.setFlags(Qt.ItemIsSelectable | Qt.ItemIsUserCheckable | Qt.ItemIsEnabled)
                    item.setCheckState(Qt.Checked if i in start[1] else Qt.Unchecked)

                form.addRow(l, w)
        else:
            dlg.delete()
            raise Exception("Unrecognized ValueType")

    dlg.show()

    return ret


def retrieveWidgets(obj, parent, widgets):
        if type(parent) is QTabWidget:
            childs = [parent.widget(i) for i in range(0, parent.count)]
        else:
            childs = parent.children()

        names = [w[0] for w in widgets]
        stores = [w[1] for w in widgets]
        grchilds = [w[2] for w in widgets]

        for c in childs:
            if c.objectName in names:
                i = names.index(c.objectName)
                if stores[i]:
                    setattr(obj, names[i], c)

                #connect slots by name
                for sig in dir(c):
                    if type(getattr(c, sig)).__name__ == "builtin_qt_signal":
                        if hasattr(obj, "on_%s_%s" % (c.objectName, sig.split('(')[0])):
                            getattr(c, sig).connect(getattr(obj, "on_%s_%s" % (c.objectName, sig.split('(')[0])))

                retrieveWidgets(obj, c, grchilds[i])

                names.pop(i)
                stores.pop(i)
                grchilds.pop(i)

            if len(names) == 0:
                return

        if len(names) != 0:
            raise Exception("Malformed uifile, widgets not found: %s" % names)

def setupUi(obj, uipath, widgets):
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

    retrieveWidgets(obj, ui, widgets)

    layout = QHBoxLayout(obj)
    layout.addWidget(ui)
    obj.setLayout(layout)


class ConfigurationDialog(QDialog):
    #[(objectName, store, [children])]
    CONF_WIDGETS = [("tabWidget", False, [
                      ("pluginsTab", False, [
                          ("differentApiButton", True, []),
                          ("pluginsList", True, []),
                          ("reloadButton", True, []),
                          ("settingsButton", True, []),
                          ("versionEdit", True, []),
                          ("nameEdit", True, []),
                          ("authorEdit", True, []),
                          ("descriptionEdit", True, []),
                          ("keywordEdit", True, []),
                          ("apiEdit", True, []),
                          ("requiredApiEdit", True, [])
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
        super(QDialog, self).__init__(parent)

        self.setAttribute(Qt.WA_DeleteOnClose)

        self.cfg = cfg
        self.host = host

        setupUi(self, os.path.join(ts3.getPluginPath(), "pyTSon", "ressources", "pyTSon-configdialog.ui"), self.CONF_WIDGETS)

        self.setWindowTitle("pyTSon - Settings")

        self.setupValues()
        self.setupSlots()

    def setupList(self):
        self.pluginsList.clear()

        for key, p in self.host.plugins.items():
            if self.cfg.getboolean("general", "differentApi", fallback=False) or p.apiVersion == 20:
                item = QListWidgetItem(self.pluginsList)
                item.setText(p.name)
                item.setFlags(Qt.ItemIsSelectable | Qt.ItemIsUserCheckable | Qt.ItemIsEnabled)
                item.setCheckState(Qt.Checked if key in self.host.active else Qt.Unchecked)
                item.setData(Qt.UserRole, key)

        self.pluginsList.sortItems()

    def setupValues(self):
        self.differentApiButton.setChecked(Qt.Checked if self.cfg.getboolean("general", "differentApi", fallback=False) else Qt.Unchecked)
        self.settingsButton.setEnabled(False)
        self.requiredApiEdit.setText(20)

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
        self.differentApiButton.connect("stateChanged(int)", self.onDifferentApiButtonChanged)
        self.pluginsList.connect("currentItemChanged(QListWidgetItem*, QListWidgetItem*)", self.onPluginsListCurrentItemChanged)
        self.pluginsList.connect("itemChanged(QListWidgetItem*)", self.onPluginsListItemChanged)
        self.reloadButton.connect("clicked()", self.onReloadButtonClicked)
        self.settingsButton.connect("clicked()", self.onSettingsButtonClicked)

        self.backgroundColorButton.connect("clicked()", self.onBackgroundColorButtonClicked)
        self.textColorButton.connect("clicked()", self.onTextColorButtonClicked)
        self.fontFamilyCombo.connect("currentFontChanged(QFont)", self.onFontFamilyComboChanged)
        self.fontSizeSpin.connect("valueChanged(int)", self.onFontSizeSpinChanged)
        self.tabcompleteButton.connect("stateChanged(int)", self.onTabcompleteButtonChanged)
        self.spacesButton.connect("stateChanged(int)", self.onSpacesButtonChanged)
        self.tabwidthSpin.connect("valueChanged(int)", self.onTabwidthSpinChanged)

    def onDifferentApiButtonChanged(self, state):
        self.cfg.set("general", "differentApi", "True" if state == Qt.Checked else "False")
        self.host.reload()
        self.setupList()

    def onPluginsListCurrentItemChanged(self, cur, prev):
        if not cur:
            self.nameEdit.clear()
            self.versionEdit.clear()
            self.authorEdit.clear()
            self.descriptionEdit.clear()
            self.keywordEdit.clear()
            self.apiEdit.clear()

            self.settingsButton.setEnabled(False)
        else:
            p = self.host.plugins[cur.data(Qt.UserRole)]

            self.nameEdit.setText(p.name)
            self.versionEdit.setText(p.version)
            self.authorEdit.setText(p.author)
            self.descriptionEdit.setPlainText(p.description)
            self.keywordEdit.setText(p.commandKeyword)
            self.apiEdit.setText(p.apiVersion)

            self.settingsButton.setEnabled(p.name in self.host.active and p.offersConfigure)

    def onPluginsListItemChanged(self, item):
        checked = item.checkState() == Qt.Checked
        name = item.data(Qt.UserRole)

        if checked and name not in self.host.active:
            self.host.activate(name)
        elif not checked and name in self.host.active:
            self.host.deactivate(name)

        if self.pluginsList.currentItem() == item:
            self.settingsButton.setEnabled(checked and name in self.host.active and self.host.active[name].offersConfigure)

    def onReloadButtonClicked(self):
        self.host.reload()
        self.host.start()
        self.setupList()

    def onSettingsButtonClicked(self):
        cur = self.pluginsList.currentItem()
        if cur:
            self.host.active[cur.data(Qt.UserRole)].configure(self)

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
        super(QPlainTextEdit, self).__init__(parent)

        self.setAttribute(Qt.WA_DeleteOnClose)

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

