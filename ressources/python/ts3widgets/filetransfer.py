import os

from . import _errprint

import ts3lib
from ts3defines import (FileListType, ERROR_ok, ERROR_database_empty_result,
                        ERROR_file_transfer_complete, ChannelProperties)
import pytson
from pytsonui import setupUi
import ts3client

from pluginhost import PluginHost
from signalslot import Signal

from PythonQt.QtCore import (Qt, QAbstractItemModel, QModelIndex, QUrl)
from PythonQt.QtGui import (QDialog, QStyledItemDelegate, QIcon, QHeaderView,
                            QSortFilterProxyModel, QFileDialog, QLineEdit,
                            QInputDialog, QStatusBar, QMessageBox,
                            QStyleOptionProgressBar, QApplication, QStyle,
                            QDesktopServices, QMenu)
from PythonQt import BoolResult

from datetime import datetime

from collections import OrderedDict


def splitpath(path):
    """
    Splits a TS3 filepath into its sections.
    @param path: the path to split
    @type path: str
    @return: the list of sections
    @rtype: list[str]
    """
    return ["/"] + list(filter(None, path.split('/')))


def joinpath(*args):
    """
    Joins multiple sections into a TS3 filepath.
    @param args: sections to join
    @type args: tuple(str)
    @return: the resulting path
    @rtype: str
    """
    return "/" + "/".join(filter(lambda x: x not in ["/", ""], args))


def bytesToStr(size):
    """
    Creates a human readable string of a number of bytes.
    @param size: number of bytes
    @type size: int
    @return: the converted size and most fitting unit
    @rtype: str
    """
    bias = 1024.0
    units = ['B', 'KiB', 'MiB', 'GiB', 'TiB']

    for u in units:
        if abs(size) < bias:
            return "%3.2f %s" % (size, u)
        size /= bias

    return "%3.2f PiB" % size


class File(object):
    """
    Container class to hold all information on a remote TS3 file.
    """

    def __init__(self, path, name, size, date, atype, incompletesize):
        self.path = path
        self.name = name
        self.size = size
        self.datetime = datetime.fromtimestamp(date)
        self.type = atype
        self.incompletesize = incompletesize

    @property
    def isDirectory(self):
        return self.type == FileListType.FileListType_Directory

    @property
    def icon(self):
        """
        Returns the most fitting icon for the file
        @return: the icon
        @rtype: QIcon
        """
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

    @property
    def fullpath(self):
        return joinpath(self.path, self.name)


class FileListModel(QAbstractItemModel, pytson.Translatable):
    """
    Itemmodel to abstract the files contained on a TS3 filepath.
    """

    def __init__(self, schid, cid, password, parent=None, *, readonly=False):
        super(QAbstractItemModel, self).__init__(parent)

        self.schid = schid
        self.cid = cid
        self.password = password

        self.readonly = readonly

        self.pathChanged = Signal()
        self.error = Signal()

        self._path = None
        self.newpath = None
        self.files = []
        self.newfiles = []

        self.retcode = None
        self.renretcode = None
        self.renfile = ()

        self.titles = [self._tr("Name"), self._tr("Size"), self._tr("Type"),
                       self._tr("Last Changed")]

        PluginHost.registerCallbackProxy(self)

    def __del__(self):
        PluginHost.unregisterCallbackProxy(self)

    @property
    def path(self):
        return self._path

    @path.setter
    def path(self, val):
        self._reload(val)

    @property
    def currentFiles(self):
        return self.files

    def _reload(self, path=None):
        if path:
            self.newpath = path
        else:
            self.newpath = self._path

        self.retcode = ts3lib.createReturnCode()
        err = ts3lib.requestFileList(self.schid, self.cid, self.password,
                                     self.newpath, self.retcode)
        if err != ERROR_ok:
            _errprint(self._tr("Error requesting filelist"), err, self.schid,
                      self.cid)

    def onFileListEvent(self, schid, channelID, path, name, size, date,
                        atype, incompletesize, returnCode):
        if (schid != self.schid or channelID != self.cid or
           returnCode != self.retcode):
            return

        self.newfiles.append(File(path, name, size, date, atype,
                                  incompletesize))

    def onFileListFinishedEvent(self, schid, channelID, path):
        if (schid != self.schid or channelID != self.cid or
           path != self.newpath):
            return
        # might be unneeded (event is catched in onServerErrorEvent)

    def onServerErrorEvent(self, schid, errorMessage, error, returnCode,
                           extraMessage):
        if schid != self.schid:
            return

        if returnCode == self.retcode:
            if error in [ERROR_ok, ERROR_database_empty_result]:
                self.beginResetModel()
                self.files = self.newfiles
                self.newfiles = []
                self.endResetModel()

                if self._path != self.newpath:
                    self._path = self.newpath
                    self.pathChanged.emit(self._path)
            else:
                self.error.emit(self._tr("Error requesting filelist"), error,
                                errorMessage)
        elif returnCode == self.renretcode:
            if error != ERROR_ok:
                self.renfile[0].name = self.renfile[1]

                self.error.emit(self._tr("Error renaming file"), error,
                                errorMessage)

            self.renfile = ()

    def onServerPermissionErrorEvent(self, schid, errorMessage, error,
                                     returnCode, failedPermissionID):
        if schid != self.schid or returnCode != self.retcode:
            return

        if returnCode == self.retcode:
            if error != ERROR_ok:
                self.error.emit(self._tr("Error requesting filelist"), error,
                                errorMessage)
        elif returnCode == self.renretcode:
            if error != ERROR_ok:
                self.renfile[0].name = self.renfile[1]

                self.error.emit(self._tr("Error renaming file"), error,
                                errorMessage)

            self.renfile = ()

    def headerData(self, section, orientation, role=Qt.DisplayRole):
        if role == Qt.DisplayRole and orientation == Qt.Horizontal:
            return self.titles[section]

        return None

    def flags(self, idx):
        f = Qt.ItemIsEnabled | Qt.ItemIsSelectable

        if not self.readonly:
            return f | Qt.ItemIsEditable
        else:
            return f

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
            elif role == Qt.EditRole and not self.readonly:
                return f.name
            elif role == Qt.UserRole:
                if f.isDirectory:
                    return "a%s" % f.name
                else:
                    return "b%s" % f.name
        elif role == Qt.DisplayRole:
            if idx.column() == 1 and not f.isDirectory:
                return bytesToStr(f.size)
            elif idx.column() == 2:
                if f.isDirectory:
                    return self._tr("Directory")
                else:
                    return self._tr("File")
            elif idx.column() == 3:
                return f.datetime.strftime(pytson.tr("filetransfer",
                                                     "%Y-%m-%d %H:%M:%S"))
        return None

    def setData(self, idx, value, role=Qt.EditRole):
        if not idx.isValid():
            return False

        f = self.fileByIndex(idx)
        if value == f.name:
            return

        self.renretcode = ts3lib.createReturnCode()
        self.renfile = (f, f.name)

        err = ts3lib.requestRenameFile(self.schid, self.cid, self.password,
                                       0, "", f.fullpath, joinpath(f.path,
                                                                   value),
                                       self.renretcode)

        if err == ERROR_ok:
            f.name = value
            return True

    def fileByIndex(self, idx):
        if idx.isValid():
            return self.files[idx.row()]
        return None


