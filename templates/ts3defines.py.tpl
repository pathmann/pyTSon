from collections import namedtuple

TS3Vector = namedtuple('TS3Vecor', 'x y')

PLUGIN_MENU_BUFSZ = 128
PLUGIN_HOTKEY_BUFSZ = 128

{% for key, value in constants.items() %}
{{key}} = {{value}}

{% endfor %}

{% for name, e in enums.items() %}
class {{name}}(object):
    {% for (n, v) in e %}
    {{n}} = {{v}}
    {% endfor %}
    
{% endfor %}
