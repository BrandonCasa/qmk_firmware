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

bool vertical_register = false;
bool horizontal_register = false;

uint16_t horizontal_to_unregister = 0;
uint16_t vertical_to_unregister = 0;

uint16_t current_horizontal_key = 0;
uint16_t current_vertical_key = 0;

void update_key_state(uint16_t keycode, bool pressed) {
    switch (keycode) {
        case L_W: // virtual 'W' key
            if (pressed && !s_pressed) {
                // 'W' key down    <- event
                // 'S' key up
                // Real input to 'W' from 'none'
                current_vertical_key = KC_W;
                vertical_register = true;
                vertical_to_unregister = 0;
            }
            else if (pressed && s_pressed) {
                // 'W' key down    <- event
                // 'S' key down
                // Real input to 'W' from 'S'
                current_vertical_key = KC_W;
                vertical_register = true;
                vertical_to_unregister = KC_S;
            }
            else if (!pressed && !s_pressed) {
                // 'W' key up    <- event
                // 'S' key up
                // Real input to 'none' from 'W'
                current_vertical_key = 0;
                vertical_register = false;
                vertical_to_unregister = KC_W;
            }
            else if (!pressed && s_pressed && current_vertical_key == KC_W) {
                // 'W' key up    <- event
                // 'S' key down
                // Real input to 'S' from 'W'
                current_vertical_key = KC_S;
                vertical_register = true;
                vertical_to_unregister = KC_W;
            }
            else if (!pressed && s_pressed && current_vertical_key == KC_S) {
                // 'W' key up    <- event
                // 'S' key down
                // Real input to 'S' from 'S'
                current_vertical_key = KC_S;
                vertical_register = false;
                vertical_to_unregister = 0;
            }
            if (w_pressed == pressed) vertical_register = false;
            w_pressed = pressed;
            break;
        case L_A: // virtual 'A' key
            if (pressed && !d_pressed) {
                // 'A' key down    <- event
                // 'D' key up
                // Real input to 'A' from 'none'
                current_horizontal_key = KC_A;
                horizontal_register = true;
                horizontal_to_unregister = 0;
            }
            else if (pressed && d_pressed) {
                // 'A' key down    <- event
                // 'D' key down
                // Real input to 'A' from 'D'
                current_horizontal_key = KC_A;
                horizontal_register = true;
                horizontal_to_unregister = KC_D;
            }
            else if (!pressed && !d_pressed) {
                // 'A' key up    <- event
                // 'D' key up
                // Real input to 'none' from 'A'
                current_horizontal_key = 0;
                horizontal_register = false;
                horizontal_to_unregister = KC_A;
            }
            else if (!pressed && d_pressed && current_horizontal_key == KC_A) {
                // 'A' key up    <- event
                // 'D' key down
                // Real input to 'D' from 'A'
                current_horizontal_key = KC_D;
                horizontal_register = true;
                horizontal_to_unregister = KC_A;
            }
            else if (!pressed && d_pressed && current_horizontal_key == KC_D) {
                // 'A' key up    <- event
                // 'D' key down
                // Real input to 'D' from 'D'
                current_horizontal_key = KC_D;
                horizontal_register = false;
                horizontal_to_unregister = 0;
            }
            if (a_pressed == pressed) horizontal_register = false;
            a_pressed = pressed;
            break;
        case L_S: // virtual 'S' key
            if (pressed && !w_pressed) {
                // 'W' key up
                // 'S' key down    <- event
                // Real input to 'S' from 'none'
                current_vertical_key = KC_S;
                vertical_register = true;
                vertical_to_unregister = 0;
            }
            else if (pressed && w_pressed) {
                // 'W' key down
                // 'S' key down    <- event
                // Real input to 'S' from 'W'
                current_vertical_key = KC_S;
                vertical_register = true;
                vertical_to_unregister = KC_W;
            }
            else if (!pressed && !w_pressed) {
                // 'W' key up
                // 'S' key up    <- event
                // Real input to 'none' from 'S'
                current_vertical_key = 0;
                vertical_register = false;
                vertical_to_unregister = KC_S;
            }
            else if (!pressed && w_pressed && current_vertical_key == KC_S) {
                // 'W' key down
                // 'S' key up    <- event
                // Real input to 'W' from 'S'
                current_vertical_key = KC_W;
                vertical_register = true;
                vertical_to_unregister = KC_S;
            }
            else if (!pressed && w_pressed && current_vertical_key == KC_W) {
                // 'W' key down
                // 'S' key up    <- event
                // Real input to 'W' from 'W'
                current_vertical_key = KC_W;
                vertical_register = false;
                vertical_to_unregister = 0;
            }
            if (s_pressed == pressed) vertical_register = false;
            s_pressed = pressed;
            break;
        case L_D: // virtual 'D' key
            if (pressed && !a_pressed) {
                // 'A' key up
                // 'D' key down    <- event
                // Real input to 'D' from 'none'
                current_horizontal_key = KC_D;
                horizontal_register = true;
                horizontal_to_unregister = 0;
            }
            else if (pressed && a_pressed) {
                // 'A' key down
                // 'D' key down    <- event
                // Real input to 'D' from 'A'
                current_horizontal_key = KC_D;
                horizontal_register = true;
                horizontal_to_unregister = KC_A;
            }
            else if (!pressed && !a_pressed) {
                // 'A' key up
                // 'D' key up    <- event
                // Real input to 'none' from 'D'
                current_horizontal_key = 0;
                horizontal_register = false;
                horizontal_to_unregister = KC_D;
            }
            else if (!pressed && a_pressed && current_horizontal_key == KC_D) {
                // 'A' key down
                // 'D' key up    <- event
                // Real input to 'A' from 'D'
                current_horizontal_key = KC_A;
                horizontal_register = true;
                horizontal_to_unregister = KC_D;
            }
            else if (!pressed && a_pressed && current_horizontal_key == KC_A) {
                // 'A' key down
                // 'D' key up    <- event
                // Real input to 'A' from 'A'
                current_horizontal_key = KC_A;
                horizontal_register = false;
                horizontal_to_unregister = 0;
            }
            if (d_pressed == pressed) horizontal_register = false;
            d_pressed = pressed;
            break;
    }

    // Vertical Update
    if (vertical_to_unregister != 0) {
        unregister_code(vertical_to_unregister);
        vertical_to_unregister = 0;
    }
    if (vertical_register) {
        register_code(current_vertical_key);
    }
    vertical_register = false;

    // Horizontal Update
    if (horizontal_to_unregister != 0) {
        unregister_code(horizontal_to_unregister);
        horizontal_to_unregister = 0;
    }
    if (horizontal_register) {
        register_code(current_horizontal_key);
    }
    horizontal_register = false;
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
