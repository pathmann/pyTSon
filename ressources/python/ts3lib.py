import _ts3lib


for func in dir(_ts3lib):
    if not func.startswith("__"):
        if func not in globals():
            globals()[func] = getattr(_ts3lib, func)
