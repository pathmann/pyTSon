import os

from PythonQt.QtCore import Qt, QLocale
from PythonQt.QtGui import (QDialog, QHeaderView, QTableWidgetItem,
                            QToolButton, QIcon, QColor, QFont, QMessageBox,
                            QCheckBox, QColorDialog, QFileDialog, QInputDialog)

import ts3defines

from . import setupUi, ts3print
from .repository import RepositoryDialog, InstallDialog

import pytson
import ts3client
import devtools


class ConfigurationDialog(QDialog, pytson.Translatable):
    # [(objectName, store, [children])]
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
                        ("languageGroup", False, [
                            ("languageButton", True, []),
                            ("languageCombo", True, []),
                        ]),
                        ("siteGroup", False, [
                            ("siteTable", True, []),
                            ("sitereloadButton", True, []),
                            ("siteaddButton", True, []),
                            ("siteremoveButton", True, []),
                        ]),
                        ("loadMenusButton", True, []),
                        ("verboseButton", True, []),
                    ]),
                    ("consoleTab", False, [
                        ("colorsBox", False, [
                            ("bgColorButton", True, []),
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
        self.rpd = None
        self.sitepkgs = None

        try:
            setupUi(self, pytson.getPluginPath("ressources",
                                               "pyTSon-configdialog.ui"),
                    widgets=self.CONF_WIDGETS)
            self.pluginsTable.horizontalHeader().setSectionResizeMode(0, QHeaderView.Stretch)
            self.siteTable.horizontalHeader().setSectionResizeMode(0, QHeaderView.Stretch)

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
            ts3print(self._tr("Error loading iconpack: {exception}").
                     format(exception=e), ts3defines.LogLevel.LogLevel_ERROR,
                     "pyTSon.ConfigurationDialog.setupList", 0)

        row = 0
        diffapi = self.cfg.getboolean("general", "differentApi",
                                      fallback=False)
        for key, p in self.host.plugins.items():
            if diffapi or p.apiVersion == pytson.getCurrentApiVersion():
                item = QTableWidgetItem(p.name)
                item.setFlags(Qt.ItemIsSelectable | Qt.ItemIsUserCheckable |
                              Qt.ItemIsEnabled)
                item.setCheckState(Qt.Checked if key in self.host.active
                                   else Qt.Unchecked)
                item.setData(Qt.UserRole, key)
                self.pluginsTable.setItem(row, 0, item)

                if p.offersConfigure:
                    setbutton = QToolButton()
                    setbutton.connect("clicked()", lambda n=p.name:
                                      self.onSettingsButtonClicked(n))
                    setbutton.setToolTip(self._tr("Configure"))
                    if ico:
                        setbutton.setIcon(QIcon(ico.icon("SETTINGS")))
                    self.pluginsTable.setCellWidget(row, 1, setbutton)

                    if p.name not in self.host.active:
                        setbutton.setEnabled(False)

                rembutton = QToolButton()
                if ico:
                    rembutton.setIcon(QIcon(ico.icon("DELETE")))
                rembutton.connect("clicked()", lambda n=p.name:
                                  self.onRemoveButtonClicked(n))
                rembutton.setToolTip(self._tr("Remove"))
                self.pluginsTable.setCellWidget(row, 2, rembutton)

                row += 1

        if ico:
            ico.close()

        self.pluginsTable.setRowCount(row)
        self.pluginsTable.sortItems(0)

    def setupValues(self):
        if self.cfg.getboolean("general", "differentApi", fallback=False):
            self.differentApiButton.setChecked(Qt.Checked)
        else:
            self.differentApiButton.setChecked(Qt.Unchecked)
        self.requiredApiEdit.setText(pytson.getCurrentApiVersion())

        if self.cfg.getboolean("general", "loadAllMenus", fallback=False):
            self.loadMenusButton.setChecked(Qt.Checked)
        else:
            self.loadMenusButton.setChecked(Qt.Unchecked)

        bgcolor = QColor(self.cfg.get("console", "backgroundColor")).name()
        self.bgColorButton.setStyleSheet("background-color: %s;" % bgcolor)
        txtcolor = QColor(self.cfg.get("console", "textColor")).name()
        self.textColorButton.setStyleSheet("background-color: %s;" % txtcolor)

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
        self.silentButton.setChecked(self.cfg.getboolean("console",
                                                         "silentStartup"))

        self.languageCombo.addItem("Default", "en_US")
        for loc in pytson.locales():
            qloc = QLocale(loc)
            self.languageCombo.addItem(qloc.nativeLanguageName(), loc)

            if self.cfg.get("general", "language") == loc:
                self.languageCombo.setCurrentIndex(self.languageCombo.count - 1)

        if self.cfg.get("general", "language") == "inherited":
            self.languageButton.setChecked(True)
            self.languageCombo.setEnabled(False)

        if self.cfg.getboolean("general", "verbose"):
            self.verboseButton.setChecked(True)

        self.setupList()
        self.reloadSite()

    def setupSlots(self):
        # you can of course connect signal-slots manually,
        # scriptEdit.textEdited for example is connected by setupui
        self.differentApiButton.connect("stateChanged(int)",
                                        self.onDifferentApiButtonChanged)
        self.pluginsTable.connect("currentCellChanged(int, int, int, int)",
                                  self.onPluginsTableCurrentItemChanged)
        self.pluginsTable.connect("itemChanged(QTableWidgetItem*)",
                                  self.onPluginsTableItemChanged)
        self.reloadButton.connect("clicked()", self.onReloadButtonClicked)

        self.loadMenusButton.connect("stateChanged(int)",
                                     self.onLoadMenusButtonChanged)

        self.bgColorButton.connect("clicked()",
                                   self.onBackgroundColorButtonClicked)
        self.textColorButton.connect("clicked()",
                                     self.onTextColorButtonClicked)
        self.fontFamilyCombo.connect("currentFontChanged(QFont)",
                                     self.onFontFamilyComboChanged)
        self.fontSizeSpin.connect("valueChanged(int)",
                                  self.onFontSizeSpinChanged)
        self.tabcompleteButton.connect("stateChanged(int)",
                                       self.onTabcompleteButtonChanged)
        self.spacesButton.connect("stateChanged(int)",
                                  self.onSpacesButtonChanged)
        self.tabwidthSpin.connect("valueChanged(int)",
                                  self.onTabwidthSpinChanged)
        self.languageButton.connect("stateChanged(int)",
                                    self.onLanguageButtonStateChanged)
        self.languageCombo.connect("currentIndexChanged(int)",
                                   self.onLanguageComboCurrentIndexChanged)
        self.verboseButton.connect("stateChanged(int)",
                                   self.onVerboseButtonStateChanged)
        self.sitereloadButton.connect("clicked()", self.reloadSite)

    def onLoadMenusButtonChanged(self, state):
        if state == Qt.Checked:
            self.cfg.set("general", "loadAllMenus", "True")
        else:
            self.cfg.set("general", "loadAllMenus", "False")

        QMessageBox.information(self, self._tr("Restart required"),
                                self._tr("Changes only take effect after a "
                                         "restart."))

    def onDifferentApiButtonChanged(self, state):
        if state == Qt.Checked:
            self.cfg.set("general", "differentApi", "True")
        else:
            self.cfg.set("general", "differentApi", "False")

        self.host.reload()
        self.host.start()
        self.setupList()

    def onPluginsTableCurrentItemChanged(self, currow, curcol, prevrow,
                                         prevcol):
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
                    self.pluginsTable.cellWidget(item.row(),
                                                 1).setEnabled(True)
            else:
                item.setCheckState(Qt.Unchecked)
                QMessageBox.critical(self, self._tr("Activation failed"),
                                     self._tr("Error starting plugin, check "
                                     "your client log for more information"))
        elif not checked and name in self.host.active:
            if self.host.active[name].offersConfigure:
                self.pluginsTable.cellWidget(item.row(), 1).setEnabled(False)
            self.host.deactivate(name)

    def onRemoveButtonClicked(self, pluginname):
        if pluginname in self.host.plugins:
            if self.cfg.getboolean("general", "uninstallQuestion"):
                def cbox_state_changed(state):
                    if state == Qt.Checked:
                        self.cfg.set("general", "uninstallQuestion", "False")
                    else:
                        self.cfg.set("general", "uninstallQuestion", "True")

                cbox = QCheckBox(self._tr("Don't ask me again"))
                cbox.connect("stateChanged(int)", cbox_state_changed)

                msgbox = QMessageBox(self)
                msgbox.setInformativeText(self._tr("Do you really want to "
                                                   "delete plugin {name}?\n"
                                                   "This will erase all "
                                                   "script data of the "
                                                   "plugin from disk.").
                                          format(name=pluginname))
                msgbox.setIcon(4)  # QMessageBox::Icon::Question = 4
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
        curcolor = QColor(self.cfg.get("console", "backgroundColor"))
        c = QColorDialog.getColor(curcolor, self,
                                  self._tr("Console Background Color"))
        if c.isValid():
            self.cfg.set("console", "backgroundColor", c.name())
            self.bgColorButton.setStyleSheet("background-color: %s;" %
                                             c.name())

    def onTextColorButtonClicked(self):
        curcolor = QColor(self.cfg.get("console", "textColor"))
        c = QColorDialog.getColor(curcolor, self,
                                  self._tr("Console Text Color"))
        if c.isValid():
            self.cfg.set("console", "textColor", c.name())
            self.textColorButton.setStyleSheet("background-color: %s;" %
                                               c.name())

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
        curdir = os.path.dirname(self.cfg.get("console", "startup"))
        fname = QFileDialog.getOpenFileName(self, self._tr("Startup script"),
                                            curdir, "Python scripts (*.py)")
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

    def onLanguageComboCurrentIndexChanged(self, idx):
        self.cfg.set("general", "language", self.languageCombo.currentData)

    def onLanguageButtonStateChanged(self, state):
        if state == Qt.Checked:
            self.cfg.set("general", "language", "inherited")
        else:
            self.cfg.set("general", "language", self.languageCombo.currentData)

        self.languageCombo.setEnabled(state == Qt.Unchecked)

    def onVerboseButtonStateChanged(self, state):
        self.cfg.set("general", "verbose", str(state == Qt.Checked))

    def reloadSite(self):
        self.siteTable.clearContents()

        self.sitepkgs = devtools.installedPackages()
        self.siteTable.setRowCount(len(self.sitepkgs))

        for i, pkg in enumerate(self.sitepkgs):
            item = QTableWidgetItem(pkg["name"])
            item.setData(Qt.UserRole, i)
            self.siteTable.setItem(i, 0, item)

            item = QTableWidgetItem(pkg["version"])
            item.setData(Qt.UserRole, i)
            self.siteTable.setItem(i, 1, item)

        self.siteTable.sortItems(0)

    def on_siteTable_itemSelectionChanged(self):
        self.siteremoveButton.setEnabled(self.siteTable.selectedItems())

    def on_siteaddButton_clicked(self):
        pkg = QInputDialog.getText(self, self._tr("Install site package"),
                                   self._tr("Package name[==version]")).strip()

        if pkg != "":
            self.installer = InstallDialog(self.host, self)
            self.installer.show()
            self.installer.installPackage(pkg)
            self.reloadSite()

    def on_siteremoveButton_clicked(self):
        ids = {it.data(Qt.UserRole) for it in self.siteTable.selectedItems()}

        if ids:
            if (QMessageBox.question(self, self._tr("Remove site packages"),
                                     self._tr("Are you sure to remove the "
                                              "selected packages? Some "
                                              "plugins might not work "
                                              "properly.")) ==
               QMessageBox.Yes):
                try:
                    for idx in ids:
                        devtools.removePackage(self.sitepkgs[idx]["name"],
                                               self.sitepkgs[idx]["version"])
                except Exception as e:
                    QMessageBox.critical(self, self._tr("Error"), str(e))

        self.reloadSite()
