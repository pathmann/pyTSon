from collections import namedtuple

TS3Vector = namedtuple('TS3Vecor', 'x y')

PLUGIN_MENU_BUFSZ = 128
PLUGIN_HOTKEY_BUFSZ = 128

{% for key, value in constants|dictsort %}
{{key}} = {{value}}

{% endfor %}

{% for name, e in enums|dictsort %}
class {{name}}(object):
    {% for (n, v) in e %}
    {{n}} = {{v}}
    {% endfor %}

{% endfor %}
