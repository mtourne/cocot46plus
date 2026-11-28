# cocot46plus keymap (mtourne)

A Dvorak layout for the cocot46plus with dual-use keys, modelled on
[dlip's engram layout](https://github.com/dlip/qmk_firmware/tree/dlip/keyboards/tamatama/keymaps/engram):
letters and thumb keys act as modifiers or layer switches when held.

[cheatsheet.svg](cheatsheet.svg) shows every layer on one board.
[keymap.svg](keymap.svg) shows each layer separately.

## Always Dvorak, on any computer

You always type Dvorak. There are two base layers so the board works the same
whatever the computer's keyboard setting is:

| Mode (base layer) | Use when the computer is set to | The board sends |
|---|---|---|
| **DV** (default; QWERTY base) | Dvorak | normal US keys; the OS turns them into Dvorak |
| **QW** (DVORAK base) | US (QWERTY) | Dvorak letters directly |

Shortcuts always follow Dvorak positions: ⌘C is on the Dvorak C key. On a Mac,
use the plain **Dvorak** input source, not "Dvorak – QWERTY ⌘".

## Thumb keys

Keycaps are colour-coded from the trackball outwards.

| | Purple (4) | Pink (3) | Yellow (2) | Green (1) |
|---|---|---|---|---|
| Left | Alt | Cmd | Bksp / hold NAV | Delete word / hold NUM |
| Right | Esc / hold Alt | Tab / hold Cmd | Space / hold NAV | Enter / hold NUM |

For FUN, mash a green key together with the cyan mouse key next to it (on
either side). The green keys only switch to
NUM, and the left yellow Bksp to NAV, only after a full hold (190 ms), so a
quick tap rolled into the next letter never switches layer. The right pink and
purple keys work the same way: Tab and Esc when tapped, Cmd and Alt after a
full hold, mirroring the left thumbs so a shortcut on a left-hand letter (⌘Q,
⌘X, ⌥←) can use the other hand's thumb. Tap and then hold Bksp, Tab or Esc to
repeat it.

The left thumb deletes (Bksp, delete word) and the right one types (Enter,
Space). On NAV, the left yellow key is a plain Bksp, so it repeats while
held, and the green key deletes the line. ⌘Tab, Alt+Tab and ⌘Enter are two-thumb chords: left pink (Cmd) or
purple (Alt) plus the right key.

Tapped, the right purple key works like QMK's Grave Escape: Esc on its own, `` ` `` with Cmd held (⌘\`
cycles windows) and `~` with Shift held. With Ctrl held it stays Esc, so
Ctrl+Shift+Esc still opens Task Manager on Windows. ⌘⌥Esc (Force Quit) is
lost; use the Apple menu instead.

## Layers

