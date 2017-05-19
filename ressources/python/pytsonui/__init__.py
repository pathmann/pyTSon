import os

from PythonQt.QtCore import QFile, QIODevice
from PythonQt.QtGui import (QIcon, QTabWidget, QSplitterHandle, QButtonGroup,
                            QAction)

from PythonQt.QtUiTools import QUiLoader

import ts3lib
import ts3defines
import pytson
import ts3client


def ts3print(msg, level, channel, aid):
    err = ts3lib.logMessage(msg, level, channel, aid)
    if err != ts3defines.ERROR_ok:
        print(msg)


def setIcon(obj, iconpack, pluginicons=None):
    """
    Sets the icon of a QWidget (if it has a property Icon) to an icon in the
    iconpack represented by a variable which is acquired by the property
    'pytsonicon' of the object.
    If the property instead contains a string formated as
    "octicons:filename.png", the icon is set to filename.png of the octicons
    pack.
    If no such property is available, nothing is done.
    @param obj: the widget
    @type obj: QWidget
    @param iconpack: the iconpack
    @type iconpack: ts3client.IconPack
    @param pluginicons: callable which gets a string and either returns the
    path to the image file or returns a QPixmap to set the icon property to;
    defaults to None
    @type pluginicons: Callable(str) -> str or QIcon
    """
    if iconpack:
        if hasattr(obj, "setIcon") and hasattr(obj, "pytsonicon"):
            var = obj.pytsonicon

            if var.startswith("octicons:"):
                fname = os.path.join(pytson.getPluginPath("ressources",
                                     "octicons", var.split(":")[1]))
                if os.path.isfile(fname):
                    obj.setIcon(QIcon(fname))
            elif var.startswith("plugin:"):
                if pluginicons:
                    ico = pluginicons(var.split(":")[1])
                    if type(ico) is str:
                        obj.setIcon(QIcon(ico))
                    else:
                        obj.setIcon(ico)
            else:
                obj.setIcon(QIcon(iconpack.icon(var)))


def connectSignalSlotsByName(sender, receiver):
    """
    Connects pythonqt signals by name
    (receiver.on_<sender.objectname>_<signalname>)
    @param sender: the sender of signals
    @type sender: QObject
    @param receiver: the receiver which has slots as callables defined
    @type receiver: object
    """
    for sig in dir(sender):
        if type(getattr(sender, sig)).__name__ == "builtin_qt_signal":
            if hasattr(receiver,
                       "on_%s_%s" % (sender.objectName, sig.split('(')[0])):
                getattr(sender, sig).connect(getattr(receiver,
                                             "on_%s_%s" % (sender.objectName,
                                                           sig.split('(')[0])))


def retrieveWidgets(obj, parent, widgets, seticons=True, iconpack=None,
                    pluginicons=None):
    """
    Retrieves widgets from a list and adds them as attribute to another object.
    If defined, signals from widgets are connected by name to methods in obj.
    @param obj: the object which will get the attributes added
    @type obj: object
    @param parent: the toplevel widget
    @type parent: QWidget
    @param widgets: a recursive (parent-relation of widgets) list of tuples,
    defining which widgets should be added as attributes to obj.
    The elements must be children of parent. First element of tuple must held
    the widget's objectname. If second element is True, the widget will be
    added as property (by objectname) to obj. Third element of the tuple are
    the child widgets, which should be handled by setupui
    @type widgets: list[tuple(str, bool, list(...))]
    @param seticons: if True, icons will be set according to
    the widgets 'pytsonicon' attribute
    @type seticons: bool
    @param iconpack: the iconpack
    @type iconpack: ts3client.IconPack
    @param pluginicons: callable which gets a string and either returns the
    path to the image file or returns a QPixmap to set the icon property to;
    defaults to None
    @type pluginicons: Callable(str) -> str or QIcon
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
            ts3print(pytson.tr("pytsonui", "Error loading iconpack: "
                               "{exception}").format(exception=e),
                     ts3defines.LogLevel.LogLevel_ERROR,
                     "pytsonui.retrieveWidgets.%s" % obj.objectName, 0)

    names = [w[0] for w in widgets]
    stores = [w[1] for w in widgets]
    grchilds = [w[2] for w in widgets]

    for c in childs:
        if c.objectName in names:
            i = names.index(c.objectName)
            if stores[i]:
                if hasattr(obj, names[i]):
                    raise Exception("Widget objectName %s is not unique" %
                                    names[i])
                setattr(obj, names[i], c)

            connectSignalSlotsByName(c, obj)

            if seticons:
                setIcon(c, iconpack, pluginicons)

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


def retrieveAllWidgets(obj, parent, seticons=True, iconpack=None,
                       pluginicons=None):
    """
    Retrieves all child widgets from a parent widget and adds them as attribute
    to another object. If defined, signals from widgets are connected by name
    to methods in obj.
    @param obj: the object which will get the attributes added
    @type obj: object
    @param parent: the toplevel widget
    @type parent: QWidget
    @param seticons: if True, icons will be set according
    to the widgets 'pytsonicon' attribute
    @type seticons: bool
    @param iconpack: the iconpack
    @type iconpack: ts3client.IconPack
    @param pluginicons: callable which gets a string and either returns the
    path to the image file or returns a QPixmap to set the icon property to;
    defaults to None
    @type pluginicons: Callable(str) -> str or QIcon
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
            ts3print(pytson.tr("pytsonui", "Error loading iconpack: "
                     "{exception}").format(exception=e),
                     ts3defines.LogLevel.LogLevel_ERROR,
                     "pytsonui.retrieveAllWidgets.%s" % obj.objectName, 0)

    for c in parent.children():
        if not c.isWidgetType():
            if (not isinstance(c, (QButtonGroup, QAction)) and
               not c.inherits("QLayout")):
                continue

        if (c.objectName != "" and type(c) not in [QSplitterHandle] and
            c.objectName not in ["qt_scrollarea_viewport",
                                 "qt_scrollarea_hcontainer",
                                 "qt_scrollarea_vcontainer",
                                 "qt_spinbox_lineedit",
                                 "qt_toolbox_toolboxbutton"]):
            if not hasattr(obj, c.objectName):
                setattr(obj, c.objectName, c)

            connectSignalSlotsByName(c, obj)

            if seticons:
                setIcon(c, iconpack, pluginicons)

            retrieveAllWidgets(obj, c, seticons, iconpack, pluginicons)

    if root:
        iconpack.close()


