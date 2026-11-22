# Future: a wireless cocot46plus

Notes for making this board wireless one day: what the hardware needs, which
firmware could run it, and what to check again before starting. Everything
here was researched in November 2026; firmware support moves quickly, so
re-check each "today" before deciding.

Nothing here is planned yet. The wired board works well as it is.

## The short version

Swapping the controller is not enough. The trackball sensor this board uses
was made for wired mice and would drain a battery, so a wireless version is
effectively a new PCB: a Bluetooth controller, a low-power sensor, a battery
and a power switch. QMK, the firmware this layout uses today, has no Bluetooth
for the controllers that make sense, so the layout would move to RMK or ZMK.

## Why a controller swap is not enough

The board's sensor is an ADNS-5050. Its lamp stays on all the time, which is
normal for a sensor meant for wired mice, and its datasheet gives these
figures:

- Supply: 4.0–5.25 V, a 5 V part.
- Current: about 11 mA idle and 13 mA while tracking. A power-down mode
  (about 200 µA) exists, but the firmware would have to use it.

Bluetooth controllers run at 3.3 V. The BLE Micro Pro, for example, is not
5 V tolerant on most pins, so the sensor would need its own 5 V supply and
level shifting on its signal lines. At those currents the sensor alone would
empty a 300 mAh battery in about a day.

The board's 12 RGB LEDs (WS2812) also draw a lot of power. On a battery
board they would stay off or be left out.

## What a wireless board would need

- **Controller:** an nRF52840 board with the Pro Micro footprint, such as a
  nice!nano or a clone. It also has far more flash (1 MB, against 28 KB
  today).
- **Sensor:** one made for wireless mice, which dims its lamp when idle. The
  PMW3610 is the usual choice; sekigon also makes a small PAW3222 module.
- **Power:** a battery, room for it in the case, and an on/off switch so it
  never drains overnight.
- **LEDs:** off, or left out.

## Firmware options

### QMK

Mainline QMK supports Bluetooth only on old AVR modules (Adafruit Bluefruit,
RN-42). Its wireless docs say no new wireless keyboards will be accepted until
that code is refactored. Recent contributions toward it (multiple Bluetooth
profiles and a 2.4 GHz dongle API in 2026, a driver restructure in 2024) were
closed as stale without review. No pull request found adds the nRF52840. The older community
forks for it (joric, sekigon's first branch) date from 2020–2023 and are no
longer maintained.

If QMK adds nRF52840 Bluetooth by the time this starts, the layout carries
over almost unchanged, so check this first.

### BLE Micro Pro (sekigon's QMK fork)

A Bluetooth controller with the Pro Micro footprint, running a maintained QMK
fork (`dev/ble-micro-pro` in `sekigon-gonnoc/vial-qmk`). It is the closest
thing to "QMK, but wireless" and supports raw HID, so the live view could
work. The catch: the fork is based on QMK from mid-2024, before Chordal Hold
and Flow Tap, which the home-row mods rely on. Using it means copying those
across from current QMK.

### RMK

A Rust firmware with Bluetooth on the nRF52840, Pico W, ESP32 and others
(v0.9.0, August 2026, still before 1.0). The board is described in a
`keyboard.toml` file, with Rust for anything custom. It covers most of this
layout:

| This layout | RMK today |
|---|---|
| Home-row mods, per-key tap-hold settings, tap-then-hold repeat | Yes (tap-hold profiles, `quick_tap_timeout`) |
| Flow Tap | Yes, but one global window, so no shorter window for Shift |
| Chordal Hold | `unilateral_tap`, experimental |
| NUM Shift pairs, Esc taps | Yes ("forks") |
| Mouse layer on ball movement | Yes, with timeout, drift threshold, modifier-clicks and a timer reset after clicks |
| Hold-to-scroll | Yes (pointing processor scroll mode) |
| PMW3610 sensor that sleeps | Yes |
| Caps Word, repeat key, combos, macros, tap dance, knob | Yes |
| OLED | Yes (SSD1306 and others) |
| Live view | Possible through Rynk, its experimental host protocol, which reports the layer and held keys |
| Mac/Windows and DV/QW modes | Custom Rust code ("processors") |
| RGB LEDs | No support found |
| ADNS-5050 | No driver |

RMK fits best if it matures: the custom parts of this layout become ordinary
Rust code with no flash budget to fight.

### ZMK

The long-established firmware for nice!nano boards, with a PMW3610 driver.
Its hold-tap has options similar to Chordal Hold and Flow Tap (positional
holds, a required idle time before a hold). The
Mac/Windows modes, the OLED screen and the live view would all need custom
work, and ZMK has no built-in raw HID.

## Ideas that need the bigger chip

- **Abbreviation chords**, like dlip's
  [abbrgen](https://github.com/dlip/abbrgen): a short chord plus a thumb key
  types a whole word. Its list has about 1,100 words, far beyond today's
  flash. abbrgen can also export to espanso, a text-expansion app, to try the
  idea with no firmware at all.

## Options ruled out

- **A Bluetooth dongle** between the keyboard's USB and the computer. The
  keyboard would still draw full power from the dongle's battery, the live
  view would almost certainly not pass through, and the board could no longer
  tell when the computer sleeps.
- **Only swapping in a Bluetooth controller.** See above: the sensor's
  voltage and current.

## Before starting

1. Check whether mainline QMK supports nRF52840 Bluetooth.
2. Check RMK's version and whether Chordal Hold (`unilateral_tap`) and RGB
   support have matured.
3. Test the home-row mods on a nice!nano with RMK or ZMK before designing a
   PCB.

## Sources

- [ADNS-5050 datasheet](https://www.espruino.com/datasheets/ADNS5050.pdf)
- [BLE Micro Pro docs](https://github.com/sekigon-gonnoc/BLE-Micro-Pro) and
  [its QMK fork](https://github.com/sekigon-gonnoc/vial-qmk)
- [QMK wireless docs](https://develop-docs.qmk.fm/features/wireless), and pull
  requests [#25837](https://github.com/qmk/qmk_firmware/pull/25837),
  [#26199](https://github.com/qmk/qmk_firmware/pull/26199),
  [#26203](https://github.com/qmk/qmk_firmware/pull/26203) and
  [#22783](https://github.com/qmk/qmk_firmware/pull/22783)
- [joric's HHKB-BLE notes](https://github.com/joric/qmk/wiki/hhkb_ble)
- [RMK](https://github.com/HaoboGu/rmk) and its
  [wireless docs](https://rmk.rs/docs/features/wireless)
- [A Keyball39 BLE Micro Pro power write-up](https://note.com/hundert_phigik/n/n4d6743908808?hl=en)
