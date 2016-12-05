import os, shutil
import ts3

from itertools import chain
from zipfile import ZipFile
from xmlrpc.client import ServerProxy

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

    fp = os.path.join(p, ename + ".py")
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
        self.pypi = ServerProxy("https://pypi.python.org/pypi")
        self.stdout = stdout

        self.working = False

    def _print(self, msg):
        if self.stdout:
            self.stdout(msg)
        else:
            print(msg)

    def _downloadUrl(self, package, version):
        l = self.pypi.release_urls(package, version)
        if len(l) > 0:
            return l[0]["url"]
        raise Exception("No such package download found")

    def _latestVersion(self, package):
        l = self.pypi.package_releases(package)
        if len(l) > 0:
            return l[0]
        raise Exception("No such package release found")

    def _dependencies(self, package, version, localpkgdir):
        pass

    def _install(self, package, version, localpackagedir):
        pass

    def _downloadFinished(self, pkg, reply):
        if reply.error() == QNetworkReply.NoErro:
            arch = io.BytesIO(reply.readAll())

        else:
            self.working = False
            raise Exception("Download of %s failed with %d" % (pkg, reply.error()))

    def _removePackage(self, package, version, localpackagedir):
        pass

    def getInstalledPackages(path):
        """
        Returns a dict {'pkg': 'version'}
        """
        pass

    def installPlugin(addon, data, iszip):
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
        self._print("Directory created. Installing dependencies ...")

        self.installPackages(os.path.abspath(fp), addon["dependencies"])
        self._print("Done. Installing files ...")

        ext = os.path.splitext(fpath)
        if iszip:
            with ZipFile(data) as zip:
                zip.extractall(os.path.abspath(fp))
        else:
            with open(fp, "w") as f:
                f.write(data)

        self._print("Plugin installed.")

    def installPackages(path, deps):
        """

        """
        if len(deps) == 0:
            return

        self.deppath = os.path.join(path, "include")
        if not os.path.isdir(self.deppath):
            os.mkdir(self.deppath)

        self.curdeps = self.getInstalledPackages(deppath)
        #if this queue is empty again, we are done
        self.queue = []

        for d in deps:
            (pkg, *version) = d.split("==")

            if version:
                version = version[0]
            else:
                version = self._latestVersion(pkg)

            if pkg in curdeps:
                if curdeps[pkg] == version:
                    self._print("Omitting dependency %s, already installed" % pkg)
                    continue
                else:
                    #update, so remove existing dep first
                    self._removePackage(pkg, curdeps[pkg], self.deppath)

            self.queue.append((pkg, version))
            r = self.nwm.get(QNetworkRequest(QUrl(self._downloadUrl(pkg, version))))
            r.connect("finished(QNetworkReply*)", lambda x: self._downloadFinished(pkg, x))
            #download pkg==version
            #read requires: installPackages(name, requires)
            #install files
            pass