class UiLoader(QUiLoader):
    """
    QUILoader subclass to omit the parent widget from being recreated.
    """

    def __init__(self, main, parent=None, *, extraWidgets=None):
        """
        Instantiate a new object
        @param main: parent class which will be omitted
        @type main: QWidget
        @param parent: parent class; defaults to None
        @type parent: QObject
        @param extraWidgets: list of extra classes to be created, there must
        be a constructor which takes only the parent object
        @type extraWidgets: list(QWidget type)
        """
        super().__init__(parent)
        self.main = main
        self.extraWidgets = extraWidgets

    def createWidget(self, clsname, parent=None, name=''):
        if parent is None:
            return self.main
        else:
            if self.extraWidgets:
                for w in self.extraWidgets:
                    if w.__name__ == clsname:
                        obj = w(parent)
                        obj.setObjectName(name)
                        return obj

            return QUiLoader.createWidget(self, clsname, parent, name)


def setupUi(obj, uipath, *, widgets=None, seticons=True, iconpack=None,
            pluginicons=None, extraWidgets=None):
    """
    Loads a Qt designer file (.ui), creates the widgets defined in and adds
    them as property to a given object. This internally calls retrieveWidgets,
    so signals from widgets are connected by name to obj.
    @param obj: The object which will act as parent of the loaded ui
    (this object will receive a new layout)
    @type obj: QWidget
    @param uipath: the path to the Qt designer file
    @type uipath: str
    @param widgets: optional argument; a recursive (parent-relation of widgets)
    list of tuples, defining which widgets should be added as attributes to
    obj. See retrieveWidgets for details.
    If you omit this or pass None, recursively all child widgets will be stored
    @type widgets: list[tuple(str, bool, list(...))] or None
    @param seticons: if True, widgets containing a string-property
    called 'pytsonicon' will get the icon of a soundpack
    (value of property = variable in soundpack)
    @type seticons: bool
    @param iconpack: if set, the iconpack will be used, if None, the current
    iconpack is used
    @type iconpack: ts3client.IconPack
    @param pluginicons: callable which gets a string and either returns the
    path to the image file or returns a QPixmap to set the icon property to;
    defaults to None
    @type pluginicons: Callable(str) -> str or QIcon
    @param extraWidgets: list of extra classes to be created, there must be a
    constructor which takes only the parent object
    @type extraWidgets: list(QWidget type)
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
            ts3print(pytson.tr("pytsonui", "Error loading iconpack: "
                     "{exception}").format(exception=e),
                     ts3defines.LogLevel.LogLevel_ERROR,
                     "pytsonui.setupUi.%s" % obj.objectName, 0)

    if os.path.isfile(uipath):
        f = QFile(uipath)
        if f.open(QIODevice.ReadOnly):
            loader = UiLoader(obj, extraWidgets=extraWidgets)
            ui = loader.load(f)
            f.close()

            if not ui:
                raise Exception("Error creating widget from uifile: %s" %
                                loader.errorString())
        else:
            raise Exception("Could not open uifile")
    else:
        raise Exception("Could not find uifile")

    if widgets:
        retrieveWidgets(obj, ui, widgets, seticons, iconpack, pluginicons)
    else:
        retrieveAllWidgets(obj, ui, seticons, iconpack, pluginicons)

    if hasattr(obj, "parent") and obj.parent():
        par = obj.parent()
        obj.move(par.window().frameGeometry.topLeft() +
                 par.window().rect.center() - obj.rect.center())

    if root:
        iconpack.close()
