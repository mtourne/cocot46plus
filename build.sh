#!/bin/sh
# Build, flash or redraw the cocot46plus firmware.
#
#   ./build.sh          compile cocot46plus_mtourne.hex
#   ./build.sh flash    compile, wait for the bootloader, then flash
#   ./build.sh draw     redraw the cheat sheet, keymap and live view, and copy
#                       them into docs/ for the web page
#   ./build.sh app      build Cocot Live.app (macOS; run draw first)
#
# QMK lives in $QMK_HOME if you set it, otherwise in .qmk/ (cloned on first
# run). The board folder is linked into it as keyboards/aki27/cocot46plus,
# so QMK always builds the files in this repo.
set -eu
REPO=$(cd "$(dirname "$0")" && pwd)
QMK_TAG=0.31.12
QMK_HOME=${QMK_HOME:-$REPO/.qmk}
export QMK_HOME
BOARD=$QMK_HOME/keyboards/aki27/cocot46plus
KEYMAP=$BOARD/keymaps/mtourne
HEX=aki27_cocot46plus_mtourne.hex

if [ ! -d "$QMK_HOME/quantum" ]; then
    git clone --depth 1 --branch "$QMK_TAG" --recurse-submodules --shallow-submodules \
        https://github.com/qmk/qmk_firmware.git "$QMK_HOME"
fi
if [ -e "$BOARD" ] && [ ! -L "$BOARD" ]; then
    echo "$BOARD already exists and is not a link to this repo. Move it away, or set QMK_HOME to another QMK checkout." >&2
    exit 1
fi
mkdir -p "$(dirname "$BOARD")"
ln -sfn "$REPO/firmware" "$BOARD"

compile() {
    (cd "$QMK_HOME" && qmk compile -kb aki27/cocot46plus -km mtourne)
    cp "$QMK_HOME/$HEX" "$REPO/cocot46plus_mtourne.hex"
}

case "${1:-build}" in
build)
    compile
    ;;
flash)
    compile
    echo "Start the bootloader: hold a green thumb key and the cyan key next to it, then press Boot (top left), or press reset twice."
    while :; do
        for port in /dev/cu.usbmodem* /dev/ttyACM*; do
            if [ -e "$port" ]; then
                sleep 1
                avrdude -p atmega32u4 -c avr109 -P "$port" -U "flash:w:$REPO/cocot46plus_mtourne.hex:i"
                exit 0
            fi
        done
        sleep 0.5
    done
    ;;
draw)
    "$KEYMAP/draw.sh"
    for file in cheatsheet.svg cheatsheet.pdf keymap.svg keymap.pdf; do
        cp "$REPO/firmware/keymaps/mtourne/$file" "$REPO/docs/"
    done
    cp "$REPO/firmware/keymaps/mtourne/visualizer.html" "$REPO/docs/live.html"
    ;;
app)
    "$REPO/firmware/keymaps/mtourne/viz-app/build.sh"
    ;;
*)
    sed -n '2,12p' "$0" >&2
    exit 1
    ;;
esac