- **Base:** home-row mods in CAGS order (pinky to index: Ctrl, Alt, Cmd,
  Shift) on both hands. Plain Tab, Ctrl and Shift on the left outer column;
  `/`, `-` and Shift on the right one. Press both outer Shifts together for
  Caps Word: capitals until the end of the word (`MAX_SIZE`), shown as CAPS
  on the OLED.

  How a home-row key decides between letter and modifier:
  - Pressed within 200 ms of any other key (except a modifier), it is always
    the letter, so fast typing never triggers a modifier (QMK's Flow Tap).
    For Shift the window is 80 ms, so a quick capital still works.
  - Overlapping a key on the same hand, it is the letter (Chordal Hold).
  - Otherwise it becomes the modifier when another key is pressed and
    released while it is held, or after 250 ms on its own. For Ctrl, Alt and
    Cmd that first rule needs the key to have been down at least 150 ms, so
    a fast roll ("to": T held, O tapped right after) stays two letters while
    a deliberate shortcut fires as usual. Shift skips this, for quick
    capitals.
- **NAV:** on the right, arrows with line start/end above Left and Right,
  word jumps below them (⌥←/⌥→, Ctrl+←/→ on Windows) with Bksp between,
  page up/down and forward Delete on the inner column, and document
  start/end and delete line on the pinky column. The cursor jumps repeat
  while held. On the left thumbs: Bksp (repeats) and delete line. On the
  left: Esc and Delete top left and Enter top right, Cut/Copy/Paste on the
  bottom row with Undo in the inner corner and Redo above it, and plain
  Ctrl, Alt, Cmd and Shift on the home row (same order as the
  home-row mods), so you can hold them with the arrows in any order, for
  example Shift with a word jump to select a word. The cheat sheet leaves them
  out because they match each key's hold; on NAV the live view keeps the
  hold icons at full strength instead. The
  trackball scrolls.
- **NUM:** one rule: Shift gives the partner you would expect. The right hand
  is a numpad shaped like the F-keys on FUN (7 8 9 0 on top, a second `0`
  next to 6 on the home row). Digits are plain digits. Beside them the
  operators come in Shift pairs: `*`/`/`, `+`/`-`, `%`/`^`, and `.`/`,` under
  the numpad. The left hand: the top row is Shift+1..5 of a normal keyboard
  (`` ` `` then `! @ # $ %`); the home row is `/ \ ( ) =` (Shift: `? |` and `+`
  as usual), with `( )` on the two strongest fingers and the home-row mods
  held; `[ ]` sit right under the parens (Shift: `{ }`), next to `^ &` and
  `|`. The PDF shows the operator pairs in light red on the right of each key;
  the live view swaps in every Shift partner on NUM. Ideas from Arsenik and Pascal
  Getreuer's [symbol-layer principles](https://getreuer.info/posts/keyboards/symbol-layer/).
- **FUN:** media keys, F1–F12, mode and Mac/Windows switches, LEDs (on/off,
  Colour, brightness -/+), trackball settings and the Boot key (top left).
  The OLED shows the short name of each FUN key you press, with the new value
  where there is one (CPI 875, ROTATE +30, SCROLL SPEED 7, OS DVORAK, MAC).
- **MOUSE:** turns on by itself while the ball moves, and stays on 1.2 s
  after the ball stops, or 300 ms after a left click (you are usually about
  to type then). Mouse buttons on the
  home row of both hands; hold K or M (index fingers, bottom row) to scroll.
  For a modifier-click, hold Cmd (left pink thumb, or hold right Tab), Alt
  (left purple thumb, or hold right Esc), or the outer Ctrl or Shift, then
  click. Plain modifiers don't turn the MOUSE layer off.

The OLED is kept light. Its lower half shows a large OS icon on the left, the
layer (NAV, NUM, FUNC, MOUSE, or CAPS during Caps Word; empty on the base
layer)
and the mode on the right (DV or QW). Above it, large modifier icons
⎈ ⌥ ⌘ ⇧ appear in fixed slots (CAGS finger order) only while held, which
helps spot an accidental home-row mod. On FUN, the top line names the FUN key
you just pressed instead. The indicator LEDs turn red during Caps Word.
`font_art.py` draws the icons.

To save power, the LEDs switch off after 60 seconds without input and while
the computer sleeps, and come back with the next key, knob turn or ball
movement. The OLED switches off after 60 seconds too.

The knob scrolls, pages on NAV and sets the volume on FUN; clicking it is the
middle mouse button.

## Live visualiser

`visualizer.html` (built by `draw.sh`) is the cheat sheet, live: open it in
Chrome, click **Connect keyboard** once, and each key lights up while held,
with the active layer and modifiers above. The keyboard reports key events
over a separate USB data channel (raw HID) only while the page is open: the
page pings every 2 seconds, and the keyboard stops sending 5 seconds after the
last ping, so with the page closed nothing is sent and typing is never
delayed.

**Cocot Live** (`viz-app/`) is the same view as a small floating macOS panel
that stays on top of other windows. Build it with `./build.sh app` (needs
the Xcode command line tools) after `./build.sh draw`, then open
`viz-app/Cocot Live.app`. It talks to the keyboard directly through macOS's
HID framework, with the same 2-second ping. It sits in the menu bar: the
keyboard icon shows the active layer and held modifiers (dimmed while the
keyboard is unplugged), a click shows or hides the panel, and right-click has
Quit. Closing the panel only hides it.

## Mac and Windows

Switch on FUN: hold a green key and the cyan key next to it, then press D (Mac) or F
(Windows). A and S pick the mode: DV or QW. Both settings are saved on
the keyboard.

Windows mode changes a few things:

- **Ctrl and Win swap places**, so the home-row mods become GACS (pinky Win,
  ring Alt, middle Ctrl, index Shift), the order the home-row-mods guide
  recommends for Windows, and the pink thumb key sends Ctrl instead of Cmd.
  Ctrl+C then uses the same fingers as ⌘C on the Mac. The outer Ctrl key stays
  Ctrl.
- **Pink thumb + Tab is Alt+Tab** (the app switcher), like ⌘Tab on the Mac;
  keep the thumb down and tap Tab to step through apps. Home-row Ctrl (middle
  finger) + Tab is still Ctrl+Tab, for browser tabs.
- Clipboard keys send Ctrl, line start/end send Home/End, delete word sends
  Ctrl+Bksp, delete line sends Shift+Home then Bksp.
- A home-row Win or Alt held and released on its own no longer opens the Start
  menu or the menu bar.

The cheat sheet shows the Mac labels (⌘ on the middle finger and pink thumb).

## Open ideas

Not built yet, in rough order of value:

- **Screen brightness keys** on FUN, for the Mac.
- **Close-window key** on FUN: ⌘W on Mac, Alt+F4 on Windows.
- **Hyper key** (Ctrl+Alt+Cmd+Shift in one key) for Raycast or window-manager
  shortcuts, for example Esc when tapped and Hyper when held.

## Build, flash and draw

From the top of this repo, `./build.sh flash` compiles the firmware and waits
for the board's bootloader. Start it with Boot on FUN, or double-tap the reset
button. To also clear the saved settings (needed after changing defaults such
as the trackball rotation), unplug the board, hold the top-left key and plug it
back in.

After editing `keymap.c`, run `../../../build.sh draw` (or `./build.sh draw`
from the top) to redraw both SVGs, their PDF copies and the live view. Both
drawings use the board's real shape, converted by `kle_layout.py` from aki27's
VIA file (`cocot46plus_via_rev.json`). The cheat sheet shows the Dvorak layer
you type, with every layer's legend on its key; `cheatsheet.py` lists the extra
legends (⌘/⇧ tricks, knob turns) and `legend.py` the colour key under it. It
uses `keymap-drawer` and `uv`.

Flash space is tight on the ATmega32u4: about 530 bytes are left. Turning
off one-shot keys, limiting the layer count to 8 and trimming the OLED font
to the characters it draws made the room. If more is needed, the candidates
are the trackball numbers on the OLED (about 560 bytes) or the whole OLED
(about 5 KB).
