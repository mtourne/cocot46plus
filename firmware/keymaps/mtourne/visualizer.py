"""Build visualizer.html: the keyboard, live. Open it in Chrome, click
"Connect keyboard" once, and each key lights up while it is held. It shows one
board at a time (live.svg from cheatsheet.py). All boards keep every label
in the same place; the one for the active layer just bolds that layer's
labels and fades the rest.

The keyboard reports key events over raw HID (see send_to_visualizer in
keymap.c): 'K', row, col, pressed, layer, mods. This page maps (row, col) to
the cheat sheet's key positions using the drawn layout. It only listens to
that channel, apart from a small "watching" ping every 2 s; the keyboard
only reports while it receives those pings.

Usage: visualizer.py live.svg cheatsheet_layout.json visualizer.html
"""

import json
import sys

PAGE = """<!doctype html>
<meta charset="utf-8">
<title>cocot46plus, live</title>
<style>
  body { font-family: -apple-system, system-ui, sans-serif; margin: 16px; background: #fafafa; }
  #bar { display: flex; gap: 18px; align-items: center; margin-bottom: 10px; font-size: 15px; }
  #layer { font-weight: 600; }
  #status { color: #777; }
  .down rect { fill: #ffd54f !important; stroke: #e65100 !important; stroke-width: 3px !important; }
  /* Held modifiers, in the gap between the halves: four fixed slots in CAGS
     order (as on the OLED), dim until held. */
  #hud { position: absolute; display: flex; gap: 0.3em; transform: translate(-50%, -50%);
         pointer-events: none; color: #7a4fb5; font-family: -apple-system, system-ui, sans-serif; }
  #hud span { opacity: 0.13; transition: opacity 60ms; }
  #hud span.on { opacity: 1; }
</style>
<div id="hud"></div>
<div id="bar">
  <button id="connect">Connect keyboard</button>
  <span id="status">not connected</span>
  <span id="layer"></span>
</div>
__SVG__
<script>
const POS = __POS__;  // "row,col" -> key position on the drawing
const LAYERS = ["BASE", "BASE (QW)", "NAV", "NUM", "FUN", "MOUSE"];
const MODS = [[0x11, "\\u2388"], [0x44, "\\u2325"], [0x88, "\\u2318"], [0x22, "\\u21E7"]];  // left|right bits
const FILTER = { vendorId: 0x1727, productId: 0x0003, usagePage: 0xFF60, usage: 0x61 };
const $ = (id) => document.getElementById(id);

// One board per view; show the one for the active layer (and Shift on NUM),
// cropped to it.
const VIEWS = ["BASE", "BASE", "NAV", "NUM", "FUN", "MOUSE"];
let current = null;
const HUD = MODS.map(([, symbol]) => {
  const span = document.createElement("span");
  span.textContent = symbol;
  $("hud").appendChild(span);
  return span;
});
// Centre the modifier slots between the inner home-row keys (I and D).
function placeHud() {
  const group = document.querySelector("g.layer-" + current);
  const left = group?.querySelector(".keypos-17"), right = group?.querySelector(".keypos-18");
  if (!left || !right) return;
  const a = left.getBoundingClientRect(), b = right.getBoundingClientRect();
  const hud = $("hud");
  hud.style.left = (a.right + b.left) / 2 + scrollX + "px";
  hud.style.top = (a.top + a.bottom) / 2 + scrollY + "px";
  hud.style.fontSize = a.height * 0.36 + "px";
}
addEventListener("resize", placeHud);
function setView(name) {
  if (name === current) return;
  current = name;
  const svg = document.querySelector("svg");
  svg.querySelectorAll('g[class^="layer-"]').forEach((g) => {
    g.style.display = g.classList.contains("layer-" + name) ? "" : "none";
  });
  const box = svg.getBBox();
  svg.setAttribute("viewBox", `${box.x - 6} ${box.y - 6} ${box.width + 12} ${box.height + 12}`);
  placeHud();
}

function show(d) {
  if (d[0] !== 0x4B) return;  // 'K'
  $("layer").textContent = "Layer: " + (LAYERS[d[4]] ?? d[4]);
  HUD.forEach((span, i) => span.classList.toggle("on", (d[5] & MODS[i][0]) !== 0));
  setView(VIEWS[d[4]] ?? "BASE");
  // Shift held: the NUM board swaps emphasis to the Shift partners.
  document.querySelector("svg").classList.toggle("shift", (d[5] & 0x22) !== 0);
  if (d[1] === 0xFF) return;  // layer change only
  const pos = POS[d[1] + "," + d[2]];
  if (pos === undefined) return;
  document.querySelectorAll(".keypos-" + pos).forEach((g) => g.classList.toggle("down", d[3] === 1));
}

async function listen(device) {
  if (!device.collections.some((c) => c.usagePage === FILTER.usagePage)) return;
  if (!device.opened) await device.open();
  device.addEventListener("inputreport", (e) => show(new Uint8Array(e.data.buffer)));
  // Tell the keyboard the page is watching; it only sends while pinged.
  const ping = () => device.opened && device.sendReport(0, new Uint8Array(32).fill(0x56)).catch(() => {});
  ping();
  setInterval(ping, 2000);
  $("status").textContent = "connected";
}

{
  const svg = document.querySelector("svg");
  svg.removeAttribute("width"); svg.removeAttribute("height");
  svg.style.cssText = "width: 100%; height: auto; display: block;";
  setView(location.hash.slice(1) || "BASE");  // e.g. visualizer.html#NUM to preview a view
  addEventListener("hashchange", () => setView(location.hash.slice(1) || "BASE"));
  // Embedded in the project page: just the board, on the page's background.
  if (window.top !== window.self) {
    $("bar").style.display = "none";
    document.body.style.cssText = "margin: 0; background: transparent;";
  }
}

if (!("hid" in navigator)) {
  $("status").textContent = "WebHID is not available here: open this page in Chrome";
} else {
  $("connect").onclick = async () => {
    const [device] = await navigator.hid.requestDevice({ filters: [FILTER] });
    if (device) listen(device);
  };
  navigator.hid.getDevices().then((devices) => devices.forEach(listen));
  navigator.hid.addEventListener("connect", (e) => listen(e.device));
  navigator.hid.addEventListener("disconnect", () => { $("status").textContent = "disconnected"; });
}
</script>
"""


def main(svg_path, layout_path, out_path):
    svg = open(svg_path).read()
    layout = json.load(open(layout_path))["layouts"]["LAYOUT"]["layout"]
    positions = {"%d,%d" % tuple(key["matrix"]): i for i, key in enumerate(layout)}
    page = PAGE.replace("__SVG__", svg).replace("__POS__", json.dumps(positions))
    open(out_path, "w").write(page)


if __name__ == "__main__":
    main(*sys.argv[1:4])
