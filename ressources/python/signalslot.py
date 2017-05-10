from weakref import WeakSet, WeakKeyDictionary
import inspect


class Signal(object):
    """
    Simple class to emit signals to connected callable receivers.
    """

    def __init__(self):
        """
        Instantiate a new object
        """
        self.funcs = WeakSet()
        self.meths = WeakKeyDictionary()

    def connect(self, c):
        """
        Connect a callable as receiver for the signal
        @param c: signal receiver
        @type c: Callable
        """
        if inspect.ismethod(c):
            if c.__self__ not in self.meths:
                self.meths[c.__self__] = set()

            self.meths[c.__self__].add(c.__func__)
        else:
            if c not in self.funcs:
                self.funcs.add(c)

    def disconnect(self, c):
        """
        Disconnect the callable from receiving the signal
        @param c: signal receiver
        @type c: Callable
        """
        if inspect.ismethod(c):
            if c.__self__ in self.meths:
                self.meths[c.__self__].remove(c.__func__)
        else:
            if c in self.funcs:
                self.funcs.remove(c)

    def disconnectAll(self):
        """
        Disconnects all signal receivers
        """
        self.funcs.clear()
        self.meths.clear()

    def emit(self, *args, **kwargs):
        """
        Fires the signal to all connected receivers
        """
        for c in self.funcs:
            c(*args, **kwargs)

        for obj, funcs in self.meths.items():
            for func in funcs:
                func(obj, *args, **kwargs)
