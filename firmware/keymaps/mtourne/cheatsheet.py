"""Build cheatsheet.yaml: one board showing what you actually type (Dvorak, the
same in DV and QW mode), with every layer's legend on its key, in the
style of the Remap cheat sheet. Run by draw.sh; reads keymap.yaml from
keymap-drawer and also colours its thumb keys in place, so both drawings match
the keycaps.

Legend positions on each key:
  center        base layer (tap)                       black
  bottom-left   base layer (hold)                      grey
  top-right     NUM (hold green)                       red
  top-left      FUN (green + cyan)                     green
  bottom-right  NAV (hold yellow)                      blue
  middle-left   MOUSE (on while the ball moves)        cyan
  bottom-centre what the key does with ⌘ / ⇧ held      purple

The two dotted boxes beside the knob show what turning it does; the board's
three unused bottom-right positions are left out.

It also writes live.yaml for the live view (visualizer.html, Cocot Live): one
board per view (BASE, NAV, NUM, FUN, MOUSE) with labels in the same slots, so
the display stays still and only the emphasis changes.

Usage: cheatsheet.py keymap.yaml cheatsheet.yaml layout.json trimmed.json live.yaml
"""

import json
import sys

import yaml

# LAYOUT positions (see keymap.c).
LEFT_GREEN, LEFT_CYAN, RIGHT_CYAN, RIGHT_GREEN = 39, 40, 41, 42
KNOB_LEFT, KNOB_CLICK, KNOB_RIGHT = 46, 47, 48
UNUSED = (49, 50, 51)  # dropped from the cheat sheet

# Thumb keycap colours, from the trackball outwards: 1 green, 2 yellow,
# 3 pink, 4 purple (lilac); mouse buttons cyan. Keys are drawn with a CSS
# class per colour.
THUMB_COLOURS = {
    36: "purple", 37: "pink", 38: "yellow", 39: "green", 40: "cyan",
    41: "cyan", 42: "green", 43: "yellow", 44: "pink", 45: "purple",
}

# Behaviour the keymap alone does not show, as bottom-centre legends.
WITH_MODS = {
    24: "both: CAPS",  # both outer Shifts: Caps Word
    35: "both: CAPS",
    45: "⌘` ⇧~",  # Grave Escape
}

# What turning the knob does, per layer (base / NAV / FUN).
KNOB = {
    KNOB_LEFT: {"t": "↺ ↑", "br": "PgUp", "tl": "Vol−"},
    KNOB_RIGHT: {"t": "↻ ↓", "br": "PgDn", "tl": "Vol+"},
}
# What Shift turns each NUM character into (US layout, as on a number row).
SHIFTED = dict(zip("`1234567890-=[]\\;',./", "~!@#$%^&*()_+{}|:\"<>?"))


def paint(keys):
    """Add the thumb colour class to each thumb key (keeping 'held' etc.)."""
    for pos, colour in THUMB_COLOURS.items():
        key = keys[pos] if isinstance(keys[pos], dict) else {"t": keys[pos]}
        key["type"] = f"{key.get('type', '')} th-{colour}".strip()
        keys[pos] = key
    return keys


def stack_green_thumb(pos, text):
    """On a green thumb key the NAV legend shares the bottom edge with the
    grey NUM hold label; put it on two lines so they don't collide."""
    return text.replace(" ", "\n") if THUMB_COLOURS.get(pos) == "green" else text


def legend(key, field="t"):
    """Return a key's legend, or '' for empty and transparent keys. Keys that
    turn a layer on keep their legend on that layer (e.g. left yellow is Bksp
    and holds NAV, but on NAV, held from the right, it is Delete)."""
    if isinstance(key, str):
        return "" if field != "t" or key == "▽" else key
    text = str(key.get(field, ""))
    return "" if "trans" in key.get("type", "") or text == "▽" else text


