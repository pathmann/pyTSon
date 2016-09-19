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

ARCHFILES = {'win32' : [("build/pyTSon.dll", "plugins/pyTSon_win32.dll")], 'win64' : [("build/pyTSon.dll", "plugins/pyTSon_win64.dll")], 'linux_x86' : [("build/libpyTSon.so.1.0.0", "plugins/libpyTSon_linux_x86.so")], 'linux_amd64' : [("build/libpyTSon.so.1.0.0", "plugins/libpyTSon_linux_amd64.so")], 'mac' : [("build/libpyTSon.1.0.0.dylib", "plugins/libpyTSon_mac.dylib")]}

PYTHONFILES = {'win32' : [("python35.dll", "plugins/pyTSon/python35.dll"), ("Lib", "plugins/pyTSon/include/Lib_win32"), ("DLLs/_bz2.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/_bz2.pyd"), ("DLLs/_ctypes.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/_ctypes.pyd"), ("DLLs/_decimal.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/_decimal.pyd"), ("DLLs/_elementtree.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/_elementtree.pyd"), ("DLLs/_hashlib.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/_hashlib.pyd"), ("DLLs/_lzma.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/_lzma.pyd"), ("DLLs/_msi.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/_msi.pyd"), ("DLLs/_multiprocessing.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/_multiprocessing.pyd"), ("DLLs/_overlapped.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/_overlapped.pyd"), ("DLLs/_socket.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/_socket.pyd"), ("DLLs/_ssl.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/_ssl.pyd"), ("DLLs/pyexpat.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/pyexpat.pyd"), ("DLLs/select.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/select.pyd"), ("DLLs/unicodedata.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/unicodedata.pyd"), ("DLLs/winsound.pyd", "plugins/pyTSon/include/Lib_win32/lib-dynload/winsound.pyd"), ],
               'win64' : [("python35.dll", "plugins/pyTSon/python35.dll"), ("Lib", "plugins/pyTSon/include/Lib_win64"), ("DLLs/_bz2.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_bz2.pyd"), ("DLLs/_ctypes.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_ctypes.pyd"), ("DLLs/_decimal.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_decimal.pyd"), ("DLLs/_elementtree.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_elementtree.pyd"), ("DLLs/_hashlib.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_hashlib.pyd"), ("DLLs/_lzma.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_lzma.pyd"), ("DLLs/_msi.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_msi.pyd"), ("DLLs/_multiprocessing.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_multiprocessing.pyd"), ("DLLs/_overlapped.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_overlapped.pyd"), ("DLLs/_socket.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_socket.pyd"), ("DLLs/_sqlite3.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_sqlite3.pyd"), ("DLLs/_ssl.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/_ssl.pyd"), ("DLLs/pyexpat.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/pyexpat.pyd"), ("DLLs/select.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/select.pyd"), ("DLLs/unicodedata.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/unicodedata.pyd"), ("DLLs/winsound.pyd", "plugins/pyTSon/include/Lib_win64/lib-dynload/winsound.pyd") ],
               'linux_x86' : [("lib/libpython3.5m.so.1.0", "plugins/pyTSon/libpython3.5m_32.so"), ("lib/python3.5", "plugins/pyTSon/include/Lib_linux32"), ],
               'linux_amd64' : [("lib/libpython3.5m.so.1.0", "plugins/pyTSon/libpython3.5m_64.so"), ("lib/python3.5", "plugins/pyTSon/include/Lib_linux64"), ],
               'mac' : [("lib/libpython3.5m.dylib", "plugins/pyTSon/libpython3.5m.dylib"), ("lib/python3.5", "plugins/pyTSon/include/Lib"), ]}

INIBASE = """
Name = pyTSon
Type = Plugin
Author = Thomas "PLuS" Pathmann
Version = 1.0.3
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