class SmartStatusBar(QStatusBar):
    """
    StatusBar which automatically hides itsself, when the message is cleared.
    """

    # default Timeout of messages
    defaultTimeout = 5000

    def __init__(self, parent=None):
        super(SmartStatusBar, self).__init__(parent)

        self.connect("messageChanged(QString)", self._onMessageChanged)

    def _onMessageChanged(self, message):
        if message == "":
            self.hide()

    def showMessage(self, message, timeout=0):
        """
        Displays a message for a specified duration.
        @param message: the message to display
        @type message: str
        @param timeout: duration in ms; optional; if set to 0,
        SmartStatusBar.defaultTimeout is used
        @type timeout: int
        """
        self.show()
        if timeout == 0:
            timeout = self.defaultTimeout

        QStatusBar.showMessage(self, message, timeout)


class FileCollector(pytson.Translatable):
    """
    Collects all files recursively from TS3 filetransfer directories with their
    corresponding download path.
    Emits a signal collectionFinished with a list of tuples(str, list[File])
    containing the download dir and a list of files.
    The signal collectionError(str, int) is emitted on error with the
    errorstring and the errorcode.
    """

    def __init__(self, schid, cid, password, rootdir):
        """
        Instantiates a new object.
        @param schid: the id of the serverconnection handler
        @type schid: int
        @param cid: the id of the channel
        @type cid: int
        @param password: the password of the channel
        @type password: str
        @param rootdir: the root download directory
        @type rootdir: str
        """
        super().__init__()

        self.schid = schid
        self.cid = cid
        self.password = password
        self.rootdir = rootdir

        self.collectionFinished = Signal()
        self.collectionError = Signal()

        self.queue = {}
        self.files = {}

        PluginHost.registerCallbackProxy(self)

    def __del__(self):
        PluginHost.unregisterCallbackProxy(self)

    def addFiles(self, files):
        """
        Manually adds a list of files to the collection (emitted with the
        rootdir)
        @param files: list of files to emit
        @type files: list(File)
        """
        self.files[self.rootdir] = files

    def collect(self, dirs):
        """
        Starts collecting files from a list of directories
        @param dirs: list of directories
        @type dirs: list(File)
        """
        for d in dirs:
            retcode = ts3lib.createReturnCode()
            self.queue[retcode] = d.fullpath

            err = ts3lib.requestFileList(self.schid, self.cid, self.password,
                                         d.fullpath, retcode)

            if err != ERROR_ok:
                del self.queue[retcode]
                self.collectionError.emit(self._tr("Error requesting "
                                                   "filelist of {dirname}").
                                          format(dirname=d.fullpath), err)

    def onServerErrorEvent(self, schid, errorMessage, error, returnCode,
                           extraMessage):
        if schid != self.schid or returnCode not in self.queue:
            return

        if error not in [ERROR_ok, ERROR_database_empty_result]:
            self.collectionError.emit(self._tr("Error requesting filelist "
                                               "of {dirname}").
                                      format(dirname=self.queue[returnCode]),
                                      error)

        del self.queue[returnCode]

        if not self.queue:
            self.collectionFinished.emit([(k, v)
                                          for k, v in self.files.items()])
            self.files = {}

    def onFileListEvent(self, schid, channelID, path, name, size, datetime,
                        atype, incompletesize, returnCode):
        if (schid != self.schid or self.cid != channelID or
           returnCode not in self.queue):
            return

        downpath = os.path.join(self.rootdir, *splitpath(path)[1:])
        f = File(path, name, size, datetime, atype, incompletesize)

        if f.isDirectory:
            self.collect([f])
        else:
            if downpath in self.files:
                self.files[downpath].append(f)
            else:
                self.files[downpath] = [f]


