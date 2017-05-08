from . import ts3print, setupUi
from .dialogs import MultiInputDialog

import ts3defines
import pytson
import devtools

import json
import traceback
import io

from PythonQt.QtCore import Qt, QUrl
from PythonQt.QtGui import (QDialog, QMovie, QListWidgetItem, QMessageBox,
                            QInputDialog, QLineEdit)
from PythonQt.QtNetwork import (QNetworkAccessManager, QNetworkRequest,
                                QNetworkReply)
from PythonQt import BoolResult


class RepositoryDialog(QDialog, pytson.Translatable):
    master_url = QUrl("https://raw.githubusercontent.com/pathmann/pyTSon_repository/master/repositorymaster.json")

    def __init__(self, host, parent=None):
        super(QDialog, self).__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)

        self.host = host

        self.pending = 0

        try:
            with open(pytson.getConfigPath("repositorymaster.json"), "r") as f:
                repmaster = json.loads(f.read())
                self.replist = {x["name"]: x for x in repmaster}
        except:
            ts3print(self._tr("Error opening repositorymaster"),
                     ts3defines.LogLevel.LogLevel_ERROR,
                     "pyTSon.RepositoryDialog", 0)
            raise Exception("Error opening repositorymaster")

        try:
            setupUi(self, pytson.getPluginPath("ressources", "repository.ui"))
            self.updateMasterlist()

            movie = QMovie(pytson.getPluginPath("ressources", "loading.gif"),
                           "", self)
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

        self.pending += sum(x["active"] for x in self.replist.values())
        for rep in self.replist.values():
            if all(x in rep for x in ['name', 'url', 'origin', 'active']):
                if rep["active"]:
                    self.nwm.get(QNetworkRequest(QUrl(rep["url"])))
            else:
                self.pending -= 1
                ts3print(self._tr("Invalid repository in list, ignoring"),
                         ts3defines.LogLevel.LogLevel_WARNING,
                         "pyTSon.RepositoryDialog.updateRepositories", 0)

        self.updatePendingButtons()

    def updateMaster(self):
        self.pending += 1

        self.masterloadingLabel.show()
        self.updateButton.setEnabled(False)

        self.nwm.get(QNetworkRequest(self.master_url))
        self.updatePendingButtons()

    def handleMasterReply(self, reply):
        if reply.error() == QNetworkReply.NoError:
            try:
                repos = json.loads(reply.readAll().data().decode('utf-8'))

                for r in repos:
                    if all(x in r for x in ["name", "url", "active",
                                            "origin"]):
                        self.addRepository(r)
                    else:
                        ts3print(self._tr("Invalid entry in repositorymaster"),
                                 ts3defines.LogLevel.LogLevel_WARNING,
                                 "pyTSon.RepositoryManager.onNetworkReply",
                                 0)
            except:
                ts3print(self._tr("Error reading repositorymaster: {trace}").
                         format(trace=traceback.format_exc()),
                         ts3defines.LogLevel.LogLevel_ERROR,
                         "pyTSon.RepositoryManager.onNetworkReply", 0)

        self.updateMasterlist()

    def updateAddons(self, repo, addons):
        for a in addons:
            if all(x in a for x in ["name", "author", "version",
                                    "apiVersion", "description",
                                    "url"]):
                a["repository"] = repo["name"]
                if not a["name"] in self.addons:
                    self.addons[a["name"]] = a
            else:
                ts3print(self._tr("Invalid entry in repository {name}: "
                                  "{repo}").format(name=repo["name"],
                         repo=str(a)), ts3defines.LogLevel.LogLevel_WARNING,
                         "pyTSon.RepositoryDialog.onNetworkReply", 0)
                break

    def handleRepositoryReply(self, reply):
        repo = None
        for r in self.replist.values():
            if reply.url().url() == r["url"]:
                repo = r
                break

        if not repo:
            ts3print(self._tr("Error matching answer from {url} to a "
                              "repository").format(url=reply.url().url()),
                     ts3defines.LogLevel.LogLevel_ERROR,
                     "pyTSon.RepositoryDialog.onNetworkReply", 0)
        elif reply.error() == QNetworkReply.NoError:
            try:
                self.updateAddons(repo, json.loads(reply.readAll().data()
                                  .decode('utf-8')))
            except ValueError as e:
                ts3print(self._tr("Error parsing repository {name}: "
                                  "{exception}").format(name=repo["name"],
                         exception=str(e)),
                         ts3defines.LogLevel.LogLevel_WARNING,
                         "pyTSon.RepositoryDialog.onNetworkReply", 0)
        else:
            ts3print(self._tr("Network error updating repository {name}: "
                              "{error}").format(name=repo["name"],
                                                error=reply.error()),
                     ts3defines.LogLevel.LogLevel_WARNING,
                     "pyTSon.RepositoryDialog.onNetworkReply", 0)

        if self.pending == 0:
            self.updateAddonlist()

    def onNetworkReply(self, reply):
        self.pending -= 1

        if reply.url() == self.master_url:
            self.handleMasterReply(reply)
        else:
            self.handleRepositoryReply(reply)

        reply.deleteLater()

        self.updatePendingButtons()

    def addRepository(self, r):
        name = r["name"]
        if name in self.replist:
            if self.replist[name]["origin"] == "online":
                if self.replist[name]["url"] != r["url"]:
                    self.replist[name]["url"] = r["url"]
                    ts3print(self._tr("Url for repository {name} updated").
                             format(name=name),
                             ts3defines.LogLevel.LogLevel_INFO,
                             "pyTSon.RepositoryManager.addRepository", 0)
            else:
                ts3print(self._tr("Ignoring online repository {name}, got a "
                                  "local one with that name").
                         format(name=name), ts3defines.LogLevel.LogLevel_INFO,
                         "pyTSon.RepositoryManager.addRepository", 0)
        else:
            self.replist[name] = r

    def updateMasterlist(self):
        self.repositoryList.clear()

        for name, r in self.replist.items():
            item = QListWidgetItem(name)
            item.setFlags(Qt.ItemIsSelectable | Qt.ItemIsUserCheckable |
                          Qt.ItemIsEnabled)
            item.setCheckState(Qt.Checked if r["active"] else Qt.Unchecked)
            item.setData(Qt.UserRole, name)
            self.repositoryList.addItem(item)

    def updateAddonlist(self):
        if self.pluginsList.currentItem():
            cur = self.pluginsList.currentItem().text()
        else:
            cur = None

        self.pluginsList.clear()

        for a in self.addons.values():
            if (self.replist[a["repository"]]["active"] and
               ("platforms" not in a or
               pytson.platformstr() in a["platforms"])):
                item = QListWidgetItem(a["name"], self.pluginsList)
                item.setFlags(Qt.ItemIsSelectable | Qt.ItemIsUserCheckable |
                              Qt.ItemIsEnabled)
                item.setData(Qt.UserRole, a["name"])

                if a["name"] in self.host.plugins:
                    if a["version"] > self.host.plugins[a["name"]].version:
                        item.setForeground(Qt.red)
                        item.setToolTip(self._tr("Update available"))
                    elif a["version"] == self.host.plugins[a["name"]].version:
                        item.setForeground(Qt.green)
                        item.setToolTip(self._tr("You have this plugin "
                                                 "installed, no update "
                                                 "available"))
                    elif a["version"] < self.host.plugins[a["name"]].version:
                        item.setForeground(Qt.gray)
                        item.setToolTip(self._tr("Your local version has a "
                                        "greater version number"))

                if cur and a["name"] == cur:
                    self.pluginsList.setCurrentItem(item)

        self.pluginsList.sortItems()

    def on_updateButton_clicked(self):
        self.updateMaster()

    def on_addButton_clicked(self):
        (res, name, url) = MultiInputDialog.getTexts(self._tr("Add repository"),
                                                     self._tr("Name:"),
                                                     self._tr("URL:"), "", "",
                                                     self)

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
                item.setFlags(Qt.ItemIsSelectable | Qt.ItemIsUserCheckable |
                              Qt.ItemIsEnabled)
                item.setCheckState(Qt.Checked)
                item.setData(Qt.UserRole, name)
                self.repositoryList.addItem(item)
            else:
                QMessageBox.critical(self._tr("Error"),
                                     self._tr("The URL {url} is not valid").
                                     format(url=url))

    def on_deleteButton_clicked(self):
        cur = self.repositoryList.currentItem()
        if cur:
            name = cur.data(Qt.UserRole)
            if name not in self.replist:
                QMessageBox.critical(self._tr("Internal error"),
                                     self._tr("Can't find repository {name} "
                                              "in list").format(name=name))
                return

            del self.replist[name]
            cur.delete()

    def on_repositoryList_doubleClicked(self, item):
        name = item.data(Qt.UserRole)
        try:
            rep = self.replist[name]
        except:
            QMessageBox.critical(self, self._tr("Internal error"),
                                 self._tr("Can't find repository {name} in "
                                          "list").format(name=name))
            return

        ok = BoolResult()
        newurl = QInputDialog.getText(self,
                                      self._tr("Change url of repository "
                                               "{name}").format(name=name),
                                      self._tr("Url:"), QLineEdit.Normal,
                                      rep["url"], ok)

        if ok:
            rep["url"] = newurl
            rep["origin"] = "local"

    def on_repositoryList_currentItemChanged(self, cur, prev):
        if cur:
            name = cur.data(Qt.UserRole)
            if name not in self.replist:
                self.deleteButton.setEnabled(False)
                QMessageBox.critical(self, self._tr("Internal error"),
                                     self._tr("Can't find repository {name} "
                                              "in list").format(name=name))
                return

            self.deleteButton.setEnabled(True)
        else:
            self.deleteButton.setEnabled(False)

    def on_repositoryList_itemChanged(self, item):
        if not item:
            return

        name = item.data(Qt.UserRole)

        if name not in self.replist:
            QMessageBox.critical(self, self._tr("Internal error"),
                                 self._tr("Can't find repository {name} in "
                                          "list").format(name=name))
            return
        if self.replist[name]["active"] != (item.checkState() == Qt.Checked):
            self.replist[name]["active"] = (item.checkState() == Qt.Checked)
            self.updateAddonlist()

    def on_pluginsList_currentItemChanged(self, cur, prev):
        if cur:
            name = cur.data(Qt.UserRole)
            if name not in self.addons:
                QMessageBox.critical(self, self._tr("Internal error"),
                                     self._tr("Can't find addon {name} in "
                                              "list").format(name=name))
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
                    self.installButton.setText(self._tr("Update"))
                else:
                    self.installButton.setEnabled(False)
                    self.installButton.setText(self._tr("Install"))
            else:
                self.installButton.setEnabled(True)
                self.installButton.setText(self._tr("Install"))
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
        if name not in self.addons:
            QMessageBox.critical(self, self._tr("Internal error"),
                                 self._tr("Can't find addon {name} in list").
                                 format(name=name))
            return

        p = self.addons[name]

        # update?, so remove the local one first
        if name in self.host.plugins:
            if p["version"] > self.host.plugins[name].version:
                devtools.PluginInstaller.removePlugin(name)
            else:
                # should not happen (ui restricted)
                QMessageBox.critical(self, self._tr("Internal error"),
                                     self._tr("This plugin is already "
                                              "installed"))
                return

        self.installer = InstallDialog(self.host, self)
        self.installer.show()
        self.installer.install(p)


