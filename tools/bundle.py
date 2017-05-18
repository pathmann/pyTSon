#!/usr/bin/env python3

import sys
import os
import zipfile
import shutil

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
         ("VERSION", "plugins/pyTSon/VERSION")]

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
                         ("Lib", "plugins/pyTSon/lib_new"),
                         ("DLLs/_bz2.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_bz2.pyd"),
                         ("DLLs/_ctypes.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_ctypes.pyd"),
                         ("DLLs/_decimal.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_decimal.pyd"),
                         ("DLLs/_elementtree.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_elementtree.pyd"),
                         ("DLLs/_hashlib.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_hashlib.pyd"),
                         ("DLLs/_lzma.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_lzma.pyd"),
                         ("DLLs/_msi.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_msi.pyd"),
                         ("DLLs/_multiprocessing.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_multiprocessing.pyd"),
                         ("DLLs/_overlapped.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_overlapped.pyd"),
                         ("DLLs/_socket.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_socket.pyd"),
                         ("DLLs/_ssl.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_ssl.pyd"),
                         ("DLLs/pyexpat.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/pyexpat.pyd"),
                         ("DLLs/select.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/select.pyd"),
                         ("DLLs/unicodedata.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/unicodedata.pyd"),
                         ("DLLs/winsound.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/winsound.pyd"),
                         ("include", "plugins/pyTSon/include/python3.5m"), ],
               'win64': [("python35.dll", "plugins/pyTSon/python35.dll"),
                         ("Lib", "plugins/pyTSon/lib_new"),
                         ("DLLs/_bz2.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_bz2.pyd"),
                         ("DLLs/_ctypes.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_ctypes.pyd"),
                         ("DLLs/_decimal.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_decimal.pyd"),
                         ("DLLs/_elementtree.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_elementtree.pyd"),
                         ("DLLs/_hashlib.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_hashlib.pyd"),
                         ("DLLs/_lzma.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_lzma.pyd"),
                         ("DLLs/_msi.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_msi.pyd"),
                         ("DLLs/_multiprocessing.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_multiprocessing.pyd"),
                         ("DLLs/_overlapped.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_overlapped.pyd"),
                         ("DLLs/_socket.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_socket.pyd"),
                         ("DLLs/_sqlite3.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_sqlite3.pyd"),
                         ("DLLs/_ssl.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/_ssl.pyd"),
                         ("DLLs/pyexpat.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/pyexpat.pyd"),
                         ("DLLs/select.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/select.pyd"),
                         ("DLLs/unicodedata.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/unicodedata.pyd"),
                         ("DLLs/winsound.pyd",
                          "plugins/pyTSon/lib_new/lib-dynload/winsound.pyd"),
                         ("include", "plugins/pyTSon/include/python3.5m"), ],
               'linux_x86': [("lib/libpython3.5m.so.1.0",
                              "plugins/pyTSon/libpython3.5m_32.so"),
                             ("lib/python3.5",
                              "plugins/pyTSon/lib_new/python3.5"),
                             ("include", "plugins/pyTSon/include"), ],
               'linux_amd64': [("lib/libpython3.5m.so.1.0",
                                "plugins/pyTSon/libpython3.5m_64.so"),
                               ("lib/python3.5",
                                "plugins/pyTSon/lib_new/python3.5"),
                               ("include", "plugins/pyTSon/include"), ],
               'mac': [("lib/libpython3.5m.dylib",
                        "plugins/pyTSon/libpython3.5m.dylib"),
                       ("lib/python3.5", "plugins/pyTSon/lib_new/python3.5"),
                       ("include", "plugins/pyTSon/include"), ]}

INIBASE = """
Name = pyTSon
Type = Plugin
Author = Thomas \\"PLuS\\" Pathmann
Version = 1.2.1
Platforms = %s
Description = "pyTSon - A python plugin to enhance the TS3 client with python\
scripts"
"""


def writeFiles(root, files, tozip):
    for loc, inzip in files:
        locpath = os.path.join(root, loc)

        if os.path.isfile(locpath):
            tozip.write(locpath, inzip)
        else:
            for base, dirs, files in os.walk(locpath):
                if not os.path.basename(base) == "__pycache__":
                    for f in files:
                        fn = os.path.join(base, f)
                        tozip.write(fn, inzip + fn[len(locpath):])


def main(root, pythondir, outdir, arches, buildbase, update):
    for a in arches:
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

        if update:
            writeFiles(root, FILES, zipout)
        elif buildbase:
            writeFiles(root, ARCHFILES[a], zipout)
            writeFiles(pythondir, PYTHONFILES[a], zipout)
        else:
            writeFiles(root, FILES + ARCHFILES[a], zipout)
            writeFiles(pythondir, PYTHONFILES[a], zipout)

        if not buildbase:
            zipout.writestr("package.ini", INIBASE % a)

        zipout.close()


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