class FileBrowser(QDialog, pytson.Translatable):
    """
    Dialog to display files contained on a TS3 filepath.
    """

    def __init__(self, schid, cid, password='', path='/', parent=None, *,
                 staticpath=False, readonly=False, downloaddir=None,
                 iconpack=None):
        """
        Instantiates a new object.
        @param schid: the id of the serverconnection handler
        @type schid: int
        @param cid: the id of the channel
        @type cid: int
        @param password: password to the channel, defaults to an empty string
        @type password: str
        @param path: path to display, defaults to the root path
        @type path: str
        @param parent: parent of the dialog; optional keyword arg;
        defaults to None
        @type parent: QWidget
        @param staticpath: if set to True, the initial path can't be
        changed by the user; optional keyword arg; defaults to False
        @type staticpath: bool
        @param readonly: if set to True, the user can't download, upload
        or delete files, or create new directories; optional keyword arg;
        defaults to False
        @type readonly: bool
        @param downloaddir: directory to download files to; optional keyword
        arg; defaults to None; if set to None, the TS3 client's download
        directory is used
        @type downloaddir: str
        @param iconpack: iconpack to load icons from; optional keyword arg;
        defaults to None; if set to None, the current iconpack is used
        @type iconpack: ts3client.IconPack
        """
        super(QDialog, self).__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)

        iconpackopened = False
        if not iconpack:
            try:
                iconpack = ts3client.IconPack.current()
                iconpack.open()
                iconpackopened = True
            except Exception as e:
                self.delete()
                raise e

        try:
            setupUi(self, pytson.getPluginPath("ressources", "filebrowser.ui"),
                    iconpack=iconpack)

            self.statusbar = SmartStatusBar(self)
            self.layout().addWidget(self.statusbar)
            self.statusbar.hide()
        except Exception as e:
            self.delete()
            raise e

        err, cname = ts3lib.getChannelVariableAsString(schid, cid,
                                                       ChannelProperties.
                                                       CHANNEL_NAME)

        if err == ERROR_ok:
            self.setWindowTitle(self._tr("File Browser - {cname}").format(
                                cname=cname))
        else:
            self.setWindowTitle(self._tr("File Browser"))

        self.schid = schid
        self.cid = cid
        self.password = password
        self.path = None

        self.staticpath = staticpath
        self.readonly = readonly

        self.createretcode = None
        self.delretcode = None

        if not self.readonly and not downloaddir:
            cfg = ts3client.Config()
            q = cfg.query("SELECT value FROM filetransfer "
                          "WHERE key='DownloadDir'")
            del cfg

            if q.next():
                self.downloaddir = q.value("value")
            else:
                self.delete()
                raise Exception("Error getting DownloadDir from config")
        else:
            self.downloaddir = downloaddir

        if not self.readonly:
            menu = self.menu = QMenu(self)

            self.openAction = menu.addAction(QIcon(iconpack.icon("FILE_UP")),
                                             self._tr("Open"))
            self.openAction.connect("triggered()",
                                    self.on_openAction_triggered)

            self.downAction = menu.addAction(QIcon(iconpack.icon("DOWN")),
                                             self._tr("Download"))
            self.downAction.connect("triggered()", self.downloadFiles)
            self.renameAction = menu.addAction(QIcon(iconpack.icon("EDIT")),
                                               self._tr("Rename"))
            self.renameAction.connect("triggered()",
                                      self.on_renameAction_triggered)
            self.copyAction = menu.addAction(QIcon(iconpack.icon("COPY")),
                                             self._tr("Copy URL"))
            self.copyAction.connect("triggered()",
                                    self.on_copyAction_triggered)
            self.delAction = menu.addAction(QIcon(iconpack.icon("DELETE")),
                                            self._tr("Delete"))
            self.delAction.connect("triggered()", self.deleteFiles)

            self.upAction = menu.addAction(QIcon(iconpack.icon("UP")),
                                           self._tr("Upload files"))
            self.upAction.connect("triggered()", self.uploadFiles)
            self.createAction = menu.addAction(QIcon.fromTheme("folder"),
                                               self._tr("Create Folder"))
            self.createAction.connect("triggered()", self.createFolder)
            self.refreshAction = menu.addAction(QIcon(iconpack.icon(
                                                "FILE_REFRESH")),
                                                self._tr("Refresh"))
            self.refreshAction.connect("triggered()", self.refresh)

            self.allactions = [self.openAction, self.downAction,
                               self.renameAction, self.copyAction,
                               self.delAction, self.upAction,
                               self.createAction, self.refreshAction]

        self.collector = FileCollector(schid, cid, password, self.downloaddir)
        self.collector.collectionFinished.connect(self._startDownload)
        self.collector.collectionError.connect(self.showError)

        self.fileDoubleClicked = Signal()
        self.contextMenuRequested = Signal()

        self.transdlg = None

        self.listmodel = FileListModel(schid, cid, password, self,
                                       readonly=readonly)
        self.listmodel.pathChanged.connect(self.onPathChanged)
        self.listmodel.error.connect(self.showError)

        self.proxy = QSortFilterProxyModel(self)
        self.proxy.setSortRole(Qt.UserRole)
        self.proxy.setSortCaseSensitivity(Qt.CaseInsensitive)
        self.proxy.setFilterCaseSensitivity(Qt.CaseInsensitive)
        self.proxy.setSourceModel(self.listmodel)

        self.listmodel.path = path

        self._adjustUi()

        if iconpackopened:
            iconpack.close()

        PluginHost.registerCallbackProxy(self)

    def __del__(self):
        PluginHost.unregisterCallbackProxy(self)

    def _enableMenus(self, actlist):
        for act in self.allactions:
            act.setVisible(act in actlist)

    def _adjustMenu(self):
        selfiles = self.selectedFiles()
        cur = self.listmodel.fileByIndex(self.currentItem())

        if len(selfiles) == 0:
            self._enableMenus([self.upAction, self.createAction,
                               self.refreshAction])
        elif cur.isDirectory:
            self._enableMenus([self.openAction, self.downAction,
                               self.renameAction, self.copyAction,
                               self.delAction])
        else:
            self._enableMenus([self.downAction, self.renameAction,
                               self.copyAction, self.delAction])

    def _adjustUi(self):
        self.filterFrame.hide()

        self.filecountLabel.hide()

        self.downloaddirButton.setText(self.downloaddir)

        self.iconButton.setChecked(True)
        self.stack.setCurrentWidget(self.listPage)

        self.list.setModel(self.proxy)
        self.table.setModel(self.proxy)
        self.table.sortByColumn(0, Qt.AscendingOrder)

        if self.staticpath:
            self.upButton.hide()
            self.homeButton.hide()

        if self.readonly:
            self.uploadButton.hide()
            self.downloadButton.hide()
            self.directoryButton.hide()
            self.deleteButton.hide()

            self.downloaddirLabel.hide()
            self.downloaddirButton.hide()

        header = self.table.horizontalHeader()
        header.setSectionResizeMode(0, QHeaderView.Stretch)
        for i in range(1, header.count()):
            header.setSectionResizeMode(i, QHeaderView.ResizeToContents)

        self.refreshButton.connect("clicked()", self.refresh)
        self.uploadButton.connect("clicked()", self.uploadFiles)
        self.downloadButton.connect("clicked()", self.downloadFiles)
        self.deleteButton.connect("clicked()", self.deleteFiles)
        self.directoryButton.connect("clicked()", self.createFolder)
        self.list.connect("doubleClicked(QModelIndex)", self.viewDoubleClicked)
        self.table.connect("doubleClicked(QModelIndex)",
                           self.viewDoubleClicked)

    def _showTransfers(self):
        if not self.transdlg:
            self.transdlg = FileTransferDialog(self.schid, self.cid,
                                               self.password, self)
            self.transdlg.show()

    def onPathChanged(self, newpath):
        self.path = newpath
        self.pathEdit.setText(newpath)

        inroot = newpath == "/"
        self.upButton.setEnabled(not inroot)
        self.homeButton.setEnabled(not inroot)

        files = self.listmodel.currentFiles

        if not files:
            self.filecountLabel.hide()
        else:
            self.filecountLabel.show()

            fcount = 0
            dcount = 0

            for f in files:
                if f.isDirectory:
                    dcount += 1
                else:
                    fcount += 1

            fstr = self._tr("{filecount} file(s)", n=fcount).format(
                            filecount=fcount)
            dstr = self._tr("{dircount} directory(s)", n=dcount).format(
                            dircount=dcount)

            if dcount == 0:
                self.filecountLabel.setText(fstr)
            elif fcount == 0:
                self.filecountLabel.setText(dstr)
            else:
                cstr = self._tr("{dircountstr} and {fcountstr}").format(
                                dircountstr=dstr, fcountstr=fstr)
                self.filecountLabel.setText(cstr)

    def on_pathEdit_returnPressed(self):
        oldpath = self.listmodel.path
        if not self.readonly:
            self.listmodel.path = self.pathEdit.text

        self.pathEdit.text = oldpath or ""

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

    def on_upButton_clicked(self):
        if self.staticpath:
            return
        if self.path == "/":
            return

        self.listmodel.path = joinpath(*splitpath(self.path)[:-1])

    def on_homeButton_clicked(self):
        if self.staticpath:
            return

        self.listmodel.path = "/"

    def refresh(self):
        self.listmodel.path = self.listmodel.path

    def on_downloaddirButton_clicked(self):
        QDesktopServices.openUrl(QUrl(self.downloaddir))

    def showError(self, prefix, errcode, msg=None):
        if not msg:
            err, msg = ts3lib.getErrorMessage(errcode)
        else:
            err = ERROR_ok

        if err != ERROR_ok:
            self.statusbar.showMessage("%s: %s" % (prefix, errcode))
        else:
            self.statusbar.showMessage("%s: %s" % (prefix, msg))

    def uploadFiles(self):
        if self.readonly:
            return

        files = QFileDialog.getOpenFileNames(self, self._tr("Upload files"),
                                             self.downloaddir)

        fca = FileCollisionAction.overwrite
        curfiles = {f.name: f for f in self.listmodel.currentFiles}
        for f in files:
            fname = os.path.split(f)[-1]
            if fname in curfiles:
                if not fca & FileCollisionAction.toall:
                    fca = FileCollisionDialog.getAction(f, curfiles[fname],
                                                        False, len(files) > 1,
                                                        self)

                if fca == 0:
                    return

                if fca & FileCollisionAction.skip:
                    if not fca & FileCollisionAction.toall:
                        fca = FileCollisionAction.overwrite
                    break

            self._showTransfers()
            self.transdlg.addUpload(self.path, f,
                                    fca & FileCollisionAction.overwrite,
                                    fca & FileCollisionAction.resume)

            if not fca & FileCollisionAction.toall:
                fca = FileCollisionAction.overwrite

    def onServerErrorEvent(self, schid, errorMessage, error, returnCode,
                           extraMessage):
        if schid != self.schid:
            return

        if returnCode == self.createretcode:
            if error == ERROR_ok:
                self.listmodel.path = self.path
            else:
                self.showError(self._tr("Error creating directory"), error,
                               errorMessage)
        elif returnCode == self.delretcode:
            if error == ERROR_ok:
                self.listmodel.path = self.path
            else:
                self.showError(self._tr("Error deleting files"), error,
                               errorMessage)

    def selectedFiles(self):
        if self.stack.currentWidget() == self.listPage:
            view = self.list
        else:
            view = self.table

        return [self.listmodel.fileByIndex(self.proxy.mapToSource(x))
                for x in view.selectionModel().selectedIndexes]

    def currentItem(self, source=True):
        if self.stack.currentWidget() == self.listPage:
            view = self.list
        else:
            view = self.table

        if source:
            return self.proxy.mapToSource(view.currentIndex())
        else:
            return view.currentIndex()

    def _startDownload(self, collection):
        """
        @param collection: list of tuples containing the download directory and
        the list of files to download to that directory
        @type collection: list[tuple(str, list[File])]
        """
        if not collection:
            return

        fca = FileCollisionAction.overwrite

        for (downdir, files) in collection:
            for f in files:
                multi = len(files) + len(collection) > 2
                fname = os.path.join(downdir, f.name)
                if os.path.isfile(fname):
                    if not fca & FileCollisionAction.toall:
                        fca = FileCollisionDialog.getAction(fname, f, True,
                                                            multi, self)

                    if fca == 0:
                        return

                    if fca & FileCollisionAction.skip:
                        if not fca & FileCollisionAction.toall:
                            fca = FileCollisionAction.overwrite
                        break

                self._showTransfers()
                self.transdlg.addDownload(f, downdir,
                                          fca & FileCollisionAction.overwrite,
                                          fca & FileCollisionAction.resume)

                if not fca & FileCollisionAction.toall:
                    fca = FileCollisionAction.overwrite

    def downloadFiles(self, files=None):
        if self.readonly:
            return

        if not files:
            selfiles = self.selectedFiles()
        else:
            selfiles = files

        if not selfiles:
            return

        downfiles = []
        downdirs = []

        for f in selfiles:
            if f.isDirectory:
                downdirs.append(f)
            else:
                downfiles.append(f)

        if not downdirs:
            self._startDownload([(self.downloaddir, downfiles)])
        else:
            if downfiles:
                self.collector.addFiles(downfiles)

            self.collector.collect(downdirs)

    def createFolder(self):
        if self.readonly:
            return

        ok = BoolResult()
        dirname = QInputDialog.getText(self, self._tr("Create Folder"),
                                       self._tr("Folder name:"),
                                       QLineEdit.Normal, "", ok)

        if not ok or dirname == "":
            return

        self.createretcode = ts3lib.createReturnCode()
        err = ts3lib.requestCreateDirectory(self.schid, self.cid,
                                            self.password,
                                            joinpath(self.path, dirname),
                                            self.createretcode)

        if err != ERROR_ok:
            self.showError(self._tr("Error creating directory"), err)

    def deleteFiles(self, files=None):
        if self.readonly:
            return

        if not files:
            selfiles = self.selectedFiles()
        else:
            selfiles = files

        if not selfiles:
            return

        if QMessageBox.question(self, self._tr("Delete files"),
                                self._tr("Do you really want to delete all "
                                         "selected files?")) == QMessageBox.No:
            return

        pathes = [f.fullpath for f in selfiles]
        self.delretcode = ts3lib.createReturnCode()
        err = ts3lib.requestDeleteFile(self.schid, self.cid, self.password,
                                       pathes, self.delretcode)

        if err != ERROR_ok:
            self.showError(self._tr("Error deleting files"), err)

    def on_table_customContextMenuRequested(self, pos):
        selfiles = self.selectedFiles()
        globpos = self.table.mapToGlobal(pos)

        if self.readonly:
            self.contextMenuRequested.emit(selfiles, globpos)
        else:
            self._adjustMenu()
            self.menu.popup(globpos)

    def on_list_customContextMenuRequested(self, pos):
        selfiles = self.selectedFiles()
        globpos = self.list.mapToGlobal(pos)

        if self.readonly:
            self.contextMenuRequested.emit(selfiles, globpos)
        else:
            self._adjustMenu()
            self.menu.popup(globpos)

    def viewDoubleClicked(self, idx):
        if not idx.isValid():
            return

        f = self.listmodel.fileByIndex(self.proxy.mapToSource(idx))
        if f.isDirectory:
            if self.staticpath:
                self.fileDoubleClicked.emit(f)
            else:
                self.listmodel.path = f.fullpath
        else:
            if self.readonly:
                self.fileDoubleClicked.emit(f)
            else:
                self.downloadFiles([f])

    def on_openAction_triggered(self):
        cur = self.listmodel.fileByIndex(self.currentItem())

        if not cur or not cur.isDirectory:
            return

        self.listmodel.path = cur.fullpath

    def on_renameAction_triggered(self):
        if self.stack.currentWidget() == self.listPage:
            view = self.list
        else:
            view = self.table

        view.edit(self.currentItem(False))

    def on_copyAction_triggered(self):
        cur = self.listmodel.fileByIndex(self.currentItem())

        if not cur:
            return

        err, host, port, _ = ts3lib.getServerConnectInfo(self.schid)

        if err == ERROR_ok:
            url = ("[URL=ts3file://{address}?port={port}&channel={cid}&"
                   "path={path}&filename={fname}&isDir={isdir}&"
                   "size={size}&fileDateTime={date}]{fname}[/URL]").format(
                   address=host, port=port, cid=self.cid,
                   path=QUrl.toPercentEncoding(cur.path), fname=cur.name,
                   isdir=1 if cur.isDirectory else 0, size=cur.size,
                   date=int(cur.datetime.timestamp()))

            QApplication.clipboard().setText(url)
        else:
            self.showError(self._tr("Error getting server connection info"),
                           err)


