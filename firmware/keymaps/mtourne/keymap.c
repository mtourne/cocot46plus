// cocot46plus keymap for mtourne: dual-use keys in the style of dlip's engram
// layout, with two base layers:
//   QWERTY - sends normal US keycodes; the OS is set to Dvorak.
//   DVORAK - "hardware Dvorak"; the OS stays on US QWERTY.
// Both produce the same characters for the same physical keys.
//
// Thumb keys are numbered from the trackball outwards and colour-coded on the
// keycaps: 1 green, 2 yellow, 3 pink, 4 purple.

#include QMK_KEYBOARD_H
#include "keymap_extras/keymap_dvorak.h"
#include "raw_hid.h"

enum layers {
    _QWERTY, // base, OS set to Dvorak
    _DVORAK, // base, OS set to US
    _NAV,    // hold either yellow thumb (Space)
    _NUM,    // hold left green thumb
    _FUN,    // hold a green thumb + the cyan mouse key next to it
    _MOUSE,  // turns on by itself while the trackball moves
};

enum custom_keycodes {
    // Clipboard keys: Cmd or Ctrl depending on the OS setting.
    CK_UNDO = SAFE_RANGE,
    CK_CUT,
    CK_COPY,
    CK_PSTE,
    CK_REDO,
    // Delete the previous word: Option+Bksp on Mac, Ctrl+Bksp on Windows.
    CK_DWRD,
    // Delete to the start of the line: Cmd+Bksp on Mac, Shift+Home then Bksp
    // on Windows.
    CK_DLIN,
    // LED colour: steps through white and eight colours; Shift steps back.
    CK_HUE,
    // Line start/end: Cmd+Left/Right on Mac, Home/End on Windows.
    CK_HOME,
    CK_END,
    // OS setting, saved on the keyboard.
    OS_MAC,
    OS_WIN,
    // Trackball scrolls (up/down or sideways) while held.
    CK_SCRL,
    // Symbols on the NUM layer. In the QWERTY base (OS set to Dvorak) these are
    // translated so the OS still types the intended character.
    // Digits and ` ~ \ | ! @ # $ % ^ & * ( ) are the same on both layouts and
    // use plain keycodes.
    SY_EQL,
    SY_LBRC,
    SY_RBRC,
    // NUM operator keys with their own Shift partner: * /, + -, % ^, . ,
    NP_STAR,
    NP_PLUS,
    NP_PERC,
    NP_DOT,
};

// Home-row mods, CAGS order (pinky to index): Ctrl, Alt, Gui, Shift.
// QWERTY base
#define HQ_A MT(MOD_LCTL, KC_A)
#define HQ_S MT(MOD_LALT, KC_S)
#define HQ_D MT(MOD_LGUI, KC_D)
#define HQ_F MT(MOD_LSFT, KC_F)
#define HQ_J MT(MOD_LSFT, KC_J)
#define HQ_K MT(MOD_LGUI, KC_K)
#define HQ_L MT(MOD_LALT, KC_L)
#define HQ_SCLN MT(MOD_LCTL, KC_SCLN)
// DVORAK base (same physical keys)
#define HD_A MT(MOD_LCTL, KC_A)
#define HD_O MT(MOD_LALT, KC_O)
#define HD_E MT(MOD_LGUI, KC_E)
#define HD_U MT(MOD_LSFT, KC_U)
#define HD_H MT(MOD_LSFT, KC_H)
#define HD_T MT(MOD_LGUI, KC_T)
#define HD_N MT(MOD_LALT, KC_N)
#define HD_S MT(MOD_LCTL, KC_S)

// NUM layer home row: the same CAGS mods when held, so e.g. NUM + hold J
// (Shift) + [ gives "{". "/" (translated for Dvorak) and "( )" (shifted
// characters) can't be carried by a mod-tap, so they use stand-ins F13-F15
// and process_record_user sends the real symbol on tap.
// NAV home row: plain modifiers (see the NAV layer).
#define NV_CTL LCTL(KC_NO)
#define NV_ALT LALT(KC_NO)
#define NV_GUI LGUI(KC_NO)
#define NV_SFT LSFT(KC_NO)

