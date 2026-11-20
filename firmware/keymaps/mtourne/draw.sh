#!/bin/sh
# Draw this keymap to keymap.svg with keymap-drawer.
# Run from anywhere: keyboards/aki27/cocot46plus/keymaps/mtourne/draw.sh
set -eu
HERE=$(cd "$(dirname "$0")" && pwd)
QMK_HOME=$(cd "$HERE/../../../../.." && pwd)
TMP=$(mktemp -d)

cd "$QMK_HOME"
# Real key positions and rotations, from aki27's VIA file for this board
# (github.com/aki27kbd/cocot46plus, firmware/cocot46plus_via_rev.json).
python3 -I "$HERE/kle_layout.py" "$HERE/cocot46plus_via_rev.json" \
    "$QMK_HOME/keyboards/aki27/cocot46plus/keyboard.json" "$TMP/layout.json"
qmk c2json --no-cpp -kb aki27/cocot46plus -km mtourne "$HERE/keymap.c" -o "$TMP/keymap.json" >/dev/null

# c2json keeps our #define aliases (HQ_A, TH_SPC, ...); expand them so
# keymap-drawer sees MT(...) / LT(...) and can show hold actions.
python3 -I - "$HERE/keymap.c" "$TMP/keymap.json" <<'EOF'
import json, re, sys
src, path = sys.argv[1], sys.argv[2]
defs = dict(re.findall(r'^#define\s+(\w+)\s+(\w+\(.*?\))', open(src).read(), re.M))
data = json.load(open(path))
layers = re.findall(r"^\s+(_[A-Z]+),", re.search(r"enum layers \{(.*?)\};", open(src).read(), re.S).group(1), re.M)
data["layers"] = [[defs.get(k, k) for k in layer] for layer in data["layers"]]
for name, i in sorted(((n, i) for i, n in enumerate(layers)), key=lambda x: -len(x[0])):
    data["layers"] = [[k.replace(name, str(i)) for k in layer] for layer in data["layers"]]
json.dump(data, open(path, "w"))
EOF

uvx --quiet --from keymap-drawer keymap -c "$HERE/keymap_drawer.yaml" parse -l "DV (computer set to Dvorak)" "QW (computer set to US)" NAV NUM FUN MOUSE -q "$TMP/keymap.json" > "$HERE/keymap.yaml"
# Home-row mod holds as icons (Material Design Icons, as on the OLED).
sed -i "" -e 's/h: MOD_LCTL/h: $$mdi:ship-wheel$$/; s/h: MOD_LALT/h: $$mdi:apple-keyboard-option$$/' \
    -e 's/h: MOD_LGUI/h: $$mdi:apple-keyboard-command$$/; s/h: MOD_LSFT/h: $$mdi:apple-keyboard-shift$$/' "$HERE/keymap.yaml"
# Colour thumb keys and build the all-layers cheat sheet (like Remap's).
uv run --quiet --with pyyaml python3 -I "$HERE/cheatsheet.py" "$HERE/keymap.yaml" "$TMP/cheatsheet.yaml" \
    "$TMP/layout.json" "$TMP/cheatsheet_layout.json" "$TMP/live.yaml"
uvx --quiet --from keymap-drawer keymap -c "$HERE/keymap_drawer.yaml" draw -g 46 48 49 50 51 \
    -j "$TMP/layout.json" "$HERE/keymap.yaml" > "$HERE/keymap.svg"

uvx --quiet --from keymap-drawer keymap -c "$HERE/cheatsheet_drawer.yaml" draw \
    -j "$TMP/cheatsheet_layout.json" "$TMP/cheatsheet.yaml" > "$HERE/cheatsheet.svg"
# Add the colour key under the drawing.
python3 -I "$HERE/legend.py" "$HERE/cheatsheet.svg"
python3 -I "$HERE/stack.py" "$HERE/cheatsheet.svg"
# The live visualiser (Chrome page, also used by Cocot Live): one board per
# view, switched by layer and Shift, plus a key map.
uvx --quiet --from keymap-drawer keymap -c "$HERE/live_drawer.yaml" draw \
    -j "$TMP/cheatsheet_layout.json" "$TMP/live.yaml" > "$TMP/live.svg"
python3 -I "$HERE/stack.py" "$TMP/live.svg"
python3 -I "$HERE/visualizer.py" "$TMP/live.svg" "$TMP/cheatsheet_layout.json" "$HERE/visualizer.html"
# PDF copies, via headless Chrome if it is installed. The cheat sheet goes on
# an A4 portrait page (scaled to the page width), so viewers never rotate it;
# keymap.pdf keeps the drawing's own (tall) size.
CHROME="/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
if [ -x "$CHROME" ]; then
    for name in cheatsheet keymap; do
        if [ "$name" = cheatsheet ]; then
            page="A4 portrait; margin: 10mm"
        else
            page="$(sed -n 's/.*<svg[^>]* width="\([0-9]*\)" height="\([0-9]*\)".*/\1px \2px/p' "$HERE/$name.svg" | head -1); margin: 0"
        fi
        printf '<style>@page { size: %s } body { margin: 0 } img { width: 100%% }</style><img src="file://%s">' \
            "$page" "$HERE/$name.svg" > "$TMP/$name.html"
        "$CHROME" --headless=new --disable-gpu --no-pdf-header-footer \
            --print-to-pdf="$HERE/$name.pdf" "file://$TMP/$name.html" >/dev/null 2>&1
    done
fi
rm -rf "$TMP"
echo "Wrote keymap.svg, cheatsheet.svg (and .pdf copies) in $HERE"
