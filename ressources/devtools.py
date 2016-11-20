from ts3plugin import PluginHost

import os
import ts3

from itertools import chain
from pip import main as pipmain

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


def createPlugin(name):
    """

    """
    ename = _escapeString(name)
    if ename in PluginHost.plugins:
        raise Exception("A plugin with that escaped name already exist")

    p = os.path.join(ts3.getPluginPath(), "pyTSon", "scripts", ename)
    if os.path.isdir(p):
        raise Exception("Directory already exist")

    os.mkdir(p)
    os.mkdir(os.path.join(p, "include"))

    fp = os.path.join(p, ename + ".py")
    with open(fp, "w") as f:
        f.write(PLUGIN_SKELETON % (ename, ename))

    return fp


def installOnlinePlugin(addon, fpath):
    """
    @param addon: json dict

    @param fpath: path to downloaded file (zip or py)
    """
    pass


def uninstallPlugin(name):
    """

    """
    pass


def installDependencies(name, deps):
    """

    """
    ename = _escapeString(name)
    if ename in PluginHost.plugins:
        return False

    try:
        pipmain(["install", "-t", os.path.join(ts3.getPluginPath(), "pyTSon", "scripts", ename)] + deps)
    except SystemExit:
        return False
