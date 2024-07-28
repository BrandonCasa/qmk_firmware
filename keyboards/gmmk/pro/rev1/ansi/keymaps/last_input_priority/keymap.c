/* Copyright 2021 Glorious, LLC <salman@pcgamingrace.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

enum custom_keycodes {
    L_W = SAFE_RANGE,
    L_A,
    L_S,
    L_D
};

bool w_pressed = false;
bool a_pressed = false;
bool s_pressed = false;
bool d_pressed = false;

uint16_t last_horizontal_key = 0;
uint16_t last_vertical_key = 0;

void update_key_state(uint16_t keycode, bool pressed) {
    switch (keycode) {
        case L_W:
            w_pressed = pressed;
            if (pressed) last_vertical_key = L_W;
            break;
        case L_A:
            a_pressed = pressed;
            if (pressed) last_horizontal_key = L_A;
            break;
        case L_S:
            s_pressed = pressed;
            if (pressed) last_vertical_key = L_S;
            break;
        case L_D:
            d_pressed = pressed;
            if (pressed) last_horizontal_key = L_D;
            break;
    }

    if (w_pressed && !s_pressed) {
        register_code(KC_W);
        unregister_code(KC_S);
    } else if (!w_pressed && s_pressed) {
        register_code(KC_S);
        unregister_code(KC_W);
    } else if (!w_pressed && !s_pressed) {
        unregister_code(KC_W);
        unregister_code(KC_S);
    }

    if (a_pressed && !d_pressed) {
        register_code(KC_A);
        unregister_code(KC_D);
    } else if (!a_pressed && d_pressed) {
        register_code(KC_D);
        unregister_code(KC_A);
    } else if (!a_pressed && !d_pressed) {
        unregister_code(KC_A);
        unregister_code(KC_D);
    }

    if (!pressed) {
        if (keycode == last_horizontal_key) {
            if (a_pressed && last_horizontal_key == L_A) register_code(KC_A);
            if (d_pressed && last_horizontal_key == L_D) register_code(KC_D);
        }
        if (keycode == last_vertical_key) {
            if (w_pressed && last_vertical_key == L_W) register_code(KC_W);
            if (s_pressed && last_vertical_key == L_S) register_code(KC_S);
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == L_W || keycode == L_A || keycode == L_S || keycode == L_D) {
        update_key_state(keycode, record->event.pressed);
        return false;
    }
    return true;
}


// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

//      ESC      F1       F2       F3       F4       F5       F6       F7       F8       F9       F10      F11      F12	     Prt           Rotary(Mute)
//      ~        1        2        3        4        5        6        7        8        9        0         -       (=)	     BackSpc           Del
//      Tab      Q        W        E        R        T        Y        U        I        O        P        [        ]        \                 PgUp
//      Caps     A        S        D        F        G        H        J        K        L        ;        "                 Enter             PgDn
//      Sh_L              Z        X        C        V        B        N        M        ,        .        ?                 Sh_R     Up       End
//      Ct_L     Win_L    Alt_L                               SPACE                               Alt_R    FN       Ct_R     Left     Down     Right


    // The FN key by default maps to a momentary toggle to layer 1 to provide access to the QK_BOOT key (to put the board into bootloader mode). Without
    // this mapping, you have to open the case to hit the button on the bottom of the PCB (near the USB cable attachment) while plugging in the USB
    // cable to get the board into bootloader mode - definitely not fun when you're working on your QMK builds. Remove this and put it back to KC_RGUI
    // if that's your preference.
    //
    // To put the keyboard in bootloader mode, use FN+backslash. If you accidentally put it into bootloader, you can just unplug the USB cable and
    // it'll be back to normal when you plug it back in.
    //
    // This keyboard defaults to 6KRO instead of NKRO for compatibility reasons (some KVMs and BIOSes are incompatible with NKRO).
    // Since this is, among other things, a "gaming" keyboard, a key combination to enable NKRO on the fly is provided for convenience.
    // Press Fn+N to toggle between 6KRO and NKRO. This setting is persisted to the EEPROM and thus persists between restarts.
    [0] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_PSCR,          KC_MUTE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,          KC_DEL,
        KC_TAB,  KC_Q,    L_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_PGUP,
        KC_CAPS, L_A,    L_S,    L_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,           KC_PGDN,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, KC_UP,   KC_END,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, MO(1),   KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [1] = LAYOUT(
        _______, KC_MYCM, KC_WHOM, KC_CALC, KC_MSEL, KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP, KC_MUTE, KC_VOLD, KC_VOLU, _______, _______,          _______,
        _______, RGB_TOG, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, RGB_VAI, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, QK_BOOT,          _______,
        _______, _______, RGB_VAD, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          _______,
        _______,          _______, RGB_HUI, _______, _______, _______, NK_TOGG, _______, _______, _______, _______,          _______, RGB_MOD, _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, RGB_SPD, RGB_RMOD, RGB_SPI
    ),


};
// clang-format on

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [1] = { ENCODER_CCW_CW(KC_TRNS, KC_TRNS) }
};
#endif

void matrix_scan_user(void) {
    // Add any periodic tasks here if needed
}