def combine(base, nav, num, fun, mouse):
    keys = []
    for pos, (b, nv, nm, fn, ms) in enumerate(zip(base, nav, num, fun, mouse)):
        key = {
            "t": legend(b),
            "bl": legend(b, "h"),
            "tr": "" if legend(nm) == "FUN" else legend(nm),  # the FUN marks show that
            "tl": legend(fn),
            # NAV's plain mods repeat the home-row hold on the same key: the live
            # view shows them, the printed sheet doesn't.
            "br": "" if legend(nv) == legend(b, "h") else stack_green_thumb(pos, legend(nv)),
            "right": legend(nm, "s"),  # NUM's own Shift partners (* /, + -, % ^, . ,)
            "left": legend(ms) if legend(ms) != legend(nv) else "",  # NAV shows it already
            "h": WITH_MODS.get(pos, ""),
        }
        key.update(KNOB.get(pos, {}))
        keys.append({k: v for k, v in key.items() if v})
    keys[KNOB_CLICK]["bl"] = "knob"
    return [k for pos, k in enumerate(keys) if pos not in UNUSED]


def live_layers(base, nav, num, fun, mouse):
    """One board per view for the live display. Every board uses the same
    slots, so nothing moves when the view changes; live_drawer.yaml fades the
    labels that do not apply and bolds the active layer's. NUM adds the
    shifted character in NAV's slot (NAV is not in use then); FUN and MOUSE
    add their labels only on their own board."""
    boards = {}
    for view in ("BASE", "NAV", "NUM", "FUN", "MOUSE"):
        keys = []
        for pos, (b, nv, nm, fn, ms) in enumerate(zip(base, nav, num, fun, mouse)):
            n = "" if legend(nm) == "FUN" else legend(nm)
            key = {"t": legend(b), "bl": legend(b, "h"), "tr": n, "br": stack_green_thumb(pos, legend(nv)), "h": WITH_MODS.get(pos, "")}
            if legend(nv) and legend(nv) == legend(b, "h"):
                # NAV's plain mods are the same as the key's hold: no blue
                # copy, and the hold icon stays at full strength on NAV.
                key["br"] = ""
                key["type"] = "navmod"
            if view == "NUM":
                key["br"] = legend(nm, "s") or SHIFTED.get(n, "")
            elif view == "FUN":
                key["tl"] = legend(fn)
            elif view == "MOUSE":
                key["left"] = legend(ms)
            key.update(KNOB.get(pos, {}))
            if view != "FUN":
                key.pop("tl", None)
            if view == "NUM" and pos in KNOB:
                key.pop("br", None)  # the knob's PgUp/PgDn belong to NAV
            keys.append({k: v for k, v in key.items() if v})
        boards[view] = paint([k for pos, k in enumerate(keys) if pos not in UNUSED])
    return boards


def trim_layout(src, dst):
    """Copy keyboard.json without the unused positions."""
    info = json.load(open(src))
    layout = info["layouts"]["LAYOUT"]["layout"]
    info["layouts"]["LAYOUT"]["layout"] = [k for pos, k in enumerate(layout) if pos not in UNUSED]
    json.dump(info, open(dst, "w"))


def main(src, dst, info_src, info_dst, live_dst):
    keymap = yaml.safe_load(open(src))
    _qwerty, dvorak, nav, num, fun, mouse = keymap["layers"].values()
    name = "Dvorak: what you type, in DV and QW mode"
    out = {
        "layout": keymap["layout"],
        "layers": {name: combine(dvorak, nav, num, fun, mouse)},
        "combos": [
            # FUN: a green thumb mashed with the cyan mouse key next to it.
            {"p": [LEFT_GREEN, LEFT_CYAN], "k": "FUN", "l": [name]},
            {"p": [RIGHT_CYAN, RIGHT_GREEN], "k": "FUN", "l": [name]},
        ],
    }
    paint(out["layers"][name])
    yaml.safe_dump(out, open(dst, "w"), allow_unicode=True, sort_keys=False)
    trim_layout(info_src, info_dst)
    live = {"layout": keymap["layout"], "layers": live_layers(dvorak, nav, num, fun, mouse)}
    yaml.safe_dump(live, open(live_dst, "w"), allow_unicode=True, sort_keys=False)

    for keys in keymap["layers"].values():
        paint(keys)
    yaml.safe_dump(keymap, open(src, "w"), allow_unicode=True, sort_keys=False)


if __name__ == "__main__":
    main(*sys.argv[1:6])
