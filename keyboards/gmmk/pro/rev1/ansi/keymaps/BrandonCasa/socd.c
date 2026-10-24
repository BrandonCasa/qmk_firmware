// SPDX-License-Identifier: GPL-2.0-or-later
#include "socd.h"
#include <string.h>

typedef struct {
    uint8_t held[2];
    uint8_t output;
} axis_t;

static axis_t axes[2];
// Remember press ownership even if VIA edits that position while it is down.
static uint8_t held_direction[MATRIX_ROWS][MATRIX_COLS];
static const uint8_t codes[2][2] = {{KC_W, KC_S}, {KC_A, KC_D}};

static void update_axis(uint8_t direction, bool pressed) {
    uint8_t axis_index = direction / 2;
    uint8_t side       = direction % 2;
    axis_t *axis       = &axes[axis_index];
    uint8_t desired;

    if (pressed) {
        ++axis->held[side];
        desired = codes[axis_index][side]; // Most recent press wins.
    } else {
        --axis->held[side];
        desired = axis->held[side] ? axis->output : axis->held[!side] ? codes[axis_index][!side] : KC_NO;
    }

    if (desired == axis->output) {
        return; // In particular, releasing the losing direction sends nothing.
    }
    if (axis->output != KC_NO) {
        del_key(axis->output);
    }
    if (desired != KC_NO) {
        add_key(desired);
    }
    axis->output = desired;
    // Mutate first, then send once: no intermediate neutral report on switches.
    send_keyboard_report();
}

bool process_socd(uint16_t keycode, keyrecord_t *record) {
    if (!IS_KEYEVENT(record->event)) {
        return true;
    }
    keypos_t pos = record->event.key;
    if (pos.row >= MATRIX_ROWS || pos.col >= MATRIX_COLS) {
        return true;
    }
    uint8_t *held = &held_direction[pos.row][pos.col];
    if (!record->event.pressed && *held) {
        uint8_t direction = *held - 1;
        *held = 0;
        update_axis(direction, false);
        return false;
    }
    if (keycode < L_W || keycode > L_D) {
        return true;
    }
    if (record->event.pressed && !*held) {
        *held = keycode - L_W + 1;
        update_axis(keycode - L_W, true);
    }
    return false; // Ignore duplicate presses and unmatched releases after wake.
}

void reset_socd(void) {
    for (uint8_t i = 0; i < 2; ++i) {
        if (axes[i].output != KC_NO) {
            del_key(axes[i].output);
        }
    }
    memset(axes, 0, sizeof(axes));
    memset(held_direction, 0, sizeof(held_direction));
}
