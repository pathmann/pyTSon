import os

from . import _errprint

import ts3lib
from ts3defines import ERROR_ok, ERROR_database_empty_result, FileListType
import pytson
from pytsonui import setupUi

from pluginhost import PluginHost
from signalslot import Signal

from PythonQt.QtCore import (Qt, QAbstractItemModel, QModelIndex)
from PythonQt.QtGui import (QDialog, QStyledItemDelegate, QIcon,
                            QSortFilterProxyModel)


class File(object):
    """

    """

    def __init__(self, path, name, size, datetime, atype, incompletesize):
        self.path = path
        self.name = name
        self.size = size
        self.datetime = datetime
        self.type = atype
        self.incompletesize = incompletesize

    @property
    def isDirectory(self):
        return self.type == FileListType.FileListType_Directory

    @property
    def icon(self):
        if self.isDirectory:
            return QIcon.fromTheme("folder")
        else:
            ext = os.path.splitext(self.name)[-1][1:].lower()
            theme = None

            if ext in ['exe', 'dll', 'bat', 'dylib', 'sh', 'run']:
                theme = "application-x-executable"
            elif ext in ['mp3', 'ogg', 'wav', 'wma', 'flac']:
                theme = "audio-x-generic"
            elif ext in ['ttf', 'woff', 'eot']:
                theme = "font-x-generic"
            elif ext in ['png', 'jpg', 'jpeg', 'gif', 'svg', ' bmp']:
                theme = "image-x-generic"
            elif ext in ['avi', 'wmv', 'qt', 'mkv', 'flv', 'mpg', 'ram', 'mov',
                         'mp4']:
                theme = "video-x-generic"

            if not theme:
                theme = "text-x-generic"

            return QIcon.fromTheme(theme)


class FileListModel(QAbstractItemModel, pytson.Translatable):
    """

    """

    titles = ["Name"]

    def __init__(self, schid, cid, password, parent=None):
        """

        """
        super(QAbstractItemModel, self).__init__(parent)

        self.schid = schid
        self.cid = cid
        self.password = password

        self.pathChanged = Signal()

        self._path = None
        self.newpath = None
        self.files = []
        self.newfiles = []

        PluginHost.registerCallbackProxy(self)

    def __del__(self):
        """

        """
        PluginHost.unregisterCallbackProxy(self)

    @property
    def path(self):
        return self._path

    @path.setter
    def path(self, val):
        self._reload(val)

    def _reload(self, path=None):
        if path:
            self.newpath = path
        else:
            self.newpath = self._path

        self.retcode = ts3lib.createReturnCode()
        err = ts3lib.requestFileList(self.schid, self.cid, self.password,
                                     self.newpath, self.retcode)
        if err != ERROR_ok:
            _errprint(self._tr("Error requesting filellist"), err, self.schid,
                      self.cid)

    def onFileListEvent(self, schid, channelID, path, name, size, datetime,
                        atype, incompletesize, returnCode):
        if (schid != self.schid or channelID != self.cid or
           returnCode != self.retcode):
            return

        self.newfiles.append(File(path, name, size, datetime, atype,
                                  incompletesize))

    def onFileListFinishedEvent(self, schid, channelID, path):
        if (schid != self.schid or channelID != self.cid or
           path != self.newpath):
            return
        print("fini %s" % path)

    def onServerErrorEvent(self, schid, errorMessage, error, returnCode,
                           extraMessage):
        if schid != self.schid or returnCode != self.retcode:
            return

        if error in [ERROR_ok, ERROR_database_empty_result]:
            self.beginResetModel()
            self.files = self.newfiles
            self.newfiles = []
            self.endResetModel()

            if self._path != self.newpath:
                self._path = self.newpath
                self.pathChanged.emit(self._path)

    def headerData(self, section, orientation, role=Qt.DisplayRole):
        if role == Qt.DisplayRole:
            return self.titles[section]

        return None

    def index(self, row, column, parent=QModelIndex()):
        if parent.isValid():
            return QModelIndex()

        return self.createIndex(row, column)

    def parent(self, idx):
        return QModelIndex()

    def rowCount(self, parent=QModelIndex()):
        if parent.isValid():
            return 0

        return len(self.files)

    def columnCount(self, parent=QModelIndex()):
        return len(self.titles)

    def data(self, idx, role=Qt.DisplayRole):
        if not idx.isValid():
            return None

        f = self.files[idx.row()]
        if idx.column() == 0:
            if role == Qt.DisplayRole:
                return f.name
            elif role == Qt.DecorationRole:
                return f.icon

        if role == Qt.UserRole:
            if f.isDirectory:
                return "a%s" % f.name
            else:
                return "b%s" % f.name

        return None


class FileBrowser(QDialog):
    """

    """

    def __init__(self, schid, cid, password, path, parent=None, *,
                 staticpath=False, readonly=False, iconpack=None):
        """

        """
        super().__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)

        try:
            setupUi(self, pytson.getPluginPath("ressources", "filebrowser.ui"),
                    iconpack=iconpack)
        except Exception as e:
            self.delete()
            raise e

        self.filterFrame.hide()

        self.iconButton.setChecked(True)
        self.stack.setCurrentWidget(self.listPage)

        self.cid = cid
        self.password = password

        self.staticpath = staticpath
        self.readonly = readonly

        self.listmodel = FileListModel(schid, cid, password, self)
        self.listmodel.pathChanged.connect(self.onPathChanged)

        self.proxy = QSortFilterProxyModel(self)
        self.proxy.setSortRole(Qt.UserRole)
        self.proxy.setSortCaseSensitivity(Qt.CaseInsensitive)
        self.proxy.setFilterCaseSensitivity(Qt.CaseInsensitive)
        self.proxy.setSourceModel(self.listmodel)

        self.listmodel.path = path

        self.list.setModel(self.proxy)
        self.table.setModel(self.proxy)
        self.table.sortByColumn(0, Qt.AscendingOrder)

    def onPathChanged(self, newpath):
        self.pathEdit.setText(newpath)

    def on_pathEdit_returnPressed(self):
        oldpath = self.listmodel.path
        if not self.readonly:
            self.listmodel.path = self.pathEdit.text

        self.pathEdit.text = oldpath

    def on_iconButton_toggled(self, act):
        if act:
            self.stack.setCurrentWidget(self.listPage)

    def on_detailedButton_toggled(self, act):
        if act:
            self.stack.setCurrentWidget(self.tablePage)

    def on_filterButton_clicked(self):
        self.filterFrame.show()

    def on_clearButton_clicked(self):
        self.filterEdit.clear()
        self.filterFrame.hide()

    def on_filterEdit_textChanged(self, newtext):
        self.proxy.setFilterRegExp(newtext)


class FileCollisionAction(object):
    overwrite = 1
    resume = 2
    toall = 4


class FileCollisionDialog(QDialog):
    """

    """
    pass


class FileTransfer(object):
    """

    """
    pass


class FileTransferModel(QAbstractItemModel):
    """

    """
    pass


class FileTransferDelegate(QStyledItemDelegate):
    """

    """
    pass


class FileTransferDialog(QDialog):
    """

    """
    pass
