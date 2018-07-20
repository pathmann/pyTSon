import os
import shutil
import platform
import glob
import re

import ts3lib

from PythonQt.QtGui import QApplication


_PYTSON_VERSION = "1.2.3"


def tr(context, sourcetext, *, disambiguation="", n=-1):
    """
    Returns the current translation for a string. This function calls can be
    extracted by pyTSon's pylupdate.py.
    @param context: context of the string literal, must be a raw string, not
    the return value of another function, an attribute or such
    @type context: str
    @param sourcetext: translatable string, must be a raw string, not the
    return value of another function, an attribute or such
    @type sourcetext: str
    @param disambiguation: used to distinguish between two equal sourcetexts
    int the same context, or as comment, optional, defaults to an empty string,
    must be a raw string, not the return value of another function, an
    attribute or such
    @type disambiguation: str
    @param n: used for strings containing plurals, optional, defaults to -1
    @type n: int
    """
    return QApplication.translate(context, sourcetext, disambiguation, n)


class Translatable(object):
    """
    Baseclass for a class using translatable string literals.
    """
    @classmethod
    def _tr(cls, sourcetext, *, disambiguation="", n=-1, context=None):
        """
        Returns the current translation for a string. This method calls can be
        extracted by pyTSon's pylupdate.py.
        @param sourcetext: translatable string, must be a raw string, not the
        return value of another function, an attribute or such
        @type sourcetext: str
        @param disambiguation: used to distinguish between two equal
        sourcetexts int the same context, or as comment, optional, defaults
        to an empty string, must be a raw string, not the return value of
        another function, an attribute or such
        @type disambiguation: str
        @param n: used for strings containing plurals, optional, defaults to -1
        @type n: int
        @param context: context to use for the string, optional, if set to
        None, the classname is used as context, defaults to None
        @type context: str
        """
        if not context:
            return tr(cls.__name__, sourcetext, disambiguation=disambiguation,
                      n=n)
        else:
            return tr(context, sourcetext, disambiguation=disambiguation, n=n)


def locales():
    """
    Generator function to return all locale codes available for translation
    in format language_country (see ISO 639 and ISO 3166)
    @return: Generator function to return the language codes
    @rtype: Generator[str]
    """
    for f in glob.glob(getPluginPath("ressources", "i18n", "pyTSon-*.qm")):
        m = re.match(r'^pyTSon-(\w\w_\w\w).qm$', os.path.split(f)[-1])
        if m:
            yield m.group(1)


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

    with open(getPluginPath("VERSION"), "r") as f:
        global _PYTSON_VERSION
        _PYTSON_VERSION = f.readline().strip()


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
    return _PYTSON_VERSION


def getCurrentApiVersion():
    """
    Returns the current apiversion of the ts3 plugin sdk. This should not be
    used in ts3plugin.apiVersion. Be fair and update your plugin manually!
    @return: the apiVersion
    @rtype: str
    """
    return 21
