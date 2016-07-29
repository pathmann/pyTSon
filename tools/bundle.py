#!/usr/bin/env python3

import sys, os
import zipfile

from argparse import ArgumentParser

#[("path to file on disk", "path (without file) in zip")]
#pytsonui.py
#pyTSon-configialog.ui
#Lib
#ts3plugin.py
#ts3defines.py
#docs


FILES = [("ressources/pytsonui.py", "plugins/pyTSon/include/pytsonui.py"), ("ressources/pyTSon-configdialog.ui", "plugins/pyTSon/ressources/pyTSon-configdialog.ui"), ("ressources/Lib", "plugins/pyTSon/include/Lib"), ("generated/pregen/ts3plugin.py", "plugins/pyTSon/scripts/ts3plugin.py"), ("generated/pregen/ts3defines.py", "plugins/pyTSon/include/ts3defines.py"), ("docs/api.pdf", "plugins/pyTSon/docs/pyTSon.pdf")]

ARCHFILES = {'win32' : [("build/release/pyTSon.dll", "plugins/pyTSon_win32.dll")], 'win64' : [("build/release/pyTSon.dll", "plugins/pyTSon_win64.dll")], 'linux_x86' : [("build/libpyTSon.so.1.0.0", "plugins/libpyTSon_linux_x86.so")], 'linux_amd64' : [("build/libpyTSon.so.1.0.0", "plugins/libpyTSon_linux_amd64.so")], 'mac' : [("", "")]}

INIBASE = """
Name = pyTSon
Type = Plugin
Author = Thomas "PLuS" Pathmann
Version = 1.0.0
Platforms = %s
Description = "pyTSon - A python plugin to enhance the TS3 client with python scripts"
"""


def main(root, outdir, arches):
    for a in arches:
        zip = zipfile.ZipFile(os.path.join(outdir, "pyTSon_%s.ts3_plugin" % a), "w", zipfile.ZIP_DEFLATED)
        for loc, inzip in FILES + ARCHFILES[a]:
            locpath = os.path.join(root, loc)
            
            if os.path.isfile(locpath):
                zip.write(locpath, inzip)
            else:
                for base, dirs, files in os.walk(locpath):
                    for f in files:
                        fn = os.path.join(base, f)
                        zip.write(fn, inzip + fn[len(locpath):])
            
        zip.writestr("package.ini", INIBASE % a)
        zip.close()

if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument('rootdir', help='The root directory of pyTSon')
    parser.add_argument('outputdir', help='The directory, where output files should be placed in')
    parser.add_argument('arch', nargs='+', help='architecture to bundle')
    
    args = parser.parse_args()
    
    for key in args.arch:
        if key not in ARCHFILES:
            print("Unrecognized architecture, possible values are: %s" % ", ".join(ARCHFILES.keys()))
            sys.exit(1)
            
    main(args.rootdir, args.outputdir, args.arch)