#define NM_SLSH MT(MOD_LCTL, KC_F13) // tap /
#define NM_BSLS MT(MOD_LALT, KC_BSLS)
#define NM_LPRN MT(MOD_LGUI, KC_F14) // tap (
#define NM_RPRN MT(MOD_LSFT, KC_F15) // tap )
#define NM_4 MT(MOD_LSFT, KC_4)
#define NM_5 MT(MOD_LGUI, KC_5)
#define NM_6 MT(MOD_LALT, KC_6)
#define NM_0 MT(MOD_LCTL, KC_0)

// Thumbs
#define TH_SPC LT(_NAV, KC_SPC)   // right yellow
#define TH_BSPC LT(_NAV, KC_BSPC) // left yellow
// Left green: delete word when tapped (F17 is a stand-in, replaced in
// process_record_user), NUM when held.
#define TH_DWRD LT(_NUM, KC_F17)
#define TH_ENT LT(_NUM, KC_ENT)   // right green

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // Thumb row, left to right:
  //   purple4 pink3 yellow2 green1 BTN1 | BTN2 green1 yellow2 pink3 purple4
  //   Alt   Cmd     Bksp    DelWord     |      Enter   Space  Tab     Esc
  // Green keys hold NUM, yellow keys hold NAV. The left thumb deletes (Bksp,
  // delete word), the right one types (Enter, Space). Tab next to Space on
  // the right makes Cmd+Tab and Alt+Tab two-thumb chords (left pink or purple
  // + right pink).
  // Right purple is Grave Escape: Esc, but Cmd+` (cycle windows) with Cmd held
  // and ~ with Shift held.
  // Last row: only the middle position is real: the knob click (MS_BTN3).
  // The other five are unused positions in the wiring (greyed out in drawings).
  [_QWERTY] = LAYOUT(
      KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                 KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, // LBRC types / (OS Dvorak)
      KC_LCTL, HQ_A,    HQ_S,    HQ_D,    HQ_F,    KC_G,                 KC_H,    HQ_J,    HQ_K,    HQ_L,    HQ_SCLN, KC_QUOT,
      KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                 KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
                        KC_LALT, KC_LGUI, TH_BSPC, TH_DWRD, MS_BTN1,     MS_BTN2, TH_ENT,  TH_SPC,  KC_TAB,  QK_GESC,
                                                   XXXXXXX, MS_BTN3,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
  ),
  [_DVORAK] = LAYOUT(
      KC_TAB,  KC_QUOT, KC_COMM, KC_DOT,  KC_P,    KC_Y,                 KC_F,    KC_G,    KC_C,    KC_R,    KC_L,    KC_SLSH,
      KC_LCTL, HD_A,    HD_O,    HD_E,    HD_U,    KC_I,                 KC_D,    HD_H,    HD_T,    HD_N,    HD_S,    KC_MINS,
      KC_LSFT, KC_SCLN, KC_Q,    KC_J,    KC_K,    KC_X,                 KC_B,    KC_M,    KC_W,    KC_V,    KC_Z,    KC_RSFT,
                        KC_LALT, KC_LGUI, TH_BSPC, TH_DWRD, MS_BTN1,     MS_BTN2, TH_ENT,  TH_SPC,  KC_TAB,  QK_GESC,
                                                   XXXXXXX, MS_BTN3,     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
  ),
  // Right hand: arrows with Home/End above Left/Right (line start/end, which is
  // Cmd+Left/Right on Mac), PgUp/PgDn on the inner column, delete keys below
  // (delete line, Bksp, delete word, forward Delete). Left thumbs: Delete
  // (yellow) and delete line (green).
  // Left hand after dlip: editing keys, plain Ctrl Alt Cmd Shift on the home
  // row (the home-row mods' order, held in any order with the arrows, e.g.
  // Shift+Alt+Right selects a word) and clipboard keys. They are mod keycodes
  // with no key so they go through mod_config and swap Ctrl/Win on Windows
  // like the home-row mods. The trackball scrolls while this layer is on.
  [_NAV] = LAYOUT(
      _______, XXXXXXX, KC_DEL,  KC_ESC,  KC_ENT,  XXXXXXX,              KC_PGUP, CK_HOME, KC_UP,   CK_END,  XXXXXXX, _______,
      _______, NV_CTL,  NV_ALT,  NV_GUI,  NV_SFT,  XXXXXXX,              KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX, _______,
      _______, CK_UNDO, CK_CUT,  CK_COPY, CK_PSTE, CK_REDO,              CK_DLIN, KC_BSPC, CK_DWRD, KC_DEL,  XXXXXXX, _______,
                        _______, _______, KC_DEL,  CK_DLIN, _______,     _______, _______, _______, _______, _______,
                                                   _______, _______,     _______, _______, _______, _______
  ),
  // One rule: Shift gives the expected partner. Right hand: numpad shaped like
  // the F-keys on FUN (7 8 9 0 on top, a second 0 on the home row), with the
  // operators beside it: * + % and . , each with its arithmetic partner on
  // Shift (* /, + -, % ^, . ,). Left hand: the top row is Shift+1..5 as on a
  // normal keyboard (! @ # $ %); home row / \ ( ) = (Shift: ? | + as usual),
  // with ( ) on the two strongest fingers; [ ] right under the parens (Shift:
  // { }); ^ & and | direct. ` on the outer column (Shift: ~).
  // Thumbs: the cyan mouse keys turn on FUN, so a green thumb (NUM) mashed
  // together with the cyan key next to it gives FUN.
  [_NUM] = LAYOUT(
      KC_GRV,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,              NP_STAR, KC_7,    KC_8,    KC_9,    KC_0,    _______,
      XXXXXXX, NM_SLSH, NM_BSLS, NM_LPRN, NM_RPRN, SY_EQL,               NP_PLUS, NM_4,    NM_5,    NM_6,    NM_0,    _______,
      _______, KC_CIRC, KC_AMPR, SY_LBRC, SY_RBRC, KC_PIPE,              NP_PERC, KC_1,    KC_2,    KC_3,    NP_DOT,  _______,
                        _______, _______, _______, _______, MO(_FUN),    MO(_FUN), _______, _______, _______, _______,
                                                   _______, _______,     _______, _______, _______, _______
  ),
  // System, media, F-keys and trackball settings. RGB row: on/off, colour
  // (white and eight colours; Shift goes back), brightness -/+.
  [_FUN] = LAYOUT(
      QK_BOOT, XXXXXXX, KC_MUTE, KC_VOLD, KC_VOLU, KC_MPLY,              CPI_SW,  KC_F7,   KC_F8,   KC_F9,   KC_F10,  ROT_L15,
      XXXXXXX, PDF(_QWERTY), PDF(_DVORAK), OS_MAC, OS_WIN, XXXXXXX,      SCRL_SW, KC_F4,   KC_F5,   KC_F6,   KC_F11,  ROT_R15,
      XXXXXXX, UG_TOGG, CK_HUE,  UG_VALD, UG_VALU, XXXXXXX,              SCRL_IN, KC_F1,   KC_F2,   KC_F3,   KC_F12,  SCRL_TO,
                        _______, _______, _______, _______, _______,     _______, _______, _______, _______, _______,
                                                   _______, _______,     _______, _______, _______, _______
  ),
  // On while the trackball moves, off after a short pause or when you type.
  // Mouse buttons sit where they are on NAV, mirrored on the right hand so
  // either hand can click while the other rolls the ball (Mse 1 under the
  // index finger, hold-to-scroll under the pinky). Everything else falls
  // through.
  [_MOUSE] = LAYOUT(
      _______, _______, _______, _______, _______, _______,              _______, _______, _______, _______, _______, _______,
      _______, CK_SCRL, MS_BTN3, MS_BTN2, MS_BTN1, _______,              _______, MS_BTN1, MS_BTN2, MS_BTN3, CK_SCRL, _______,
      _______, _______, _______, _______, _______, _______,              _______, _______, _______, _______, _______, _______,
                        _______, _______, _______, _______, _______,     _______, _______, _______, _______, _______,
                                                   _______, _______,     _______, _______, _______, _______
  ),
};

