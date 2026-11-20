"""Convert the cocot46plus VIA definition (keyboard-layout-editor geometry,
with rotated thumb keys) into a QMK-style layout file that keymap-drawer can
draw, keeping the key order of keyboard.json's LAYOUT.

Positions that the VIA file lacks (the knob-turn boxes, matrix 4,2 and 4,4)
are placed to the left and right of the knob (matrix 3,4), and the two
mouse-button keys are eased apart slightly.

Usage: kle_layout.py via.json keyboard.json out.json
"""

import json
import sys


def kle_keys(rows):
    """Yield (label, x, y, w, h, r, rx, ry) per key, following KLE's rules."""
    x = y = r = rx = ry = 0.0
    w = h = 1.0
    for row in rows:
        for item in row:
            if isinstance(item, dict):
                if "r" in item:
                    r = item["r"]
                if "rx" in item:
                    rx = item["rx"]
                    x, y = rx, ry
                if "ry" in item:
                    ry = item["ry"]
                    x, y = rx, ry
                x += item.get("x", 0)
                y += item.get("y", 0)
                w, h = item.get("w", w), item.get("h", h)
                continue
            yield item.split("\n")[0], x, y, w, h, r, rx, ry
            x += w
            w = h = 1.0
        y += 1
        x = rx


def main(via_path, info_path, out_path):
    via = json.load(open(via_path))
    info = json.load(open(info_path))
    geometry = {}
    for label, x, y, w, h, r, rx, ry in kle_keys(via["layouts"]["keymap"]):
        geometry[label] = {"x": x, "y": y, "w": w, "h": h, "r": r, "rx": rx, "ry": ry}
    # The two mouse-button keys overlap slightly in the VIA geometry; ease
    # them apart.
    for label, dx in (("3,5", -0.3), ("8,5", 0.3)):
        geometry[label]["x"] += dx
        geometry[label]["rx"] += dx
    knob = geometry["3,4"]
    for label, dx in (("4,2", -1), ("4,4", 1)):
        geometry.setdefault(label, dict(knob, x=knob["x"] + dx))

    layout = info["layouts"]["LAYOUT"]["layout"]
    for key in layout:
        label = "%d,%d" % tuple(key["matrix"])
        key.update(geometry.get(label, {}))
    json.dump(info, open(out_path, "w"))


if __name__ == "__main__":
    main(*sys.argv[1:4])
