import os
import shutil
import platform

import ts3lib

from PythonQt.QtGui import QApplication


def tr(sourcetext, *, disambiguation="", n=-1, context=None):
    """

    """
    return QApplication.translate(context, sourcetext, disambiguation, n)


class Translatable(object):
    """

    """
    @classmethod
    def _tr(cls, sourcetext, *, disambiguation="", n=-1, context=None):
        """

        """
        if not context:
            return tr(sourcetext, disambiguation=disambiguation, n=n,
                      context=cls.__name__)
        else:
            return tr(sourcetext, disambiguation=disambiguation, n=n,
                      context=context)


def getConfigPath(*args):
    """
    Returns pyTSon's configpath, that is, the subdirectory 'pyTSon' in the
    TeamSpeak 3 config directory.
    @param args: path fields joined to the result as list of strings
    @type args: list[str]
    @return: The accumulated path
    @rtype: str
    """
    return os.path.join(ts3lib.getConfigPath(), "pyTSon", *args)


def getPluginPath(*args):
    """
    Returns pyTSon's pluginpath, that is, the subdirectory 'pyTSon' in the
    TeamSpeak 3 plugins directory.
    @param args: path fields joined to the result as list of strings
    @type args: list[str]
    @return: The accumulated path
    @rtype: str
    """
    return os.path.join(ts3lib.getPluginPath(), "pyTSon", *args)


def _setup():
    """
    Sets up pyTSon's infrastructure.
    """
    if not os.path.isdir(getConfigPath()):
        os.mkdir(getConfigPath())

    respath = getPluginPath("ressources", "repositorymaster.json")
    confpath = getConfigPath("repositorymaster.json")
    if not os.path.exists(confpath):
        shutil.copy(respath, confpath)


def platformstr():
    """
    Returns the platform pyTSon is currently running on.
    @return: the platform (and architecture) string
    @rtype: str
    """
    if platform.system() == "Mac":
        return "Mac"
    else:
        return "%s-%s" % (platform.system(), platform.architecture()[0])


def getVersion():
    """
    Returns the current version of pyTSon.
    @return: the version as string
    @rtype: str
    """
    return "1.1.0"


def getCurrentApiVersion():
    """
    Returns the current apiversion of the ts3 plugin sdk. This should not be
    used in ts3plugin.apiVersion. Be fair and update your plugin manually!
    @return: the apiVersion
    @rtype: str
    """
    return 21