// Knob: scroll on the base layers, page up/down on NAV, volume on FUN.
// NUM falls through to the base layer.
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_QWERTY] = {ENCODER_CCW_CW(MS_WHLU, MS_WHLD)},
    [_DVORAK] = {ENCODER_CCW_CW(MS_WHLU, MS_WHLD)},
    [_NAV]    = {ENCODER_CCW_CW(KC_PGUP, KC_PGDN)},
    [_NUM]    = {ENCODER_CCW_CW(_______, _______)},
    [_FUN]    = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [_MOUSE]  = {ENCODER_CCW_CW(_______, _______)},
};

// Chordal Hold handedness. Thumbs and extra keys are '*' (exempt), so a
// thumb can be held with keys on the same hand.
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = LAYOUT(
    'L', 'L', 'L', 'L', 'L', 'L',      'R', 'R', 'R', 'R', 'R', 'R',
    'L', 'L', 'L', 'L', 'L', 'L',      'R', 'R', 'R', 'R', 'R', 'R',
    'L', 'L', 'L', 'L', 'L', 'L',      'R', 'R', 'R', 'R', 'R', 'R',
              '*', '*', '*', '*', '*', '*', '*', '*', '*', '*',
                             '*', '*', '*', '*', '*', '*'
);

// ---- OS setting ----------------------------------------------------------

