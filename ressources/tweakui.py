from ts3plugin import ts3plugin

from PythonQt.QtGui import QApplication, QMainWindow, QSplitter, QStackedWidget, QDockWidget, QWidget
from PythonQt.QtCore import QTimer, Qt

class tweakui(ts3plugin):
    name = "tweakui"
    requestAutoload = False
    version = "1.0"
    apiVersion = 21
    author = "Thomas \"PLuS\" Pathmann"
    description = "Tweak the client UI to use DockingWidgets, just to show possibilities of pyTSon in conjunction with PythonQt. There is no warranty!"
    offersConfigure = False
    commandKeyword = ""
    infoTitle = None
    menuItems = []
    hotkeys = []


    def __init__(self):
        self.sviews = []
        self.retrieveWidgets()

    def stop(self):
        if self.isdocked:
            self.main.removeDockWidget(self.infodock)
            self.main.removeDockWidget(self.chatdock)

            self.splitter.addWidget(self.chat)

            for i in range(0, len(self.sviews)):
                w = self.stack.widget(i)
                self.stack.removeWidget(w)
                w.setParent(self.sviews[i])
                w.show()


            self.stack.delete()
            self.chatdock.delete()
            self.infodock.delete()

    def retrieveWidgets(self):
        process = False

        if not hasattr(self, "main"):
            qapp = QApplication.instance()

            for w in qapp.topLevelWidgets():
                if "MainWindow" in str(type(w)):
                    self.main = w
                    process = True
        else:
            process = True


        if process and not hasattr(self, "splitter"):
            for c in self.main.children():
                if type(c) is QSplitter:
                    self.splitter = c
                    break

            if not hasattr(self, "splitter"):
                process = False

        if process and (not hasattr(self, "chat") or not hasattr(self, "tabwidget")):
            for c in self.splitter.children():
                if c.objectName == "MainWindowChatWidget":
                    self.chat = c
                elif c.objectName == "MainWindowServerTabsWidget":
                    self.tabwidget = c

            if not hasattr(self, "chat") or not hasattr(self, "tabwidget"):
                process = False

        if process and not hasattr(self, "tab"):
            for c in self.tabwidget.children():
                if "ServerViewManager" in str(type(c)):
                    self.tab = c
                    break

            if not hasattr(self, "tab"):
                process = False

        if process and not hasattr(self, "svparent"):
            for c in self.tab.children():
                if type(c) is QStackedWidget:
                    self.svparent = c
                    break

        if not process:
            #it's possible that this plugin is started before the client's UI is loaded
            QTimer.singleShot(300, self.retrieveWidgets)
        else:
            self.tab.connect("currentChanged(int)", self.onTabIndexChanged)

            self.startDocking()

    def startDocking(self):
        self.isdocked = True

        infoarea = Qt.RightDockWidgetArea
        chatarea = Qt.BottomDockWidgetArea

        self.stack = QStackedWidget();
        self.chatdock = QDockWidget("Chat");
        self.infodock = QDockWidget("Info");

        self.infodock.setWidget(self.stack)
        self.infodock.setAllowedAreas(Qt.AllDockWidgetAreas)
        self.infodock.setFeatures(QDockWidget.DockWidgetMovable | QDockWidget.DockWidgetFloatable)

        self.chatdock.setWidget(self.chat)
        self.chatdock.setAllowedAreas(Qt.AllDockWidgetAreas)
        self.chatdock.setFeatures(QDockWidget.DockWidgetMovable | QDockWidget.DockWidgetFloatable)

        self.main.addDockWidget(infoarea, self.infodock)
        self.main.addDockWidget(chatarea, self.chatdock)

        for c in self.svparent.children():
            if "ServerView" in str(type(c)):
                for cc in c.children():
                    if type(cc) is QWidget:
                        self.stack.addWidget(cc)
                        self.sviews.append(c)
                        c.connect("destroyed(QObject*)", self.onServerViewDestroyed)

                        if self.tab.currentWidget() == c:
                            self.stack.setCurrentIndex(len(self.sviews) -1)

    def onTabIndexChanged(self, index):
        for i in range(0, len(self.sviews)):
            if self.sviews[i] == self.tab.widget(index):
                self.stack.setCurrentIndex(i)
                return

        #new serverview
        for c in self.svparent.children():
            if "ServerView" in str(type(c)) and not c in self.sviews:
                for cc in c.children():
                    if type(cc) is QWidget:
                        self.stack.addWidget(cc)
                        self.sviews.append(c)
                        c.connect("destroyed(QObject*)", self.onServerViewDestroyed)

                        if self.tab.currentWidget() == c:
                            self.stack.setCurrentIndex(len(self.sviews) -1)

    def onServerViewDestroyed(self, obj):
        for i in range(0, len(self.sviews)):
            if self.sviews[i] == obj:
                self.sviews.remove(obj)
                w = self.stack.widget(i)
                self.stack.removeWidget(w)
                w.delete()
                return
