// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "matrix.h"

// Electrical [row, column], NOT the six physical rows in LAYOUT().
// Source: rev1/ansi/keyboard.json -> layouts.LAYOUT.layout[].matrix.
#define FAST_W_ROW 2
#define FAST_W_COL 0
#define FAST_A_ROW 1
#define FAST_A_COL 2
#define FAST_S_ROW 2
#define FAST_S_COL 2
#define FAST_D_ROW 3
#define FAST_D_COL 2
#define FAST_SPACE_ROW 9
#define FAST_SPACE_COL 4

#ifndef DEBOUNCE
#    define DEBOUNCE 5
#endif
// Provisional defaults, not switch measurements. Tune these independently.
#ifndef FAST_PRESS_DEBOUNCE
#    define FAST_PRESS_DEBOUNCE DEBOUNCE
#endif
#ifndef FAST_RELEASE_DEBOUNCE
#    define FAST_RELEASE_DEBOUNCE DEBOUNCE
#endif
// 0: symmetric eager; 1: eager press, stable/deferred release (for comparison).
#ifndef FAST_DEBOUNCE_DEFER_RELEASE
#    define FAST_DEBOUNCE_DEFER_RELEASE 0
#endif

#if DEBOUNCE < 1 || DEBOUNCE > 255 || FAST_PRESS_DEBOUNCE < 1 || FAST_PRESS_DEBOUNCE > 255 || FAST_RELEASE_DEBOUNCE < 1 || FAST_RELEASE_DEBOUNCE > 255
#    error "Debounce intervals must be between 1 and 255 milliseconds"
#endif
#if FAST_DEBOUNCE_DEFER_RELEASE != 0 && FAST_DEBOUNCE_DEFER_RELEASE != 1
#    error "FAST_DEBOUNCE_DEFER_RELEASE must be 0 or 1"
#endif

static inline bool is_fast_debounce_position(uint8_t row, uint8_t col) {
    return (row == FAST_W_ROW && col == FAST_W_COL) || (row == FAST_A_ROW && col == FAST_A_COL) || (row == FAST_S_ROW && col == FAST_S_COL) || (row == FAST_D_ROW && col == FAST_D_COL) || (row == FAST_SPACE_ROW && col == FAST_SPACE_COL);
}
