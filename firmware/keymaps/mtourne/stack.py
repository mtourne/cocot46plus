"""Draw two-line corner legends on two lines. keymap-drawer keeps the line
break in a corner legend (cheatsheet.py adds one to "Del line" on the green
thumb) but draws it on one line; this splits it into two tspans, the first
one moved up so the pair still sits on the key's bottom edge.

Usage: stack.py drawing.svg ...
"""

import re
import sys

LEGEND = re.compile(r'(<text x="(-?[\d.]+)" y="[^"]*" class="key[^"]*\b(?:br|bl)\b[^"]*">)<tspan[^>]*>([^<\n]+)\n([^<]+)</tspan>(</text>)')


def stack(match):
    start, x, first, second, end = match.groups()
    return (f'{start}<tspan x="{x}" dy="-1em" style="font-size: 11px">{first}</tspan>'
            f'<tspan x="{x}" dy="1em" style="font-size: 11px">{second}</tspan>{end}')


for path in sys.argv[1:]:
    svg = open(path).read()
    open(path, "w").write(LEGEND.sub(stack, svg))
