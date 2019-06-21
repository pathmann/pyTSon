[![Build Status](https://build.4qt.de/buildStatus/icon?job=pyTSon)](https://build.4qt.de/job/pyTSon/) [![Gitter chat](https://badges.gitter.im/pyTSon-ts3/gitter.png)](https://gitter.im/pyTSon-ts3/Lobby) [![PayPal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=thomas.pathmann%40gmail%2ecom&lc=GB&item_name=pyTSon&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted)

# pyTSon
pyTSon is a plugin for the TeamSpeak 3 client, it offers a python3 interface for the plugin sdk.
pyTSon has a minimum C/C++ part, so that the main plugin runs in embedded python.

Current used python version is Python 3.5.2.

Batteries included
==================
The python standard library is included, either built in the plugin or bundled in the lib directory (which is in sys.path by default).
pyTSon has PythonQt included, so that scripts can have UIs written in python, see pyTSon's own configdialog or the scripting console for examples.

Dependencies
============
pyTSon has multiple layers of dependencies listed below.

Generating Code
---------------
* [Python3](https://www.python.org/download/releases/3.0/)
* [pycparser](https://github.com/eliben/pycparser/)
* [jinja2](http://jinja.pocoo.org)

Generating Documentation
------------------------
* [Python3](https://www.python.org/download/releases/3.0/)
* [epydoc](http://epydoc.sourceforge.net) (which is only available for Python2, so we'll switch to another doctool in the future)

Building
--------
* [Qt 5.12.1](https://download.qt.io/archive/qt/5.12/5.12.1/) (which is the version, the TeamSpeak 3 client is currently using)
* [Python 3.5.2](https://www.python.org/downloads/release/python-352/#Files) (configure with --enable-shared)
* [PythonQt](https://github.com/pathmann/PythonQt)
* [patchelf](http://nixos.org/patchelf.html) (linux; Version >= 0.9)

Running
-------
* None! (At least none you have to care about, python is deployed with the plugin)

How to use
==========
1. Get the latest release for your Teamspeak client [here](https://github.com/pathmann/pyTSon/releases/) or compile the source yourself.
3. Restart your Teamspeak Client.
4. Download or write any pyTSon script and place it in `%APPDATA%\TS3Client\plugins\pyTSon\scripts\<your-plugin-dir>` on windows resp. `~/.ts3client/plugins/pyTSon/scripts/<your-plugin-dir>` on unix.
5. In your client click on "Plugins" => "pyTSon" => "Settings" and on the settings dialog click on "Reload All" and check the plugins checkbox if it's not checked already.

How to build
============
* Adjust the python include and lib path in [python.pri](https://github.com/pathmann/pyTSon/blob/master/python.pri) or run qmake with `qmake [...] PYTHONPATH=<your-python-path>`
* Download/Clone PythonQt from [here](https://github.com/pathmann/PythonQt) to includes/
```
qmake
(n)make
```
Done

How to develop a python plugin
==============================
Create a *.py file in \<TeamSpeak 3 client install\>/plugins/pyTSon/scripts/\<your-plugin-dir\> with a subclass of ts3plugin.
Needed class attributes are requestAutoload, name, version, apiVersion, author, description, offersConfigure, commandKeyword, infoTitle, menuItems, hotkeys.
Otherwise pyTSon refuses to load the plugin. See ts3plugin.py or the documentation for a description of each attribute.

On load, pyTSon will create an instance of your class (call the constructor), after that callbacks are called (if available) as methods of the created instance, eg if you were poked, onClientPokeEvent(self, schid, fromClientID, pokerName, pokerUniqueIdentity, message, ffIgnored) will be called.
On unload, pyTSon will call stop and delete the instance of your class.

TeamSpeak 3's library functions are available with the ts3lib module (eg err, myid = ts3lib.getClientID(schid)).
Constants are available in the ts3defines module (see ts3defines.py in include/ directory).

Below is a small example plugin:
```python
from ts3plugin import ts3plugin

import ts3lib, ts3defines

class testplugin(ts3plugin):
    name = "test"
    requestAutoload = False
    version = "1.0"
    apiVersion = 21
    author = "Thomas \"PLuS\" Pathmann"
    description = "This is a testplugin"
    offersConfigure = True
    commandKeyword = ""
    infoTitle = ""
    menuItems = []#[(ts3defines.PluginMenuType.PLUGIN_MENU_TYPE_CLIENT, 0, "text", "icon.png")]
    hotkeys = []#[("keyword", "description")]

    def __init__(self):
        ts3lib.printMessageToCurrentTab("Yay, we are running!")

    def stop(self):
        ts3lib.printMessageToCurrentTab("Oh no, we were stopped :(")

    def onNewChannelEvent(self, schid, channelID, channelParentID):
        err, name = ts3lib.getChannelVariableAsString(schid, channelID, ts3defines.ChannelProperties.CHANNEL_NAME)
        if err == ts3defines.ERROR_ok:
            ts3lib.printMessageToCurrentTab("new channel %s" % name)
        else:
            ts3lib.printMessageToCurrentTab("got error %s" % err)
```
Repository
==========
pyTSon can load plugins from online repositories. [pyTSon master](https://github.com/pathmann/pyTSon_repository) is included by default. Check [the Readme](https://github.com/pathmann/pyTSon_repository/blob/master/README.md) for more information, if you want to setup your own. 

Releases
========
Personally I work on my linux amd64 machine, other platforms are only tested for basic running, but of course feel free to open an issue, if there is something wrong on your platform and I'll have a look into it.

Check the [release page](https://github.com/pathmann/pyTSon/releases) or myteamspeak.com within your client.
If you are interested in the most recent version, check [this repository](https://repo.4qt.de/pyTSon/nightlies/) for automatic (untested) builds.

