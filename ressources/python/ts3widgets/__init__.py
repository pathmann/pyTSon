import ts3lib
import ts3defines


def _errprint(msg, errcode, aid, secid=None):
    if secid:
        err = ts3lib.logMessage("%s (%s): %s" % (msg, secid, errcode),
                                ts3defines.LogLevel.LogLevel_ERROR,
                                "pyTSon.ts3widgets", aid)
    else:
        err = ts3lib.logMessage("%s: %s" % (msg, errcode),
                                ts3defines.LogLevel.LogLevel_ERROR,
                                "pyTSon.ts3widgets", aid)

    if err != ts3defines.ERROR_ok:
        if secid:
            print("%s (%s, %s): %s" % (msg, aid, secid, errcode))
        else:
            print("%s (%s): %s" % (msg, aid, errcode))
