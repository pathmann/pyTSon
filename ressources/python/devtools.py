import sys
import os
import shutil

import pytson

from itertools import chain
from zipfile import ZipFile

import subprocess


class PluginInstaller(object):
    """
    Class used to install new python plugins and its dependencies.
    """

    PLUGIN_SKELETON = """
from ts3plugin import ts3plugin

class %s(ts3plugin):
    name = "%s"
    apiVersion = 21
    version = "1.0.0"
    author = ""
    description = ""
    requestAutoload = False
    offersConfigure = False
    commandKeyword = ""
    infoTitle = None
    menuItems = []
    hotkeys = []

    """

    def __init__(self, stdout=None):
        """
        @param stdout: A callable used as print function (takes str argument);
        defaults to None; if None stdout is used instead
        @type stdout: callable
        """
        self.stdout = stdout

        self.working = False

    @staticmethod
    def _escapeString(name):
        """
        Replaces all occurences of non-ascii characters with '_'
        @param name: the unescaped string
        @type name: str
        @return: The escaped string
        @rtype: str
        """
        return ''.join([c if ord(c) in chain(range(48, 57), range(65, 90),
                                             range(97, 122))
                        else '_' for c in name])

    @staticmethod
    def createPlugin(name, withfile=True, content=None):
        """
        Creates the infrastructure for a new plugin.
        @param name: the name of the plugin
        @type name: str
        @param withfile: if True, the file __init__.py is created in the plugin
        directory, defaults to True
        @type withfile: bool
        @param content: content of __ini__.py; defaults to None; if None, an
        empty plugin skeleton is written to the file (if withfile is True)
        @type content: str
        @return: the path to the __init__.py of the new created plugin
        @rtype: str
        """
        ename = PluginInstaller._escapeString(name)

        p = pytson.getPluginPath("scripts", ename)
        if os.path.isdir(p):
            raise Exception("Directory already exist")

        os.mkdir(p)

        fp = os.path.join(p, "__init__.py")
        if withfile:
            with open(fp, "w") as f:
                if content:
                    f.write(content)
                else:
                    f.write(PluginInstaller.PLUGIN_SKELETON % (ename, ename))

        return fp

    @staticmethod
    def removePlugin(name):
        """
        Uninstall a plugin (delete all data in scripts directory).
        @param name: the name of the plugin
        @type name: str
        """
        ename = PluginInstaller._escapeString(name)
        p = pytson.getPluginPath("scripts", ename)
        if os.path.isdir(p):
            shutil.rmtree(p)

    def _print(self, msg):
        if self.stdout:
            self.stdout(msg)
        else:
            print(msg)

    def installPlugin(self, addon, data):
        """
        Installs a new plugin into the scripts directory.
        @param addon: json dict containing the plugin information
        @type addon: dict
        @param data: either the content of a single python file as string or a
        file-like-object to a zipfile which will be extracted
        @type data: str or file-like
        """
        if self.working:
            raise Exception("There is already an installation in progress")
        else:
            self.working = True

        fp = self.createPlugin(addon["name"], withfile=False)
        self._print("Directory created.")

        plat = pytson.platformstr()
        if ("dependencies" in addon and plat in addon["dependencies"] and
           len(addon["dependencies"][plat]) > 0):
            self._print("Installing dependencies ...")
            if not self.installPackages(addon["dependencies"][plat]):
                self._print("Aborting, package installation might be broken"
                            "and needs to be fixed manually")
                self.working = False
                return
        self._print("Done. Installing files ...")

        if type(data) is str:
            with open(fp, "w") as f:
                f.write(data)
        else:
            with ZipFile(data) as zip:
                zip.extractall(os.path.dirname(os.path.abspath(fp)))

        self._print("Plugin installed.")
        self.working = False

    def installPackages(self, deps):
        """
        Installs packages from pypi.python.org into the include directory.
        @param deps: A list of package names
        @type deps: list[str]
        @return: True on success, False otherwise
        @rtype: bool
        """
        p = subprocess.Popen([sys.executable, "-m", "pip", "install",
                              "--target", pytson.getPluginPath("include")] + deps,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()

        if err:
            self._print(err)
        if out:
            self._print(out)

        return p.returncode == 0
