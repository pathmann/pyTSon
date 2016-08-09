import sys, os

from PythonQt.QtGui import QPlainTextEdit, QPalette, QColor, QFont, QFontMetrics, QTextCursor, QTextCharFormat, QDialog, QHBoxLayout, QTabWidget, QListWidgetItem, QColorDialog
from PythonQt.QtCore import Qt, QFile, QIODevice
from PythonQt.QtUiTools import QUiLoader

from rlcompleter import Completer
import traceback
from itertools import takewhile

import ts3


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
        
        p = self.backgroundColorButton.palette
        p.setColor(QPalette.Button, QColor(self.cfg.get("console", "backgroundColor")))
        self.backgroundColorButton.palette = p
        
        p = self.textColorButton.palette
        p.setColor(QPalette.Button, QColor(self.cfg.get("console", "textColor")))
        self.textColorButton.palette = p
    
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
            p = self.backgroundColorButton.palette
            p.setColor(QPalette.Button, c)
            self.backgroundColorButton.palette = p
        
    def onTextColorButtonClicked(self):
        c = QColorDialog.getColor(QColor(self.cfg.get("console", "textColor")), self, "Console Text Color")
        if c.isValid():
            self.cfg.set("console", "textColor", c.name())
            p = self.textColorButton.palette
            p.setColor(QPalette.Button, c)
            self.textColorButton.palette = p
        
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


class StdRedirector:
    def __init__(self, callback):
        self.callback = callback
        self.no = False
        
    def write(self, text):
        if not self.no:
            self.callback(text.rstrip())
        self.no = not self.no


class PythonConsole(QPlainTextEdit):
    def __init__(self, tabcomplete=True, spaces=True, tabwidth=2, font=QFont("Monospace", 12), bgcolor=Qt.black, textcolor=Qt.white, width=800, height=600, parent=None):
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
        
        self.writePrompt(False)
        
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
            self.setTextCursor(promptCursor())
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
            self.textCursor().insertText(self.seltext)
    
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
            tokens = cmd.split(" ")
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
                        before = " ".join(tokens[:-1])
                        if before == "":
                            self.textCursor().insertText(l[0])
                        else:
                            self.textCursor().insertText(" ".join(tokens[:-1]) + " " + l[0])
                    else:
                        self.appendLine("\t\t".join(l))
                        self.writePrompt(True)
                        
                        prefix = ''.join(c[0] for c in takewhile(lambda x: all(x[0] == y for y in x), zip(*l)))
                        if prefix != '':
                            self.textCursor().insertText(" ".join(tokens[:-1]) + " " + prefix)
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
            self.appendLine(traceback.format_exc())
            
        sys.stdout = tmp
        self.writePrompt(True)
        
        self.ensureCursorVisible()
        

