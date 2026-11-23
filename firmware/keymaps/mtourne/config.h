#pragma once

// Tap-hold timing. Thumb keys switch layer after 190 ms (dlip's engram value);
// home-row keys wait 250 ms (Getreuer's advice, see get_tapping_term in
// keymap.c), so a letter held a little long stays a letter.
#undef TAPPING_TERM
#define TAPPING_TERM 190
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD
#define PERMISSIVE_HOLD_PER_KEY

// Opposite-hands rule for home-row mods. Thumb keys are exempt (see
// chordal_hold_layout in keymap.c) so Space/Nav works with either hand.
#define CHORDAL_HOLD

// While typing fast, home-row keys always act as letters: within this time of
// any key except a modifier (Shift uses a shorter window, see
// get_flow_tap_term in keymap.c).
#define FLOW_TAP_TERM 200


// Turn on the MOUSE layer while the trackball moves (layer set in keymap.c).
#define POINTING_DEVICE_AUTO_MOUSE_ENABLE

// Font with Apple/Windows icons for the OLED status line.
#undef OLED_FONT_H
#define OLED_FONT_H "keyboards/aki27/cocot46plus/keymaps/mtourne/glcdfont.c"
// The font holds only what the OLED draws: text and the icons (saves ~620 bytes).
#define OLED_FONT_START 0x20
#define OLED_FONT_END 0x97

// Trackball defaults (see cocot46plus.c). Scroll speed is a bit shift: higher
// is slower. aki27's range is 1-6 (default 5); ours is 3-8 (default 7, 4x
// slower), still adjustable with the Scroll speed key on FUN.
#define COCOT_SCROLL_DIVIDERS { 3, 4, 5, 6, 7, 8 }
// Rotation: index into { -60, -45, -30, -15, 0, 15, 30, 45, 60 }; 6 = +30
// degrees, for rolling the ball with the left thumb. Applies after the saved
// settings are cleared; otherwise use Rotate +15 on FUN.
#define COCOT_ROTATION_DEFAULT 6

// Caps Word: press both outer Shift keys together.
#define BOTH_SHIFTS_TURNS_ON_CAPS_WORD

// Pointer speed (CPI) steps for the CPI key on FUN. aki27's are
// { 250, 500, 750, 1000, 1250 } with 1250 as default; ours are lower and finer,
// with the default (and the saved index 4) at 875, about 30% slower.
#define COCOT_CPI_OPTIONS { 375, 500, 625, 750, 875, 1000 }

// Default LED colour (after the saved settings are cleared): the deep purple
// of the Colour key's list (hue 191, full saturation). The Colour key on FUN
// steps through white and the other colours.
#define RGBLIGHT_DEFAULT_HUE 191
#define RGBLIGHT_DEFAULT_SAT 255

// Static colour only (set with the Colour key on FUN): drop the board's
// animations, which also frees flash.
#undef RGBLIGHT_EFFECT_RAINBOW_MOOD
#undef RGBLIGHT_EFFECT_RAINBOW_SWIRL
#undef RGBLIGHT_EFFECT_STATIC_GRADIENT

// Flash savers: no one-shot keys are used, and 6 layers fit in 8 bits.
#define NO_ACTION_ONESHOT
#define LAYER_STATE_8BIT

// LEDs off while the computer sleeps (idle timeout: LED_IDLE_MS in keymap.c).
#define RGBLIGHT_SLEEP