class FileCollisionAction(object):
    overwrite = 1
    resume = 2
    skip = 4
    toall = 8


class FileCollisionDialog(QDialog, pytson.Translatable):
    """
    Dialog to inform about a filecollision and requests input how to handle it.
    """

    @classmethod
    def getAction(cls, localfile, remotefile, isdownload, multi, parent=None):
        """
        Convenience function to execute (blocks) the dialog.
        @param localfile: the path to the local file
        @type localfile: str
        @param remotefile: the remote file
        @type remotefile: File
        @param isdownload: set to True if remotefile should be downloaded
        @type isdownload: bool
        @param multi: set to True, if there are multiple files which could
        collide
        @type multi: bool
        @param parent: parent widget of the dialog; optional; defaults to None
        @type parent: QWidget
        """
        dlg = cls(localfile, remotefile, isdownload, multi, parent)
        return dlg.exec_()

    def __init__(self, localfile, remotefile, isdownload, multi, parent=None):
        """
        Instantiates a new dialog.
        @param localfile: the path to the local file
        @type localfile: str
        @param remotefile: the remote file
        @type remotefile: File
        @param isdownload: set to True if remotefile should be downloaded
        @type isdownload: bool
        @param multi: set to True, if there are multiple files which could
        collide
        @type multi: bool
        @param parent: parent widget of the dialog; optional; defaults to None
        @type parent: QWidget
        """
        super(QDialog, self).__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)

        try:
            setupUi(self, pytson.getPluginPath("ressources",
                                               "filecollision.ui"))
        except Exception as e:
            self.delete()
            raise e

        if not multi:
            self.multiButton.setVisible(False)
            self.skipallButton.setVisible(False)

        locsize = os.path.getsize(localfile)
        if locsize == remotefile.size:
            self.resumeButton.hide()

            self.actionLabel.text = self._tr("Do you want to overwrite the "
                                             "existing file")
        else:
            self.actionLabel.text = self._tr("Do you want to overwrite or "
                                             "resume the existing file")

        self.filenameLabel.text = "<b>%s</b>" % remotefile.name

        datefmt = pytson.tr("filetransfer", "%Y-%m-%d %H:%M:%S")
        locdate = datetime.fromtimestamp(os.path.getmtime(localfile))
        filefmt = "Size: <b>%s</b><br />Date: <b>%s</b>"

        locstr = filefmt % (bytesToStr(locsize), locdate.strftime(datefmt))
        remstr = filefmt % (bytesToStr(remotefile.size),
                            remotefile.datetime.strftime(datefmt))

        if isdownload:
            self.existingLabel.text = locstr
            self.newLabel.text = remstr
        else:
            self.existingLabel.text = remstr
            self.newLabel.text = locstr

        self.adjustSize()

    def on_overwriteButton_clicked(self):
        ret = FileCollisionAction.overwrite
        if self.multiButton.isChecked():
            ret = ret | FileCollisionAction.toall

        self.done(ret)

    def on_resumeButton_clicked(self):
        ret = FileCollisionAction.resume
        if self.multiButton.isChecked():
            ret = ret | FileCollisionAction.toall

        self.done(ret)

    def on_skipButton_clicked(self):
        ret = FileCollisionAction.skip
        if self.multiButton.isChecked():
            ret = ret | FileCollisionAction.toall

        self.done(ret)

    def on_skipallButton_clicked(self):
        self.done(FileCollisionAction.skip | FileCollisionAction.toall)

    def on_cancelButton_clicked(self):
        self.done(0)