typedef union {
    uint32_t raw;
    struct {
        bool is_windows : 1;
    };
} user_config_t;

static user_config_t user_config;

void keyboard_post_init_user(void) {
    user_config.raw = eeconfig_read_user();
    set_auto_mouse_layer(_MOUSE);
    set_auto_mouse_enable(true);
}

// Green thumbs (Bksp, Enter): NUM only after a full hold, so a fast Bksp or
// Enter rolled into the next letter stays a Bksp or Enter.
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    return keycode != TH_BSPC && keycode != TH_DWRD && keycode != TH_ENT;
}

// Flow Tap: a home-row key pressed within FLOW_TAP_TERM of the previous key is
// always a letter. The Shift keys use a shorter window, so a quick capital
// (hold H, tap a letter, release) works after a short pause or a space, while
// H in the middle of a fast word ("the", "he") never turns into Shift.
#define SHIFT_FLOW_TAP_TERM 80
uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prev_keycode) {
    if (!is_flow_tap_key(keycode) || !is_flow_tap_key(prev_keycode)) {
        return 0;
    }
    switch (keycode) {
        case HQ_F:
        case HQ_J:
        case HD_U:
        case HD_H:
            return SHIFT_FLOW_TAP_TERM;
    }
    return FLOW_TAP_TERM;
}

// Caps Word (both outer Shifts together): capitals until the end of the word.
// In the QWERTY base the OS is Dvorak, so the letter keys are not KC_A..KC_Z:
// Dvorak s w v z come from US ; , . / and US q w e z type ' , . ;
static bool os_is_dvorak(void);

bool caps_word_press_user(uint16_t keycode) {
    bool dvorak_os = os_is_dvorak();
    switch (keycode) {
        case KC_Q:
        case KC_W:
        case KC_E:
        case KC_Z:
            if (dvorak_os) {
                return false; // ' , . ; end the word
            }
            // fall through
        case KC_A ... KC_D:
        case KC_F ... KC_P:
        case KC_R ... KC_V:
        case KC_X:
        case KC_Y:
            add_weak_mods(MOD_BIT(KC_LSFT));
            return true;
        case KC_SCLN:
        case KC_COMM:
        case KC_DOT:
        case KC_SLSH:
            if (dvorak_os) {
                add_weak_mods(MOD_BIT(KC_LSFT)); // Dvorak s w v z
                return true;
            }
            return false;
        case KC_QUOT: // Dvorak - with the OS on Dvorak
        case KC_MINS: // Dvorak - with the OS on US
            if (keycode == (dvorak_os ? KC_QUOT : KC_MINS)) {
                add_weak_mods(MOD_BIT(KC_LSFT)); // - becomes _
                return true;
            }
            return false;
        case KC_1 ... KC_0:
        case KC_BSPC:
        case KC_DEL:
            return true;
    }
    return false;
}

// Keep the MOUSE layer on while the scroll key is held.
bool is_mouse_record_user(uint16_t keycode, keyrecord_t *record) {
    return keycode == CK_SCRL;
}

void eeconfig_init_user(void) {
    user_config.raw = 0; // default: macOS
    eeconfig_update_user(user_config.raw);
}

// Windows mode: swap Ctrl and Win, turning CAGS into GACS (the order the
// home-row-mods guide recommends for Windows), so Ctrl+C uses the same fingers
// as Cmd+C on the Mac. Applies to the home-row holds and the pink Cmd thumb;
// the outer Ctrl key stays Ctrl. QMK calls these for every key it sends.
uint16_t keycode_config(uint16_t keycode) {
    return user_config.is_windows && keycode == KC_LGUI ? KC_LCTL : keycode;
}

uint8_t mod_config(uint8_t mod) {
    if (user_config.is_windows && (((mod & MOD_RCTL) == MOD_LCTL) ^ ((mod & MOD_RGUI) == MOD_LGUI))) {
        mod ^= MOD_LCTL | MOD_LGUI;
    }
    return mod;
}

