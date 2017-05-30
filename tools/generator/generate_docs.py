#!/usr/bin/env python3

import sys
import os
import glob

from argparse import ArgumentParser

DOC_FILES = ["ts3lib.cpp", "ts3plugin.cpp"]
EXTRA_FILES = [("..", "..", "ressources", "python", "pytsonui"),
               ("..", "..", "ressources", "python", "pytson.py"),
               ("..", "..", "ressources", "python", "devtools.py"),
               ("..", "..", "ressources", "python", "ts3client.py"),
               ("..", "..", "ressources", "python", "pluginhost.py"),
               ("..", "..", "ressources", "python", "ts3widgets"),
               ("..", "..", "ressources", "python", "pythonqtpytson.py"),
               ("..", "..", "ressources", "python", "signalslot.py"),
               ("..", "..", "tools", "pylupdate.py"),
               ("..", "..", "generated", "pregen", "ts3defines.py")]


def main(inpdir, outdir):
    outfiles = [os.path.join(inpdir, os.path.join(*t)) for t in EXTRA_FILES]
    for fname in [os.path.join(inpdir, f) for f in DOC_FILES]:
        if not os.path.exists(fname):
            print("File %s does not exist" % fname)
            sys.exit(1)

        incomment = False

        with open(fname, "r") as inf:
            outfiles.append(os.path.join(outdir, os.path.splitext(
                            os.path.basename(fname))[0] + ".py"))
            with open(outfiles[-1], "w") as outf:
                for l in inf:
                    if not incomment and l.strip() == "/*":
                        incomment = True
                    elif incomment and l.strip() == "*/":
                        incomment = False
                        outf.write("\n")
                    elif incomment:
                        outf.write(l)

    thisdir = os.path.dirname(__file__)
    patches = glob.glob(os.path.join(thisdir, "*.patch"))

    for patch in patches:
        os.system("patch -p1 < %s" % patch)

    confpath = os.path.join(thisdir, "epydoc.conf")
    os.system("epydoc %s --parse-only -o %s/html --html --config %s" %
              (" ".join(outfiles), outdir, confpath))
    os.system("epydoc %s --parse-only -o %s/pdf --pdf --config %s" %
              (" ".join(outfiles), outdir, confpath))

    for patch in patches:
        os.system("patch -p1 -R < %s" % patch)


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument('inputdir', help='The directory, where input files \
                        live in')
    parser.add_argument('outputdir', help='The directory, processed files \
                        should be placed in')

    args = parser.parse_args()

    if not os.path.exists(args.inputdir):
        print("input directory does not exist")
        sys.exit(1)

    if not os.path.exists(args.outputdir):
        print("output directory does not exist")
        sys.exit(1)

    main(args.inputdir, args.outputdir)
