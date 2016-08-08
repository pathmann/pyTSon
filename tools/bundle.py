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


FILES = [("ressources/pytsonui.py", "plugins/pyTSon/include/pytsonui.py"), ("ressources/pyTSon-configdialog.ui", "plugins/pyTSon/ressources/pyTSon-configdialog.ui"), ("generated/pregen/ts3plugin.py", "plugins/pyTSon/scripts/ts3plugin.py"), ("generated/pregen/ts3defines.py", "plugins/pyTSon/include/ts3defines.py"), ("docs/api.pdf", "plugins/pyTSon/docs/pyTSon.pdf")]

ARCHFILES = {'win32' : [("build/pyTSon.dll", "plugins/pyTSon_win32.dll")], 'win64' : [("build/pyTSon.dll", "plugins/pyTSon_win64.dll")], 'linux_x86' : [("build/libpyTSon.so.1.0.0", "plugins/libpyTSon_linux_x86.so")], 'linux_amd64' : [("build/libpyTSon.so.1.0.0", "plugins/libpyTSon_linux_amd64.so")], 'mac' : [("", "")]}

PYTHONFILES = {'win32' : [], 'win64' : [], 'linux_x86' : [("lib/libpython3.5m.so.1.0", "libpython3.5m.so.1.0"), ("lib/python3.5", "plugins/pyTSon/include/Lib"), ], 'linux_amd64' : [("lib/libpython3.5m.so.1.0", "libpython3.5m.so.1.0"), ("lib/python3.5", "plugins/pyTSon/include/Lib"), ], 'mac' : [("lib/libpython3.5m.so.1.0", "libpython3.5m.so.1.0"), ("lib/python3.5", "plugins/pyTSon/include/Lib"), ]}

INIBASE = """
Name = pyTSon
Type = Plugin
Author = Thomas "PLuS" Pathmann
Version = 1.0.0
Platforms = %s
Description = "pyTSon - A python plugin to enhance the TS3 client with python scripts"
"""


def main(root, pythondir, outdir, arches):
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
                        
        for loc, inzip in PYTHONFILES[a]:
            locpath = os.path.join(pythondir, loc)
            
            if os.path.isfile(locpath):
                zip.write(locpath, inzip)
            else:
                for base, dirs, files in os.walk(locpath):
                    if not os.path.basename(base) == "__pycache__":
                        for f in files:
                            fn = os.path.join(base, f)
                            zip.write(fn, inzip + fn[len(locpath):])
            
        zip.writestr("package.ini", INIBASE % a)
        zip.close()

if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument('rootdir', help='The root directory of pyTSon')
    parser.add_argument('pythondir', help='The directory, python is installed in')
    parser.add_argument('outputdir', help='The directory, where output files should be placed in')
    parser.add_argument('arch', nargs='+', help='architecture to bundle')
    
    args = parser.parse_args()
    
    for key in args.arch:
        if key not in ARCHFILES:
            print("Unrecognized architecture, possible values are: %s" % ", ".join(ARCHFILES.keys()))
            sys.exit(1)
            
    main(args.rootdir, args.pythondir, args.outputdir, args.arch)

