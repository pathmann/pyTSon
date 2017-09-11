#!/usr/bin/env python3

import sys
import os
import zipfile
import shutil
import tempfile
import subprocess

from argparse import ArgumentParser

# [("path to file on disk", "path (without file) in zip")]


FILES = [("ressources/octicons", "plugins/pyTSon/ressources/octicons"),
         ("ressources/python.png", "plugins/pyTSon/python.png"),
         ("ressources/python/pytsonui", "plugins/pyTSon/include/pytsonui"),
         ("ressources/python/pytson.py", "plugins/pyTSon/include/pytson.py"),
         ("ressources/python/devtools.py",
          "plugins/pyTSon/include/devtools.py"),
         ("ressources/python/ts3client.py",
          "plugins/pyTSon/include/ts3client.py"),
         ("ressources/ui/pyTSon-configdialog.ui",
          "plugins/pyTSon/ressources/pyTSon-configdialog.ui"),
         ("ressources/ui/repository.ui",
          "plugins/pyTSon/ressources/repository.ui"),
         ("ressources/ui/installer.ui",
          "plugins/pyTSon/ressources/installer.ui"),
         ("ressources/repositorymaster.json",
          "plugins/pyTSon/ressources/repositorymaster.json"),
         ("ressources/python/ts3plugin.py",
          "plugins/pyTSon/scripts/ts3plugin.py"),
         ("ressources/python/pluginhost.py",
          "plugins/pyTSon/scripts/pluginhost.py"),
         ("generated/pregen/ts3defines.py",
          "plugins/pyTSon/include/ts3defines.py"),
         ("generated/pregen/api.pdf", "plugins/pyTSon/docs/pyTSon.pdf"),
         ("generated/pregen/ts3help.py", "plugins/pyTSon/include/ts3help.py"),
         ("ressources/python/ts3widgets", "plugins/pyTSon/include/ts3widgets"),
         ("ressources/loading.gif", "plugins/pyTSon/ressources/loading.gif"),
         ("ressources/python/signalslot.py",
          "plugins/pyTSon/include/signalslot.py"),
         ("generated/pregen/pyTSon-de_DE.qm",
          "plugins/pyTSon/ressources/i18n/pyTSon-de_DE.qm"),
         ("Changelog.html", "plugins/pyTSon/Changelog.html"),
         ("tools/pylupdate.py", "plugins/pyTSon/include/pylupdate.py"),
         ("LICENSE", "plugins/pyTSon/LICENSE.txt"),
         ("VERSION", "plugins/pyTSon/VERSION"),
         ("ressources/python/ts3lib.py", "plugins/pyTSon/include/ts3lib.py"),
         ("ressources/python/_plugincmd.py",
          "plugins/pyTSon/include/_plugincmd.py")]

ARCHFILES = {'win32': [("build/pyTSon.dll", "plugins/pyTSon_win32.dll"),
                       ("build/python.exe", "plugins/pyTSon/python.exe")],
             'win64': [("build/pyTSon.dll", "plugins/pyTSon_win64.dll"),
                       ("build/python.exe", "plugins/pyTSon/python.exe")],
             'linux_x86': [("build/libpyTSon.so.1.0.0",
                            "plugins/libpyTSon_linux_x86.so"),
                           ("build/python", "plugins/pyTSon/python")],
             'linux_amd64': [("build/libpyTSon.so.1.0.0",
                              "plugins/libpyTSon_linux_amd64.so"),
                             ("build/python", "plugins/pyTSon/python")],
             'mac': [("build/libpyTSon.1.0.0.dylib",
                      "plugins/libpyTSon_mac.dylib"),
                     ("build/python", "plugins/pyTSon/python")]}