static bool os_is_dvorak(void) {
    return get_highest_layer(default_layer_state) == _QWERTY;
}

// Tap Cmd/Ctrl (plus extra_mod) + letter. In the QWERTY base the OS is set
// to (plain) Dvorak, so the letter is sent from its Dvorak position.
static void tap_clipboard(uint16_t us_key, uint16_t dv_key, uint8_t extra_mod) {
    uint8_t mod = (user_config.is_windows ? MOD_LCTL : MOD_LGUI) | extra_mod;
    // A mod-modified keycode is (mods << 8) | key, e.g. LGUI(KC_C) = 0x0800 | KC_C.
    // (Adding QK_MODS here would also add Ctrl: QK_MODS == QK_LCTL == 0x0100.)
    tap_code16(((uint16_t)mod << 8) | (os_is_dvorak() ? dv_key : us_key));
}

// NUM layer symbols: US key, and the US key that types the same character
// when the OS is set to Dvorak.
static const uint16_t PROGMEM symbol_keys[][2] = {
    [SY_EQL - SY_EQL]  = {KC_EQL, DV_EQL},
    [SY_LBRC - SY_EQL] = {KC_LBRC, DV_LBRC},
    [SY_RBRC - SY_EQL] = {KC_RBRC, DV_RBRC},
};

// NUM operator keys: {US, Dvorak} for the key, then for its Shift partner.
static const uint16_t PROGMEM operator_keys[][4] = {
    [NP_STAR - NP_STAR] = {KC_ASTR, KC_ASTR, KC_SLSH, DV_SLSH},
    [NP_PLUS - NP_STAR] = {KC_PLUS, DV_PLUS, KC_MINS, DV_MINS},
    [NP_PERC - NP_STAR] = {KC_PERC, KC_PERC, KC_CIRC, KC_CIRC},
    [NP_DOT - NP_STAR]  = {KC_DOT, DV_DOT, KC_COMM, DV_COMM},
};

// Live visualiser (visualizer.html): report each key event and layer change
// over raw HID as 'K', row, col, pressed, layer, mods. Row 0xFF means a layer
// change only. Reports are sent only while the page is open: it pings every
// 2 s, and sending stops 5 s after the last ping, so with the page closed the
// keyboard never waits on an unread channel.
static uint16_t visualizer_ping;
static bool     visualizer_open = false;

void raw_hid_receive(uint8_t *data, uint8_t length) {
    visualizer_open = true;
    visualizer_ping = timer_read();
}

static void send_to_visualizer(uint8_t row, uint8_t col, uint8_t pressed, uint8_t layer) {
    if (!visualizer_open || timer_elapsed(visualizer_ping) > 5000) {
        visualizer_open = false;
        return;
    }
    uint8_t report[32] = {'K', row, col, pressed, layer, get_mods()}; // raw HID reports are 32 bytes
    raw_hid_send(report, sizeof(report));
}

// The last key pressed on FUN, shown by name on the OLED (KC_NO: none yet).
static uint16_t fun_key = KC_NO;

// LED colours for the Colour key: white first, then round the wheel.
static const uint8_t PROGMEM led_colours[][2] = {
    {0, 0}, {0, 255}, {21, 255}, {43, 255}, {85, 255}, {128, 255}, {170, 255}, {191, 255}, {234, 128},
};