class FileTransfer(pytson.Translatable):
    """
    Abstract container class to hold information on a filetransfer
    """

    def __init__(self, err, retcode):
        self.err = err
        self.retcode = retcode

        if err != ERROR_ok:
            errerr, msg = ts3lib.getErrorMessage(err)
            if errerr == ERROR_ok:
                self.errmsg = msg
            else:
                self.errmsg = self._tr("Unknown error {errcode}").format(
                                       errcode=err)

        self.size = 0

    def updateSize(self, val):
        self.size = val

    def updateError(self, err, msg=None):
        self.err = err
        if msg:
            self.errmsg = msg
        else:
            errerr, msg = ts3lib.getErrorMessage(err)
            if errerr == ERROR_ok:
                self.errmsg = msg
            else:
                self.errmsg = self._tr("Unknown error {errcode}").format(
                                       errcode=err)

    @property
    def progress(self):
        return 0

    @property
    def hasError(self):
        return self.err != ERROR_ok


class Download(FileTransfer, pytson.Translatable):
    """
    Container class to hold information on a download
    """

    def __init__(self, err, retcode, thefile, todir):
        super().__init__(err, retcode)

        self.file = thefile
        self.todir = todir

    @property
    def progress(self):
        if self.size == -1:
            return 100
        return round((self.size / self.file.size) * 100)

    @property
    def description(self):
        if self.hasError:
            return self._tr("Error downloading {filename}: {errmsg}").format(
                            filename=self.file.name, errmsg=self.errmsg)
        else:
            return self._tr("Downloading {filename}").format(
                            filename=self.file.name)

    @property
    def localpath(self):
        return os.path.join(self.todir, self.file.name)