PYTHONFILES = {'win32': [("python35.dll", "plugins/pyTSon/python35.dll"),
                         ("Lib", "plugins/pyTSon/lib"),
                         ("DLLs/_bz2.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_bz2.pyd"),
                         ("DLLs/_ctypes.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_ctypes.pyd"),
                         ("DLLs/_decimal.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_decimal.pyd"),
                         ("DLLs/_elementtree.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_elementtree.pyd"),
                         ("DLLs/_hashlib.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_hashlib.pyd"),
                         ("DLLs/_lzma.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_lzma.pyd"),
                         ("DLLs/_msi.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_msi.pyd"),
                         ("DLLs/_multiprocessing.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_multiprocessing.pyd"),
                         ("DLLs/_overlapped.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_overlapped.pyd"),
                         ("DLLs/_socket.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_socket.pyd"),
                         ("DLLs/_ssl.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_ssl.pyd"),
                         ("DLLs/pyexpat.pyd",
                          "plugins/pyTSon/lib/lib-dynload/pyexpat.pyd"),
                         ("DLLs/select.pyd",
                          "plugins/pyTSon/lib/lib-dynload/select.pyd"),
                         ("DLLs/unicodedata.pyd",
                          "plugins/pyTSon/lib/lib-dynload/unicodedata.pyd"),
                         ("DLLs/winsound.pyd",
                          "plugins/pyTSon/lib/lib-dynload/winsound.pyd"),
                         ("include", "plugins/pyTSon/include/python3.5m"), ],
               'win64': [("python35.dll", "plugins/pyTSon/python35.dll"),
                         ("Lib", "plugins/pyTSon/lib"),
                         ("DLLs/_bz2.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_bz2.pyd"),
                         ("DLLs/_ctypes.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_ctypes.pyd"),
                         ("DLLs/_decimal.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_decimal.pyd"),
                         ("DLLs/_elementtree.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_elementtree.pyd"),
                         ("DLLs/_hashlib.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_hashlib.pyd"),
                         ("DLLs/_lzma.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_lzma.pyd"),
                         ("DLLs/_msi.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_msi.pyd"),
                         ("DLLs/_multiprocessing.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_multiprocessing.pyd"),
                         ("DLLs/_overlapped.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_overlapped.pyd"),
                         ("DLLs/_socket.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_socket.pyd"),
                         ("DLLs/_sqlite3.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_sqlite3.pyd"),
                         ("DLLs/_ssl.pyd",
                          "plugins/pyTSon/lib/lib-dynload/_ssl.pyd"),
                         ("DLLs/pyexpat.pyd",
                          "plugins/pyTSon/lib/lib-dynload/pyexpat.pyd"),
                         ("DLLs/select.pyd",
                          "plugins/pyTSon/lib/lib-dynload/select.pyd"),
                         ("DLLs/unicodedata.pyd",
                          "plugins/pyTSon/lib/lib-dynload/unicodedata.pyd"),
                         ("DLLs/winsound.pyd",
                          "plugins/pyTSon/lib/lib-dynload/winsound.pyd"),
                         ("include", "plugins/pyTSon/include/python3.5m"), ],
               'linux_x86': [("lib/libpython3.5m.so.1.0",
                              "plugins/pyTSon/libpython3.5m_32.so"),
                             ("lib/python3.5",
                              "plugins/pyTSon/lib/python3.5"),
                             ("include", "plugins/pyTSon/include"), ],
               'linux_amd64': [("lib/libpython3.5m.so.1.0",
                                "plugins/pyTSon/libpython3.5m_64.so"),
                               ("lib/python3.5",
                                "plugins/pyTSon/lib/python3.5"),
                               ("include", "plugins/pyTSon/include"), ],
               'mac': [("lib/libpython3.5m.dylib",
                        "plugins/pyTSon/libpython3.5m.dylib"),
                       ("lib/python3.5", "plugins/pyTSon/lib/python3.5"),
                       ("include", "plugins/pyTSon/include"), ]}

INIBASE = """
Name = pyTSon
Type = Plugin
Author = Thomas \\"PLuS\\" Pathmann
Version = %s
Platforms = %s
Description = "pyTSon - A python plugin to enhance the TS3 client with python\
scripts"
"""


def copyFiles(root, files, tempdir):
    for loc, inzip in files:
        locpath = os.path.join(root, loc)

        if os.path.isfile(locpath):
            filepath = os.path.join(tempdir, inzip)
            filedir = os.path.dirname(filepath)

            if not os.path.isdir(filedir):
                os.makedirs(filepath)
            shutil.copy(locpath, filepath)
        else:
            for base, dirs, files in os.walk(locpath):
                for f in files:
                    fn = os.path.join(base, f)

                    filepath = os.path.join(tempdir, inzip +
                                            fn[len(locpath):])
                    filedir = os.path.dirname(filepath)
                    if not os.path.isdir(filedir):
                        os.makedirs(filedir)
                    shutil.copy(fn, filepath)


def writeZip(tempdir, tozip):
    for base, dirs, files in os.walk(tempdir):
        if not os.path.basename(base) == "__pycache__":
            for f in files:
                fn = os.path.join(base, f)
                tozip.write(fn, fn[len(tempdir):])


