import os, shutil
import ts3

from itertools import chain
from zipfile import ZipFile

from pip import main as pipmain
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
    os.mkdir(os.path.join(p, "include"))

    fp = os.path.join(p, ename + ".py")
    if withfile:
        with open(fp, "w") as f:
            if content:
                f.write(content)
            else:
                f.write(PLUGIN_SKELETON % (ename, ename))

    return fp


def installOnlinePlugin(addon, downpath):
    """
    @param addon: json dict

    @param downpath: path to downloaded file (zip or py)
    """
    fp = createPlugin(addon["name"], withfile=False)

    installDependencies(addon["name"], addon["dependencies"])

    ext = os.path.splitext(fpath)
    if ext == ".py":
        os.rename(downpath, fp)
    elif ext == ".zip":
        with open(downpath, "r") as f:
            zip = ZipFile(f)
            zip.extractall(os.path.abspath(fp))
            zip.close()
    else:
        raise Exception("Unrecognized file extension")


def uninstallPlugin(name):
    """

    """
    ename = _escapeString(name)
    p = os.path.join(ts3.getPluginPath(), "pyTSon", "scripts", ename)
    if os.path.isdir(p):
        shutil.rmtree(p)


def installDependencies(name, deps):
    """

    """
    if len(deps) == 0:
        return

    ename = _escapeString(name)

    #platform.libc_ver = lambda: ""
    try:
        pipmain(["install", "-t", os.path.join(ts3.getPluginPath(), "pyTSon", "scripts", ename, "include")] + deps)
    except SystemExit:
        return False