// Report each key after QMK has handled it, so mods and layer are current.
void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    send_to_visualizer(record->event.key.row, record->event.key.col, record->event.pressed, get_highest_layer(layer_state));
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed && get_highest_layer(layer_state) == _FUN) {
        fun_key = keycode;
    }
    // Windows opens the Start menu on a lone Win press and focuses the menu
    // bar on a lone Alt press. When a home-row Win/Alt is held and released
    // without any other key, tap an unused key (F18) first so Windows does
    // not see a lone press. macOS ignores lone Cmd/Option, so Mac mode skips it.
    static uint16_t last_pressed = KC_NO;
    if (record->event.pressed) {
        last_pressed = keycode;
    } else if (user_config.is_windows && IS_QK_MOD_TAP(keycode) && record->tap.count == 0 && last_pressed == keycode &&
               (QK_MOD_TAP_GET_MODS(keycode) & (MOD_LCTL | MOD_LALT) & 0x0F)) {
        // MOD_LCTL here is the pinky hold, which sends Win in Windows mode.
        tap_code(KC_F18);
    }

    // Windows mode: pink thumb (Ctrl there) + Tab gives Alt+Tab, matching
    // Cmd+Tab on the Mac. Alt stays held until the thumb is released, so Tab
    // steps through apps. Home-row Ctrl + Tab is still Ctrl+Tab (browser tabs).
    static bool thumb_ctrl = false, alt_tab = false;
    if (keycode == KC_LGUI) {
        thumb_ctrl = record->event.pressed;
        if (!record->event.pressed && alt_tab) {
            alt_tab = false;
            unregister_code(KC_LALT);
        }
    } else if (keycode == KC_TAB && record->event.pressed && user_config.is_windows && thumb_ctrl && !alt_tab) {
        alt_tab = true;
        del_mods(MOD_BIT(KC_LCTL));
        register_code(KC_LALT);
    }

    // Left green thumb: tapped, it deletes the previous word.
    if (keycode == TH_DWRD && record->tap.count) {
        if (record->event.pressed) {
            tap_code16(user_config.is_windows ? C(KC_BSPC) : A(KC_BSPC));
        }
        return false;
    }

    // NUM home row: replace the stand-in tap with the real symbol.
    if (record->tap.count && record->event.pressed) {
        uint16_t symbol = KC_NO;
        switch (keycode) {
            case NM_SLSH:
                symbol = os_is_dvorak() ? DV_SLSH : KC_SLSH; // Shift held: ?
                break;
            case NM_LPRN:
                symbol = KC_LPRN;
                break;
            case NM_RPRN:
                symbol = KC_RPRN;
                break;
        }
        if (symbol != KC_NO) {
            tap_code16(symbol);
            return false;
        }
    }

    // Operator keys: with Shift, send the partner instead (Shift released for it).
    if (keycode >= NP_STAR && keycode <= NP_DOT) {
        if (record->event.pressed) {
            uint8_t mods    = get_mods();
            bool    shifted = mods & MOD_MASK_SHIFT;
            del_mods(MOD_MASK_SHIFT);
            tap_code16(pgm_read_word(&operator_keys[keycode - NP_STAR][(shifted ? 2 : 0) + (os_is_dvorak() ? 1 : 0)]));
            set_mods(mods);
        }
        return false;
    }

    if (keycode >= SY_EQL && keycode <= SY_RBRC) {
        uint16_t code = pgm_read_word(&symbol_keys[keycode - SY_EQL][os_is_dvorak() ? 1 : 0]);
        if (record->event.pressed) {
            register_code16(code);
        } else {
            unregister_code16(code);
        }
        return false;
    }

    if (keycode == CK_SCRL) {
        cocot_set_scroll_mode(record->event.pressed || layer_state_is(_NAV));
        return false;
    }

    if (!record->event.pressed) {
        return true;
    }
    switch (keycode) {
        case CK_UNDO:
            tap_clipboard(KC_Z, DV_Z, 0);
            return false;
        case CK_CUT:
            tap_clipboard(KC_X, DV_X, 0);
            return false;
        case CK_COPY:
            tap_clipboard(KC_C, DV_C, 0);
            return false;
        case CK_PSTE:
            tap_clipboard(KC_V, DV_V, 0);
            return false;
        case CK_REDO: // Cmd+Shift+Z on Mac, Ctrl+Y on Windows
            if (user_config.is_windows) {
                tap_clipboard(KC_Y, DV_Y, 0);
            } else {
                tap_clipboard(KC_Z, DV_Z, MOD_LSFT);
            }
            return false;
        case CK_HOME:
            tap_code16(user_config.is_windows ? KC_HOME : G(KC_LEFT));
            return false;
        case CK_END:
            tap_code16(user_config.is_windows ? KC_END : G(KC_RGHT));
            return false;
        case CK_HUE: {
            static uint8_t colour = 0;
            uint8_t        count  = sizeof(led_colours) / sizeof(led_colours[0]);
            colour                = (colour + ((get_mods() & MOD_MASK_SHIFT) ? count - 1 : 1)) % count;
            rgblight_sethsv(pgm_read_byte(&led_colours[colour][0]), pgm_read_byte(&led_colours[colour][1]), rgblight_get_val());
            return false;
        }
        case CK_DLIN:
            if (user_config.is_windows) {
                tap_code16(S(KC_HOME));
                tap_code(KC_BSPC);
            } else {
                tap_code16(G(KC_BSPC));
            }
            return false;
        case CK_DWRD:
            tap_code16(user_config.is_windows ? C(KC_BSPC) : A(KC_BSPC));
            return false;
        case OS_MAC:
        case OS_WIN:
            user_config.is_windows = (keycode == OS_WIN);
            eeconfig_update_user(user_config.raw);
            return false;
    }
    return true;
}

