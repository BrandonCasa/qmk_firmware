/* Copyright 2021 Glorious, LLC <salman@pcgamingrace.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "print.h"

// -- custom virtual WASD keys --
enum custom_keycodes {
    L_W = SAFE_RANGE,
    L_S,
    L_A,
    L_D,
    DB_TOG
};

// -- debounce toggle state --
volatile bool debounce_fast_mode = false;

/* Log helper – short and sweet */
static inline void log_dbg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[128];
    vsnprintf(buf, sizeof(buf), fmt, args);
    dprintf("%s", buf);
    va_end(args);
}

// -- generic axis struct and state --
typedef struct {
    bool     pos_pressed;
    bool     neg_pressed;
    uint16_t pos_keycode;
    uint16_t neg_keycode;
    uint16_t current_key;
} axis_t;

static axis_t vertical = {
    .pos_pressed   = false,
    .neg_pressed   = false,
    .pos_keycode   = KC_W,
    .neg_keycode   = KC_S,
    .current_key   = 0,
};

static axis_t horizontal = {
    .pos_pressed   = false,
    .neg_pressed   = false,
    .pos_keycode   = KC_D,
    .neg_keycode   = KC_A,
    .current_key   = 0,
};

/**
 * Generic axis update.
 *  - ax: pointer to the axis state
 *  - is_pos: true for pos_keycode (W or D), false for neg_keycode (S or A)
 *  - pressed: whether the virtual key was pressed
 */
void update_axis(axis_t *ax, bool is_pos, bool pressed) {
    // update pressed state
    if (is_pos) ax->pos_pressed = pressed;
    else        ax->neg_pressed = pressed;

    uint16_t to_unregister = 0;
    uint16_t to_register   = 0;

    // both down: most recent wins
    if (ax->pos_pressed && ax->neg_pressed) {
        if (pressed) {
            to_register   = is_pos ? ax->pos_keycode : ax->neg_keycode;
            to_unregister = is_pos ? ax->neg_keycode : ax->pos_keycode;
            ax->current_key = to_register;
        }
        // if releasing one but still both pressed, no change here
    }
    // only pos held
    else if (ax->pos_pressed) {
        to_register   = ax->pos_keycode;
        to_unregister = (ax->current_key == ax->neg_keycode) ? ax->neg_keycode : 0;
        ax->current_key = ax->pos_keycode;
    }
    // only neg held
    else if (ax->neg_pressed) {
        to_register   = ax->neg_keycode;
        to_unregister = (ax->current_key == ax->pos_keycode) ? ax->pos_keycode : 0;
        ax->current_key = ax->neg_keycode;
    }
    // neither held
    else {
        to_unregister   = ax->current_key;
        ax->current_key = 0;
    }

    // apply key events
    if (to_unregister) unregister_code(to_unregister);
    if (to_register)   register_code(to_register);
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_PSCR,          KC_MUTE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,          KC_DEL,
        KC_TAB,  KC_Q,    L_W,     KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_PGUP,
        KC_CAPS, L_A,     L_S,     L_D,     KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,           KC_PGDN,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, KC_UP,   KC_END,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, MO(1),   KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [1] = LAYOUT(
        _______, KC_MYCM, KC_WHOM, KC_CALC, KC_MSEL, KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP, KC_MUTE, KC_VOLD, KC_VOLU, _______, _______,          _______,
        _______, RGB_TOG, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, RGB_VAI, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, QK_BOOT,          _______,
        _______, _______, RGB_VAD, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
        _______,          RGB_HUI, _______, _______, _______, _______, NK_TOGG, _______, _______, _______, _______,          _______, RGB_MOD, _______,
        DB_TOG,  _______, DB_TOGG,                            _______,                            _______, _______, _______, RGB_SPD, RGB_RMOD, RGB_SPI
    ),

    [2] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______, _______, _______
    ),

    [3] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
        _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______, _______, _______
    ),

};
// clang-format on

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [1] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
    [2] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) },
    [3] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) }
};
#endif

// -- encoder press/double/triple tap logic --
static uint16_t encoder_timer        = 0;
static uint8_t  encoder_press_count  = 0;
static uint16_t encoder_timeout_add  = 0;

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code_delay(KC_VOLU, 10);
        } else {
            tap_code_delay(KC_VOLD, 10);
        }
    }
    return false;
}

void matrix_scan_user(void) {
    if (encoder_timer &&
        (timer_elapsed(encoder_timer) - encoder_timeout_add) > 1000) {
        switch (encoder_press_count) {
            case 3:
                tap_code(KC_MEDIA_PREV_TRACK);
                log_dbg("Encoder tap #%u, previous song.\n", encoder_press_count);
                break;
            case 2:
                tap_code(KC_MEDIA_NEXT_TRACK);
                log_dbg("Encoder tap #%u, next song.\n", encoder_press_count);
                break;
            case 1:
                tap_code(KC_MEDIA_PLAY_PAUSE);
                log_dbg("Encoder tap #%u, play/pause music.\n", encoder_press_count);
                break;
        }
        encoder_timer       = 0;
        encoder_press_count = 0;
        encoder_timeout_add = 0;
    }
}

// -- combined process_record_user handler --
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool pressed = record->event.pressed;

    switch (keycode) {
        // virtual WASD
        case L_W:
            update_axis(&vertical, true, pressed);
            return false;
        case L_S:
            update_axis(&vertical, false, pressed);
            return false;
        case L_D:
            update_axis(&horizontal, true, pressed);
            return false;
        case L_A:
            update_axis(&horizontal, false, pressed);
            return false;

        // switch to WASD fast debounce mode
        case DB_TOG:
            if (pressed) {
                debounce_fast_mode = !debounce_fast_mode;
                log_dbg("Debounce fast-mode: %s\n", debounce_fast_mode ? "ON" : "OFF");
            }
            return false;

        // encoder knob press
        case KC_MUTE:
            if (pressed) {
                encoder_press_count++;
                encoder_timer       = timer_read();
                encoder_timeout_add = timer_elapsed(encoder_timer);
            }
            return false;

        default:
            return true;
    }
}

#ifdef RGB_MATRIX_ENABLE

bool rgb_matrix_indicators_user(void) {
    // RGB colors: teal vs lime green
    /* lime when “fast-debounce” is ON, teal otherwise */
    uint8_t r = debounce_fast_mode ? 255  : 0;
    uint8_t g = 0;
    uint8_t b = debounce_fast_mode ? 0  : 255;

    rgb_matrix_set_color(72, r, g, b);   // Delete
    rgb_matrix_set_color(75, r, g, b);   // PgUp
    rgb_matrix_set_color(86, r, g, b);   // PgDn
    rgb_matrix_set_color(82, r, g, b);   // End

    return true;
}

#endif
