"""Append a one-line colour key to cheatsheet.svg, explaining what each legend
colour on the keys means. Run by draw.sh after the cheat sheet is drawn.
"""

import html
import re
import sys

# (colour, text) segments; keep in sync with cheatsheet.py and
# cheatsheet_drawer.yaml.
KEY = [
    ("#222", "black: type"),
    ("#888", "grey: hold"),
    ("#d0312d", "red: NUM (hold green)"),
    ("#2b4bd6", "blue: NAV (hold yellow; the ball scrolls)"),
    ("#2e8b2e", "green: FUN (green + cyan)"),
    ("#0a9396", "cyan: MOUSE (ball)"),
    ("#7a4fb5", "purple: with ⌘/⇧"),
]


def main(path):
    svg = open(path).read()
    m = re.search(r'<svg width="(\d+)" height="(\d+)" viewBox="0 0 (\d+) (\d+)"', svg)
    width, height = int(m.group(1)), int(m.group(2))
    spans = " · ".join(f'<tspan fill="{colour}">{html.escape(text)}</tspan>' for colour, text in KEY)
    line = f'<text x="30" y="{height + 4}" class="colour-key" style="text-anchor: start" font-size="11">{spans}</text>'
    new_h = height + 24
    svg = svg.replace(m.group(0), f'<svg width="{width}" height="{new_h}" viewBox="0 0 {width} {new_h}"', 1)
    svg = svg[: svg.rindex("</svg>")] + line + "\n</svg>\n"
    open(path, "w").write(svg)


if __name__ == "__main__":
    main(sys.argv[1])