def main(root, pythondir, outdir, arches, buildbase, update):
    verpath = os.path.join(root, "VERSION")
    if not os.path.isfile(verpath):
        print("Could not find VERSION file in rootdir")
        sys.exit(1)

    with open(verpath, "r") as f:
        ver = f.readline()

    for a in arches:
        tempdir = tempfile.mkdtemp()

        if update:
            copyFiles(root, FILES, tempdir)
        elif buildbase:
            copyFiles(root, ARCHFILES[a], tempdir)
            copyFiles(pythondir, PYTHONFILES[a], tempdir)
        else:
            copyFiles(root, FILES + ARCHFILES[a], tempdir)
            copyFiles(pythondir, PYTHONFILES[a], tempdir)

        if not update:
            stddir = os.path.join(tempdir, "plugins", "pyTSon", "lib")
            if not os.path.isdir(stddir):
                print("The python standard library coult not be found, check "
                      "%s for the directory structure" % tempdir)

            if a in ["win32", "win64"]:
                intpath = os.path.join(tempdir, "plugins", "pyTSon",
                                       "python.exe")
            else:
                intpath = os.path.join(tempdir, "plugins", "pyTSon", "python")

            if not os.path.isfile(intpath):
                print("The python interpreter could not be found, check %s "
                      "for the directory structure" % tempdir)
                sys.exit(1)

            if a in ["win32", "win64"]:
                sitedir = os.path.join(stddir, "site-packages")
            else:
                sitedir = os.path.join(stddir, "python3.5", "site-packages")
            if not os.path.isdir(sitedir):
                print("The site directory could not be found, check "
                      "%s for the directory structure" % tempdir)
                sys.exit(1)

            # update pip: python -m pip install --update --target sitedir pip
            p = subprocess.Popen([intpath, "-m", "pip", "install", "--target",
                                  sitedir, "--upgrade", "pip"],
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE)
            out, err = p.communicate()
            if err:
                print("Error updating pip:")
                print(err)
                if out:
                    print(out)
                sys.exit(1)
            elif out:
                print(out)

            reqfile = os.path.join(root, "requirements.txt")
            if os.path.isfile(reqfile):
                p = subprocess.Popen([intpath, "-m", "pip", "install",
                                      "--target", sitedir, "-r", reqfile],
                                     stdout=subprocess.PIPE,
                                     stderr=subprocess.PIPE)
                out, err = p.communicate()
                if err:
                    print("Error installing requirements:")
                    print(err)
                    if out:
                        print(out)
                    sys.exit(1)
                elif out:
                    print(out)
            else:
                print("Warning: No requirements.txt found")

            # move the lib dir to lib_new
            shutil.move(stddir, os.path.join(tempdir, "plugins", "pyTSon",
                                             "lib_new"))

        if update:
            shutil.copyfile(os.path.join(outdir, "pyTSon_%s.base" % a),
                            os.path.join(outdir, "pyTSon_%s.ts3_plugin" % a))
            zipout = zipfile.ZipFile(os.path.join(outdir,
                                                  "pyTSon_%s.ts3_plugin" % a),
                                     "a", zipfile.ZIP_DEFLATED)
        elif buildbase:
            zipout = zipfile.ZipFile(os.path.join(outdir,
                                                  "pyTSon_%s.base" % a),
                                     "w", zipfile.ZIP_DEFLATED)
        else:
            zipout = zipfile.ZipFile(os.path.join(outdir,
                                                  "pyTSon_%s.ts3_plugin" % a),
                                     "w", zipfile.ZIP_DEFLATED)

        writeZip(tempdir, zipout)
        if not buildbase:
            zipout.writestr("package.ini", INIBASE % (ver, a))

        zipout.close()
        shutil.rmtree(tempdir)


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument('rootdir', help='The root directory of pyTSon')
    parser.add_argument('pythondir',
                        help='The directory, python is installed in')
    parser.add_argument('outputdir',
                        help='The directory, where output files should be \
                        placed in')
    parser.add_argument('arch', nargs='+', help='architecture to bundle')
    parser.add_argument('-u', '--update', dest='update', action='store_true',
                        help='Create a bundle by merging a base arch bundle \
                        with the non-arch-dependent files, argument pythondir \
                        will be ignored')
    parser.add_argument('-b', '--base', dest='base', action='store_true',
                        help='Create a base arch bundle')

    args = parser.parse_args()

    if args.update and args.base:
        print("update and base are mutual exclusive")
        sys.exit(1)

    if not args.update and len(args.arch) > 1:
        print("Only updates are supported on foreign platforms")
        sys.exit(1)

    for key in args.arch:
        if key not in ARCHFILES:
            print("Unrecognized architecture, possible values are: \
                  %s" % ", ".join(ARCHFILES.keys()))
            sys.exit(1)
        elif args.update:
            path = os.path.join(args.outputdir, "pyTSon_%s.base" % key)
            if not os.path.isfile(path):
                print("Could not find base arch bundle in outputdir for \
                       architecture %s" % key)
                sys.exit(1)

    main(args.rootdir, args.pythondir, args.outputdir, args.arch, args.base,
         args.update)