// ---- Layers, LEDs and trackball --------------------------------------------

// The two layer-indicator LEDs, at half the board's brightness cap (120).
#define INDICATOR_VAL 60
static void set_indicator(uint8_t hue, uint8_t sat, uint8_t val) {
    rgblight_sethsv_range(hue, sat, val ? INDICATOR_VAL : 0, 0, 2);
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // The trackball scrolls whenever NAV is on, even with MOUSE above it.
    cocot_set_scroll_mode(layer_state_cmp(state, _NAV));
    switch (get_highest_layer(state)) {
        case _NAV:
            set_indicator(HSV_YELLOW);
            break;
        case _NUM:
            set_indicator(HSV_GREEN);
            break;
        case _FUN:
            set_indicator(HSV_MAGENTA);
            break;
        case _MOUSE:
            set_indicator(HSV_CYAN);
            break;
        default:
            if (is_caps_word_on()) {
                set_indicator(HSV_RED);
            } else {
                set_indicator(0, 0, 0);
            }
            break;
    }
    rgblight_set_effect_range(2, 10);
    send_to_visualizer(0xFF, 0, 0, get_highest_layer(state)); // the new layer
    return state;
}

// LEDs off after LED_IDLE_MS without a key, knob or trackball input, and back
// on with the next input. Uses the non-saving calls, and leaves them off if
// they were already switched off with the FUN key. (The OLED has QMK's own
// 60 s timeout; the trackball sensor dims itself when idle.)
#define LED_IDLE_MS 60000 // same as the OLED
void housekeeping_task_user(void) {
    static bool asleep = false, were_on = false;
    bool        idle   = last_input_activity_elapsed() > LED_IDLE_MS;
    if (idle && !asleep) {
        asleep  = true;
        were_on = rgblight_is_enabled();
        if (were_on) {
            rgblight_disable_noeeprom();
        }
    } else if (!idle && asleep) {
        asleep = false;
        if (were_on) {
            rgblight_enable_noeeprom();
        }
    }
}

// Caps Word turning on or off: refresh the indicator LEDs (red while on).
void caps_word_set_user(bool active) {
    layer_state_set_user(layer_state);
}

#ifdef OLED_ENABLE
// 128x32 OLED, 21x4 characters, text centred. The modifier icons are drawn in
// glcdfont.c by font_art.py.
#define TEXT_COL 0
#define TEXT_WIDTH 21
#define MOD_GLYPHS 0x80 // Ctrl, Option, Command, Shift: 2x2 cells each
#define OS_GLYPHS 0x90  // Apple, Windows: 2x2 cells each

// Trackball settings, defined in cocot46plus.c.
extern uint16_t cpi_array[], scrl_div_array[], angle_array[];

// Append a RAM string to buf at position n; returns the new length.
static uint8_t append(char *buf, uint8_t n, const char *str) {
    while (*str && n < TEXT_WIDTH) {
        buf[n++] = *str++;
    }
    return n;
}

// Write text centred in the text area of a row. The whole area is written in
// one go, so the display only redraws when the text changes.
static void oled_write_centred(uint8_t row, const char *text, uint8_t len) {
    char line[TEXT_WIDTH + 1];
    memset(line, ' ', TEXT_WIDTH);
    memcpy(line + (TEXT_WIDTH - len) / 2, text, len);
    line[TEXT_WIDTH] = '\0';
    oled_set_cursor(TEXT_COL, row);
    oled_write(line, false);
}

