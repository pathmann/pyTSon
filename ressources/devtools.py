import sys, os, shutil
import ts3

from itertools import chain
from zipfile import ZipFile

import subprocess
import platform

PLUGIN_SKELETON = """
from ts3plugin import ts3plugin

class %s(ts3plugin):
    name = "%s"
    apiVersion = 21
    version = "1.0.0"
    author = ""
    description = ""

"""


def _escapeString(name):
    """

    """
    return ''.join([c if ord(c) in chain(range(65, 90), range(97, 122)) else '_' for c in name])


def createPlugin(name, withfile=True, content=None):
    """

    """
    ename = _escapeString(name)

    p = os.path.join(ts3.getPluginPath(), "pyTSon", "scripts", ename)
    if os.path.isdir(p):
        raise Exception("Directory already exist")

    os.mkdir(p)

    fp = os.path.join(p, "__init__.py")
    if withfile:
        with open(fp, "w") as f:
            if content:
                f.write(content)
            else:
                f.write(PLUGIN_SKELETON % (ename, ename))

    return fp


def uninstallPlugin(name):
    """

    """
    ename = _escapeString(name)
    p = os.path.join(ts3.getPluginPath(), "pyTSon", "scripts", ename)
    if os.path.isdir(p):
        shutil.rmtree(p)


class SimplePip(object):
    def __init__(self, stdout=None):
        self.stdout = stdout

        self.working = False

    def _print(self, msg):
        if self.stdout:
            self.stdout(msg)
        else:
            print(msg)

    def installPlugin(self, addon, data, iszip):
        """
        @param addon: json dict

        @param data:

        @param iszip:

        """
        if self.working:
            raise Exception("There is already an installation in progress")
        else:
            self.working = True

        fp = createPlugin(addon["name"], withfile=False)
        self._print("Directory created.")

        if platform.system() == "Mac":
            plat = "Mac"
        else:
            plat = "%s-%s" % (platform.system(), platform.architecture()[0])
        if addon["dependencies"] and plat in addon["dependencies"] and len(addon["dependencies"][plat]) > 0:
            self._print("Installing dependencies ...")
            self.installPackages(os.path.dirname(os.path.abspath(fp)), addon["dependencies"][plat])
        self._print("Done. Installing files ...")

        if iszip:
            with ZipFile(data) as zip:
                zip.extractall(os.path.dirname(os.path.abspath(fp)))
        else:
            with open(fp, "w") as f:
                f.write(data)

        self._print("Plugin installed.")
        self.working = False

    def installPackages(self, path, deps):
        """

        """
        p = subprocess.Popen([sys.executable, "-m", "pip", "install", "--target", os.path.join(path, "include")] + deps, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()
        self._print(out)
        self._print(err)

        return p.returncode == 0
