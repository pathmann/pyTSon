import ts3lib
import ts3defines

from pluginhost import PluginHost


class PluginMount(type):
    def __init__(cls, name, bases, attrs):
        super(PluginMount, cls).__init__(name, bases, attrs)
        if not hasattr(PluginHost, 'plugins'):
            PluginHost.plugins = {}
            PluginHost.active = {}
        else:
            for a in ['requestAutoload', 'name', 'version', 'apiVersion',
                      'author', 'description', 'offersConfigure',
                      'commandKeyword', 'infoTitle', 'menuItems', 'hotkeys']:
                if not hasattr(cls, a):
                    err = ts3lib.logMessage("Plugin %s not loaded, missing "
                                            "required attribute %s" % (name, a),
                                            ts3defines.LogLevel.LogLevel_ERROR,
                                            "pyTSon.PluginMount.init", 0)
                    if err != ts3defines.ERROR_ok:
                        print("Plugin %s not loaded, missing required "
                              "attribute %s" % (name, a))

                    return

            if cls.name not in PluginHost.plugins:
                PluginHost.plugins[cls.name] = cls
            else:
                err = ts3lib.logMessage("Error loading python plugin %s, "
                                        "already registered or a plugin with "
                                        "that name already exist" % cls.name,
                                        ts3defines.LogLevel.LogLevel_ERROR,
                                        "pyTSon.PluginMount.init", 0)
                if err != ts3defines.ERROR_ok:
                    print("Error loading python plugin %s, already registered "
                          "or a plugin with that name already exist" % cls.name)


class ts3plugin(object, metaclass=PluginMount):
    """
    requestAutoload = False
    name = "__ts3plugin__"
    version = "1.0"
    apiVersion = 21
    author = "Thomas \"PLuS\" Pathmann"
    description = "This is the baseclass for all ts3 python plugins"
    offersConfigure = False
    commandKeyword = "py"
    infoTitle = "pyTSon" #pass None to not show any info
    menuItems = []#[(ts3defines.PluginMenuType.PLUGIN_MENU_TYPE_CLIENT, 0,
                    "text", "icon.png")]
    hotkeys = []#[("keyword", "description")]
    """

    def __init__(self):
        pass

    def stop(self):
        pass
