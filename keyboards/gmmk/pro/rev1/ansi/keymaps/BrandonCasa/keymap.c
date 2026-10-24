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

#include "socd.h"

bool debounce_fast_mode = false;

/* Log helper – short and sweet */
static inline void log_dbg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[128];
    vsnprintf(buf, sizeof(buf), fmt, args);
    dprintf("%s", buf);
    va_end(args);
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
        _______, RM_TOGG, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, RM_VALU, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, QK_BOOT,          _______,
        _______, _______, RM_VALD, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
        _______,          RM_HUEU, _______, _______, _______, _______, NK_TOGG, _______, _______, _______, _______,          _______, RM_NEXT, _______,
        DB_TOG,  _______, DB_TOGG,                            _______,                            _______, _______, _______, RM_SPDD, RM_PREV, RM_SPDU
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
static uint16_t encoder_timer       = 0;
static uint8_t  encoder_press_count = 0;
static bool     encoder_waiting     = false;
static uint8_t  media_key           = KC_NO;
static uint16_t media_timer         = 0;

static void start_media_tap(uint8_t keycode) {
    media_key   = keycode;
    media_timer = timer_read();
    register_code(media_key);
}

void matrix_scan_user(void) {
    // Preserve the board's consumer-key hold interval without tap_code()'s
    // blocking wait. Direction keys keep processing while the media key is down.
    if (media_key != KC_NO && timer_elapsed(media_timer) >= TAP_CODE_DELAY) {
        unregister_code(media_key);
        media_key = KC_NO;
    }
    if (encoder_waiting && timer_elapsed(encoder_timer) > 1000) {
        switch (encoder_press_count) {
            case 3:
                start_media_tap(KC_MEDIA_PREV_TRACK);
                log_dbg("Encoder tap #%u, previous song.\n", encoder_press_count);
                break;
            case 2:
                start_media_tap(KC_MEDIA_NEXT_TRACK);
                log_dbg("Encoder tap #%u, next song.\n", encoder_press_count);
                break;
            case 1:
                start_media_tap(KC_MEDIA_PLAY_PAUSE);
                log_dbg("Encoder tap #%u, play/pause music.\n", encoder_press_count);
                break;
        }
        encoder_timer       = 0;
        encoder_press_count = 0;
        encoder_waiting     = false;
    }
}

// -- combined process_record_user handler --
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool pressed = record->event.pressed;

    if (!process_socd(keycode, record)) {
        return false;
    }

    // QMK's magic handler clears the report (including when toggling NKRO).
    // Clear ownership too, so a later release cannot revive an old direction.
    if (pressed && IS_QK_MAGIC(keycode)) {
        reset_socd();
    }

    switch (keycode) {
        // switch selective WASD + Space debounce policy (SOCD is always enabled)
        case DB_TOG:
            if (pressed) {
                debounce_fast_mode = !debounce_fast_mode;
                log_dbg("Debounce fast-mode: %s\n", debounce_fast_mode ? "ON" : "OFF");
            }
            return false;

        // encoder knob press
        case KC_MUTE:
            if (pressed) {
                if (encoder_press_count < 4) {
                    encoder_press_count++;
                }
                encoder_timer       = timer_read();
                encoder_waiting     = true;
            }
            return false;

        default:
            return true;
    }
}

#ifdef RGB_MATRIX_ENABLE

bool rgb_matrix_indicators_user(void) {
    // Red when fast debounce is selected, blue otherwise.
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

// Drop SOCD ownership across suspend. Held directions must be released and
// pressed again after wake; releasing a stale loser cannot reactivate anything.
void suspend_power_down_user(void) {
    reset_socd();
    send_keyboard_report();
    if (media_key != KC_NO) {
        unregister_code(media_key);
        media_key = KC_NO;
    }
    encoder_timer       = 0;
    encoder_press_count = 0;
    encoder_waiting     = false;
}

void suspend_wakeup_init_user(void) {
    // ChibiOS calls this from an ISR: memory changes only, no USB or logging.
    reset_socd();
}