// Short name of the last FUN key pressed, with the new value where there is
// one (e.g. "CPI  875", "ROTATE +30"). Empty for keys whose effect is obvious.
static uint8_t fun_key_label(char *buf) {
    uint8_t n = 0;
    int16_t angle;
    switch (fun_key) {
        case CPI_SW:
            n = append(buf, n, "CPI ");
            return append(buf, n, get_u16_str(cpi_array[cocot_config.cpi_idx], ' ') + 1);
        case SCRL_SW:
            n = append(buf, n, "SCROLL");
            return append(buf, n, get_u8_str(scrl_div_array[cocot_config.scrl_div], ' ') + 1);
        case ROT_L15:
        case ROT_R15:
            angle = (int16_t)angle_array[cocot_config.rotation_angle];
            n     = append(buf, n, angle < 0 ? "ROTATE -" : "ROTATE +");
            return append(buf, n, get_u8_str(angle < 0 ? -angle : angle, ' ') + 1);
        case SCRL_IN:
            return append(buf, n, "SCROLL INV");
        case SCRL_TO:
            return append(buf, n, "SCROLL LOCK");
        case CK_HUE:
            return append(buf, n, "COLOUR");
        case UG_TOGG:
            return append(buf, n, "LIGHTS");
        case UG_VALD:
            return append(buf, n, "DIMMER");
        case UG_VALU:
            return append(buf, n, "BRIGHTER");
        case KC_MUTE:
            return append(buf, n, "MUTE");
        case KC_VOLD:
            return append(buf, n, "VOL -");
        case KC_VOLU:
            return append(buf, n, "VOL +");
        case KC_MPLY:
            return append(buf, n, "PLAY");
        case PDF(_QWERTY):
            return append(buf, n, "OS DVORAK");
        case PDF(_DVORAK):
            return append(buf, n, "OS QWERTY");
        case OS_MAC:
            return append(buf, n, "MAC");
        case OS_WIN:
            return append(buf, n, "WINDOWS");
    }
    return 0;
}

//   rows 0-1  ⎈ ⌥ ⌘ ⇧ at double size in fixed slots (CAGS finger order),
//             each shown only while held; on FUN, row 1 instead shows the
//             short name of the last FUN key pressed
//   rows 2-3  OS icon (Apple or Windows) at double size on the left; on
//             row 2, the layer from column 8:
//             NAV, NUM, FUNC, MOUSE, CAPS during Caps Word, or nothing on the base layer; mode
//             on the right (DV: computer set to Dvorak; QW: set to US)
bool oled_task_user(void) {
    char    buf[TEXT_WIDTH];
    char    line[TEXT_WIDTH + 1];
    uint8_t layer   = get_highest_layer(layer_state);
    uint8_t mods    = get_mods();
    uint8_t held[4] = {mods & MOD_MASK_CTRL, mods & MOD_MASK_ALT, mods & MOD_MASK_GUI, mods & MOD_MASK_SHIFT};
    line[TEXT_WIDTH] = '\0';

    if (layer == _FUN) {
        oled_write_centred(0, buf, 0);
        oled_write_centred(1, buf, fun_key_label(buf));
    } else {
        fun_key = KC_NO;
        // One fixed 2x2-cell slot per modifier (cols 3, 7, 11, 15). Each row
        // is written as a full line, so nothing from FUN's label stays behind.
        for (uint8_t row = 0; row < 2; row++) {
            memset(line, ' ', TEXT_WIDTH);
            for (uint8_t i = 0; i < 4; i++) {
                if (held[i]) {
                    line[3 + 4 * i] = MOD_GLYPHS + 4 * i + 2 * row;
                    line[4 + 4 * i] = MOD_GLYPHS + 4 * i + 2 * row + 1;
                }
            }
            oled_set_cursor(0, row);
            oled_write(line, false);
        }
    }

    // Row 2: one full line, so it only redraws on a change.
    const char *name;
    switch (layer) {
        case _NAV:
            name = "NAV";
            break;
        case _NUM:
            name = "NUM";
            break;
        case _FUN:
            name = "FUNC";
            break;
        case _MOUSE:
            name = "MOUSE";
            break;
        default:
            name = is_caps_word_on() ? "CAPS" : ""; // base layer: nothing
            break;
    }
    // Rows 2-3: the OS icon at double size on the left (its top half on
    // row 2, bottom half on row 3); layer and mode on row 2.
    uint8_t os_glyph = OS_GLYPHS + (user_config.is_windows ? 4 : 0);
    memset(line, ' ', TEXT_WIDTH);
    line[1] = os_glyph;
    line[2] = os_glyph + 1;
    memcpy(line + 8, name, strlen(name)); // names start in the same column
    memcpy(line + TEXT_WIDTH - 3, os_is_dvorak() ? "DV" : "QW", 2);
    oled_set_cursor(0, 2);
    oled_write(line, false);
    memset(line, ' ', TEXT_WIDTH);
    line[1] = os_glyph + 2;
    line[2] = os_glyph + 3;
    oled_set_cursor(0, 3);
    oled_write(line, false);
    return false;
}
#endif
