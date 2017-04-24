import sys
import os

import traceback
import re

from rlcompleter import Completer
from itertools import takewhile

from PythonQt.QtCore import Qt
from PythonQt.QtGui import (QFont, QFontMetrics, QPlainTextEdit, QPalette,
                            QTextCursor, QApplication)

import pytson


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


class PythonConsole(QPlainTextEdit, pytson.Translatable):
    def __init__(self, tabcomplete=True, spaces=True, tabwidth=2,
                 font=defaultFont(), bgcolor=Qt.black, textcolor=Qt.white,
                 width=800, height=600, startup="", silentStartup=False,
                 parent=None, *, catchstd=False):
        super(QPlainTextEdit, self).__init__(parent)

        self.setLineWrapMode(QPlainTextEdit.NoWrap)

        self.setAttribute(Qt.WA_DeleteOnClose)
        self.setWindowTitle(self._tr("pyTSon Console"))

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
        self.incmd = False

        self.comp = Completer(self.globals)

        self.selformat = self.currentCharFormat()
        self.selformat.setBackground(Qt.white)
        self.selformat.setForeground(Qt.black)

        self.norformat = self.currentCharFormat()

        if catchstd:
            self.redirector = StdRedirector(self.appendLine)
            self.stdout = sys.stdout
            sys.stdout = self.redirector

            self.connect("destroyed()", self._resetStdout)
        else:
            self.redirector = None

        if os.path.isfile(startup):
            with open(startup, "r") as f:
                self.runCommand(f.read(), silentStartup)

        self.writePrompt(self.plainText != "")

    def _resetStdout(self):
        sys.stdout = self.stdout

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
        retcur.movePosition(QTextCursor.Right, QTextCursor.MoveAnchor,
                            self.promptLength())

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
        elif ((key == Qt.Key_Tab and mods == Qt.ShiftModifier) or
              key == Qt.Key_Backtab):
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

        # delete key to the right if there are
        if cur != self.textCursor():
            self.textCursor().deletePreviousChar()
        elif self.textCursor() == self.promptCursor():
            # if no chars in cur command, close
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
            tokens = list(filter(None, re.split("[, \t\-\+\*\[\]\{\}:\(\)]+",
                                 cmd)))
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
                            self.textCursor().insertText(cmd[:-len(tokens[-1])]
                                                         + l[0])
                    else:
                        self.appendLine("\t\t".join(l))
                        self.writePrompt(True)

                        prefix = ''.join(c[0] for c in takewhile(lambda x:
                                         all(x[0] == y for y in x), zip(*l)))
                        if prefix != '':
                            self.textCursor().insertText(cmd[:-len(tokens[-1])]
                                                         + prefix)
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
        curline = self.currentLine()
        sel = self.textCursor()

        self.moveCursor(QTextCursor.End)
        self.appendPlainText(text)

        if not self.incmd:
            self.writePrompt(True)

            if curline != "":
                self.textCursor().insertText(curline)

                cur = self.promptCursor()
                cur.movePosition(QTextCursor.Right, QTextCursor.MoveAnchor,
                                 sel.positionInBlock() - self.promptLength())
                self.setTextCursor(cur)

    def runCommand(self, cmd, silent):
        self.incmd = True

        if not self.redirector and not silent:
            tmp = sys.stdout
            sys.stdout = StdRedirector(self.appendLine)

        try:
            try:
                res = eval(cmd, self.globals)
                if res is not None:
                    self.appendLine(repr(res))
            except SyntaxError:
                exec(cmd, self.globals)
        except:
            if not silent:
                self.appendLine(traceback.format_exc(chain=False))

        if not self.redirector and not silent:
            sys.stdout = tmp

        self.incmd = False

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
