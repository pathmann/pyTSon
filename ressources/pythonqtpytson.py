"""
Module PythonQt.pytson
"""

class EventFilterObject(QObject):
    """
    Class to install as eventfilter on QObject-based objects. Connect to the signal eventFiltered(QObject*, QEvent*) to receive the filtered event per installEventFilter.
    """

    def __init__(self, typelist=list(), parent=None):
        """
        Instantiates a new object.
        @param typelist: list of Eventtypes to filter (see QEvent::Type). Defaults to an empty list.
        @type typelist: list(int)
        @param parent: QObject-parent
        @type parent: QObject
        """
        pass

    def setFilterResult(self, val):
        """
        Sets the return value the object should return in the eventFilter-method. If not set, False will be returned
        @param val: the value
        @type val: bool
        """
        pass

    def types(self):
        """
        Returns the current list of eventtypes.
        @return: the current eventtypes
        @rtype: list(int)
        """
        pass

    def addType(self, eventtype):
        """
        Adds an eventtype to the list.
        @type eventtype: an eventtype (see QEvent::Type)
        @rtype eventtype: int
        """
        pass

    def removeType(self, eventtype):
        """
        Removes an eventtype from the list.
        @param eventtype: an eventtype (see QEvent::Type)
        @type eventtype: int
        """
        pass