class InstallDialog(QDialog, pytson.Translatable):
    def __init__(self, host, parent=None):
        super(QDialog, self).__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)
        self.setModal(True)

        self.host = host
        self.addon = None

        try:
            setupUi(self, pytson.getPluginPath("ressources", "installer.ui"))

            self.pip = devtools.PluginInstaller(self.consoleEdit.append)
            self.nwm = QNetworkAccessManager(self)
            self.nwm.connect("finished(QNetworkReply*)", self.onNetworkReply)

            self.closeFrame.hide()
        except Exception as e:
            self.delete()
            raise e

    def install(self, addon):
        self.addon = addon

        req = QNetworkRequest(QUrl(addon["url"]))
        req.setAttribute(QNetworkRequest.FollowRedirectsAttribute, True)
        self.nwm.get(req)
        self.consoleEdit.append("Downloading %s ..." % addon["url"])

    def installPackage(self, pkgstr):
        self.pip.installPackages([pkgstr])
        self.closeFrame.show()

    def onNetworkReply(self, reply):
        if reply.error() == QNetworkReply.NoError:
            self.consoleEdit.append("Download finished.")

            try:
                if ("application/zip" not in
                   reply.header(QNetworkRequest.ContentTypeHeader)):
                    self.pip.installPlugin(self.addon,
                                           reply.readAll().data().
                                           decode('utf-8'))
                else:
                    self.pip.installPlugin(self.addon,
                                           io.BytesIO(reply.readAll().data()))
            except Exception as e:
                self.consoleEdit.append("Exception during installation: %s" %
                                        e)
        else:
            self.consoleEdit.append("Network error: %s" % reply.error())

        reply.deleteLater()

        self.closeFrame.show()

    def on_closeButton_clicked(self):
        self.accept()