class Upload(FileTransfer, pytson.Translatable):
    """
    Container class to hold information on an upload
    """

    def __init__(self, err, retcode, localfile):
        super().__init__(err, retcode)

        self.file = localfile
        self.completesize = os.path.getsize(localfile)

    @property
    def progress(self):
        if self.size == -1:
            return 100
        return round((self.size / self.completesize) * 100)

    @property
    def description(self):
        if self.hasError:
            return self._tr("Error uploading {filename}: {errmsg}").format(
                            filename=self.file, errmsg=self.errmsg)
        else:
            return self._tr("Uploading {filename}").format(filename=self.file)


class FileTransferModel(QAbstractItemModel, pytson.Translatable):
    """
    Itemmodel to abstract multiple filetransfers.
    """

    def __init__(self, schid, cid, password, parent=None):
        super().__init__(parent)

        self.schid = schid
        self.cid = cid
        self.password = password

        self.titles = [self._tr("Description"), self._tr("Progress")]

        self.transfers = OrderedDict()

        self.downcounter = 0
        self.timer = None

        PluginHost.registerCallbackProxy(self)

    def __del__(self):
        PluginHost.unregisterCallbackProxy(self)

    def timerEvent(self, event):
        for i, trans in enumerate(self.transfers.values()):
            if isinstance(trans, Download):
                if not trans.hasError and trans.progress != 100:
                    trans.updateSize(os.path.getsize(trans.localpath))
                    idx = self.createIndex(i, 1)
                    self.dataChanged(idx, idx)

    def addDownload(self, thefile, downloaddir, overwrite, resume):
        """
        Requests a download from the server and monitors its progress
        @param thefile: remote file to download
        @type thefile: File
        @param downloaddir: path to the download directory
        @type downloaddir: str
        @param overwrite: set to True to overwrite an existing file
        @type overwrite: bool
        @param resume: set to True to resume a previous download
        @type resume: bool
        @return: the filetransfer id
        @rtype: int
        """
        retcode = ts3lib.createReturnCode()
        err, ftid = ts3lib.requestFile(self.schid, self.cid, self.password,
                                       thefile.fullpath, overwrite, resume,
                                       downloaddir, retcode)

        self.beginInsertRows(QModelIndex(), len(self.transfers),
                             len(self.transfers))
        self.transfers[ftid] = Download(err, retcode, thefile, downloaddir)
        self.endInsertRows()

        self.downcounter += 1
        if self.downcounter == 1:
            self.timer = self.startTimer(500)

        return ftid

    def addUpload(self, path, localfile, overwrite, resume):
        """
        Requests an upload to the server.
        @param path: path to upload the file to
        @type path: str
        @param localfile: path to the file to upload
        @type localfile: str
        @param overwrite: set to True to overwrite an existing file
        @type overwrite: bool
        @param resume: set to True to resume a previous upload
        @type resume: bool
        @return: the filetransfer id
        @rtype: int
        """
        retcode = ts3lib.createReturnCode()
        err, ftid = ts3lib.sendFile(self.schid, self.cid, self.password,
                                    joinpath(path,
                                             os.path.split(localfile)[-1]),
                                    overwrite, resume,
                                    os.path.dirname(localfile), retcode)

        self.beginInsertRows(QModelIndex(), len(self.transfers),
                             len(self.transfers))
        self.transfers[ftid] = Upload(err, retcode, localfile)
        self.endInsertRows()

        return ftid

    def cleanup(self):
        """
        Cleanup finished and broken downloads
        """
        for i, ftid in enumerate(list(self.transfers)):
            trans = self.transfers[ftid]
            if trans.progress == 100 or trans.hasError:
                self.beginRemoveRows(QModelIndex(), i, i)
                del self.transfers[ftid]
                self.endRemoveRows()

    def onFileTransferStatusEvent(self, transferID, status, statusMessage,
                                  remotefileSize, schid):
        if schid != self.schid or transferID not in self.transfers:
            return

        i = list(self.transfers).index(transferID)
        idx = self.createIndex(i, 1)
        trans = self.transfers[transferID]
        if status == ERROR_file_transfer_complete:
            trans.updateSize(-1)

            if isinstance(trans, Download):
                self.downcounter -= 1
        else:
            # sadly, there are no events during transmission, then the next
            # line would make sense
            self.transfers[transferID].updateSize(remotefileSize)

        self.dataChanged(idx, idx)

        if self.downcounter == 0 and self.timer:
            self.killTimer(self.timer)

    def onServerErrorEvent(self, schid, errorMessage, error, returnCode,
                           extraMessage):
        if schid != self.schid or error != ERROR_ok:
            return

        for i, trans in enumerate(self.transfers.values()):
            if trans.retcode == returnCode:
                trans.updateError(error, errorMessage)

                idx = self.createIndex(i, 0)
                self.dataChanged(idx, idx)

                return

    def onServerPermissionErrorEvent(self, *args):
        self.onServerErrorEvent(*args)

    def headerData(self, section, orientation, role=Qt.DisplayRole):
        if role == Qt.DisplayRole and orientation == Qt.Horizontal:
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

        return len(self.transfers)

    def columnCount(self, parent=QModelIndex()):
        return 2

    def data(self, idx, role=Qt.DisplayRole):
        if not idx.isValid():
            return None

        trans = list(self.transfers.values())[idx.row()]

        if idx.column() == 0:
            if role == Qt.DisplayRole:
                return trans.description
            elif role == Qt.ForegroundRole:
                if trans.hasError:
                    return Qt.red
        elif idx.column() == 1:
            if role == Qt.DisplayRole:
                return trans.progress

        return None


