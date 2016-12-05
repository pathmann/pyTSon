#!/usr/bin/env python3

import sys, os
import logging
from argparse import ArgumentParser
from jinja2 import Environment, FileSystemLoader, StrictUndefined
from pytson import parsing, wrappers


DEFINES_SOURCES = [["include", "plugin_definitions.h"],
                   ["include", "teamlog", "logtypes.h"],
                   ["include", "teamspeak", "clientlib_publicdefinitions.h"],
                   ["include", "teamspeak", "public_definitions.h"],
                   ["include", "teamspeak", "public_errors.h"],
                   ["include", "teamspeak", "public_errors_rare.h"],
                   ["include", "teamspeak", "public_rare_definitions.h"]]

PYPLUGIN_CALLBACK_BLACKLIST = ["ts3plugin_freeMemory", "ts3plugin_registerPluginID", "ts3plugin_setFunctionPointers",
                               "ts3plugin_name", "ts3plugin_version", "ts3plugin_apiVersion", "ts3plugin_author",
                               "ts3plugin_description", "ts3plugin_init", "ts3plugin_offersConfigure",
                               "ts3plugin_commandKeyword", "ts3plugin_processCommand", "ts3plugin_infoTitle",
                               "ts3plugin_requestAutoload", "ts3plugin_onServerErrorEvent", "ts3plugin_onTextMessageEvent",
                               "ts3plugin_onClientPokeEvent", "ts3plugin_onServerPermissionErrorEvent", "ts3plugin_initHotkeys",
                               "ts3plugin_initMenus", "ts3plugin_shutdown", "ts3plugin_configure", "ts3plugin_infoData",
                               "ts3plugin_onUserLoggingMessageEvent", "ts3plugin_onEditPlaybackVoiceDataEvent",
                               "ts3plugin_onEditPostProcessVoiceDataEvent", "ts3plugin_onEditMixedPlaybackVoiceDataEvent",
                               "ts3plugin_onEditCapturedVoiceDataEvent", "ts3plugin_onCustom3dRolloffCalculationClientEvent",
                               "ts3plugin_onCustom3dRolloffCalculationWaveEvent", "ts3plugin_onFileTransferStatusEvent"]

MODULE_FUNCTION_BLACKLIST = ["freeMemory"]


log = None


def functionMerge(prio, new):
    """
    Merges two dicts of class FunctionWrapper
    :param prio: dict(str, FunctionWrapper), if a signature is the same as in new, these elements get priority. Functions not contained in new will be dropped
    :param new: dict(str, FunctionWrapper), if a signature differs from the one in prio, these elements will be in the returned dict
    :return: dict(str, FunctionWrapper)
    """
    ret = {}
    for key in new:
        if key in prio and new[key].signature == prio[key].signature:
            ret[key] = prio[key]
            log.debug("Using old %s" % key)
        else:
            ret[key] = new[key]
            log.debug("New skeleton for %s (%s)" % (key, "not contained" if not key in prio else "different signatures"))

    return ret


def main(sdkdir, tpldir, outdir, ignore, excludes):
    oldfuncs = {}
    oldcallbacks = {}
    sdkinclude = os.path.join(sdkdir, "include")

    if not ignore:
        fpath = os.path.join(outdir, "ts3module.cpp")
        if os.path.exists(fpath):
            log.info("Loading old functions")
            oldfuncs = parsing.getSymbols(fpath, [sdkinclude], False)[3]

        fpath = os.path.join(outdir, "plugin.cpp")
        if os.path.exists(fpath):
            log.info("Loading old callbacks")
            oldcallbacks = parsing.getSymbols(fpath, [sdkinclude], False)[2]

        wrappers.closeFiles()

    consts = {}
    enums = {}
    log.info("Loading defines")
    for paths in DEFINES_SOURCES:
        cs, ens, cbs, fs = parsing.getSymbols(os.path.join(sdkdir, *paths), [sdkinclude], True)
        consts.update(cs)
        enums.update(ens)

    log.info("Loading callbacks")
    callbacks = functionMerge(oldcallbacks, parsing.getSymbols(os.path.join(sdkdir, "src", "plugin.c"), [sdkinclude], True)[2])
    callbacks = {key: callbacks[key] for key in callbacks if callbacks[key].name not in PYPLUGIN_CALLBACK_BLACKLIST}

    log.info("Loading functions")
    functions = functionMerge(oldfuncs, parsing.getSymbols(os.path.join(sdkdir, "include", "ts3_functions.h"), [sdkinclude], True)[3])
    functions = {key: functions[key] for key in functions if key not in MODULE_FUNCTION_BLACKLIST}

    env = Environment(trim_blocks=True, lstrip_blocks=True, loader=FileSystemLoader(tpldir), undefined=StrictUndefined)

    for tmp in env.list_templates(extensions=['tpl']):
        if excludes and tmp in excludes:
            log.debug("Ignoring template %s" % tmp)
            continue

        log.debug("Writing template %s" % tmp)
        t = env.get_template(tmp)
        with open(os.path.join(outdir, tmp[:-4]), "w") as f:
            f.write(t.render(constants=consts, enums=enums, callbacks=callbacks, functions=functions))


def checkSdkDir(path):
    """
    Checks the given path for all needed files of the TS3 plugin sdk
    :param path: the path to the directory
    :return: True on success, False otherwise
    """
    for (p, files) in [('include', ['plugin_definitions.h',
                                    'ts3_functions.h',
                                    'teamlog/logtypes.h',
                                    'teamspeak/clientlib_publicdefinitions.h',
                                    'teamspeak/public_definitions.h',
                                    'teamspeak/public_errors.h',
                                    'teamspeak/public_errors_rare.h',
                                    'teamspeak/public_rare_definitions.h']),
                       ('src', ['plugin.c'])]:
        for f in files:
            if not os.path.exists(os.path.join(path, p, f)):
                logging.debug("%s does not exist" % f)
                return False

    return True


def checkPaths(sdkdir, tpldir, outdir):
    """

    :param sdkdir:
    :param tpldir:
    :param outdir:
    :return:
    """
    if not os.path.exists(sdkdir):
        log.error("Sdk directory does not exist")
        return False
    else:
        log.debug("Sdk directory is %s" % sdkdir)

    if not os.path.exists(tpldir):
        log.error("Template directory does not exist")
        return False
    else:
        log.debug("Template directory is %s" % tpldir)

    if not os.path.exists(outdir):
        log.error("Output directory does not exist")
        return False
    else:
        log.debug("Output directory is %s" % outdir)

    if not checkSdkDir(sdkdir):
        log.error("Sdk directory is not valid")
        return False

    return True


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument('sdkdir', help='The directory, the ts3 pluginsdk is living in')
    parser.add_argument('tpldir', help='The directory, where the jinja2 templates live in')
    parser.add_argument('outputdir', help='The directory, processed files should be placed in')
    parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='Verbose output')
    parser.add_argument('-i', '--ignore', dest='ignore', action='store_true', help='Ignore sources in output directory and overwrite files')
    parser.add_argument('-e', '--exclude', dest='excludes', action='store', nargs='*', help='Ignore template(s), pass the basename(s)')

    args = parser.parse_args()

    log = logging.getLogger("pyTSon-generator")
    ch = logging.StreamHandler()
    log.addHandler(ch)

    if args.verbose:
        log.setLevel(logging.DEBUG)
    else:
        log.setLevel(logging.INFO)


    if not checkPaths(args.sdkdir, args.tpldir, args.outputdir):
        sys.exit(1)

    main(args.sdkdir, args.tpldir, args.outputdir, args.ignore, args.excludes)
