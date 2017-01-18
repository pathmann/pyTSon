import os, shutil, platform
import ts3lib as ts3


def getConfigPath(*args):
    """
    Returns pyTSon's configpath, that is, the subdirectory 'pyTSon' in the TeamSpeak 3 config directory.
    @param args: path fields joined to the result as list of strings
    @type args: list[str]
    @return: The accumulated path
    @rtype: str
    """
    return os.path.join(ts3.getConfigPath(), "pyTSon", *args)

def getPluginPath(*args):
    """
    Returns pyTSon's pluginpath, that is, the subdirectory 'pyTSon' in the TeamSpeak 3 plugins directory.
    @param args: path fields joined to the result as list of strings
    @type args: list[str]
    @return: The accumulated path
    @rtype: str
    """
    return os.path.join(ts3.getPluginPath(), "pyTSon", *args)

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