class FileTransferDelegate(QStyledItemDelegate):
    """
    Delegate which displays a progress bar in the second column of an itemview
    """
    def paint(self, painter, option, idx):
        if idx.column() != 1:
            QStyledItemDelegate.paint(self, painter, option, idx)
            return

        progress = idx.data()

        pgoptions = QStyleOptionProgressBar()
        pgoptions.rect = option.rect
        pgoptions.minimum = 0
        pgoptions.maximum = 100
        pgoptions.progress = progress
        pgoptions.text = "%s%%" % progress
        pgoptions.textVisible = True

        QApplication.style().drawControl(QStyle.CE_ProgressBar, pgoptions,
                                         painter)


class FileTransferDialog(QDialog, pytson.Translatable):
    """
    Dialog to display filetransfers from/to a ts3 channel.
    """

    def __init__(self, schid, cid, password, parent=None):
        super(QDialog, self).__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)

        try:
            setupUi(self, pytson.getPluginPath("ressources",
                                               "filetransfer.ui"))

            self.delegate = FileTransferDelegate(self)
            self.table.setItemDelegate(self.delegate)

            self.model = FileTransferModel(schid, cid, password, self)
            self.table.setModel(self.model)
        except Exception as e:
            self.delete()
            raise e

        self.resize(770, 250)

    def on_closeButton_clicked(self):
        self.close()

    def on_cleanupButton_clicked(self):
        self.model.cleanup()

    def addUpload(self, path, localfile, overwrite, resume):
        """
        Adds an upload.
        @param path: path to upload the file to
        @type path: str
        @param localfile: path to the file to upload
        @type localfile: str
        @param overwrite: set to True to overwrite an existing file
        @type overwrite: bool
        @param resume: set to True to resume a previous upload
        @type resume: bool
        @return: the filetransfer id
        @rtype: int
        """
        return self.model.addUpload(path, localfile, overwrite, resume)

    def addDownload(self, thefile, downloaddir, overwrite, resume):
        """
        Adds a download.
        @param thefile: remote file to download
        @type thefile: File
        @param downloaddir: path to the download directory
        @type downloaddir: str
        @param overwrite: set to True to overwrite an existing file
        @type overwrite: bool
        @param resume: set to True to resume a previous download
        @type resume: bool
        @return: the filetransfer id
        @rtype: int
        """
        return self.model.addDownload(thefile, downloaddir, overwrite, resume)
