from pydoc import help as pydochelp


def help(obj):
    """
    This is a wrapper around pydoc.help to display docstrings for the ts3 module.
    Note: Adding the docstrings directly in the cpython module blows up the data segment of the pyTSon library.
    """
    if hasattr(obj, "__module__") and (obj.__module__ == "ts3lib" or obj.__module__ == "_ts3lib"):
        name = getattr(obj, "__name__", None)

        txt = ""
        if name:
            {% for name, f in functions|dictsort %}
                {% if loop.index == 1 %}
            if name == "{{name}}":
                {% else %}
            elif name == "{{name}}":
                {% endif %}
                {% if f.docString != "" %}
                txt = "{{f.docString}}"
                {% endif %}
            {% endfor %}

        if txt != "":
            for l in txt.split("\n"):
                print(l)

            return

    return pydochelp(obj)
