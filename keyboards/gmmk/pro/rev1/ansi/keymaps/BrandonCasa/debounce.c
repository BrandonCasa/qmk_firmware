// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright 2017 Alex Ong
// Copyright 2021 Simon Arlott
// Selective per-key eager/deferred debounce, based on QMK sym_eager_pk.c.
#include "debounce.h"
#include "timer.h"
#include "fast_debounce.h"
#include <string.h>

extern bool debounce_fast_mode;

static uint8_t counters[MATRIX_ROWS_PER_HAND][MATRIX_COLS];
static matrix_row_t previous_raw[MATRIX_ROWS_PER_HAND];
static matrix_row_t eager[MATRIX_ROWS_PER_HAND];
static matrix_row_t cooldown[MATRIX_ROWS_PER_HAND];
static fast_timer_t last_time;
static bool timers_active;

void debounce_init(void) {
    memset(counters, 0, sizeof(counters));
    memset(previous_raw, 0, sizeof(previous_raw));
    memset(eager, 0, sizeof(eager));
    memset(cooldown, 0, sizeof(cooldown));
    timers_active = false;
    // No timer baseline is needed until a counter is armed.
}

bool debounce(matrix_row_t raw[], matrix_row_t cooked[], bool changed) {
    if (!changed && !timers_active) {
        return false;
    }
    fast_timer_t now = timer_read_fast();
    fast_timer_t dt  = timers_active ? TIMER_DIFF_FAST(now, last_time) : 0;
    if (!changed && dt == 0) {
        return false;
    }
    uint8_t elapsed     = dt > UINT8_MAX ? UINT8_MAX : dt;
    last_time           = now; // Reconsider pending raw differences on timer expiry.
    timers_active       = false;
    bool cooked_changed = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; ++row) {
        matrix_row_t edges = raw[row] ^ previous_raw[row];
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            matrix_row_t mask = MATRIX_ROW_SHIFTER << col;
            uint8_t *counter  = &counters[row][col];
            bool down         = (raw[row] & mask) != 0;

            // Latch policy only when previously idle and fully released.
            // Pending debounce and held keys finish under their original policy.
            if (!*counter && !(previous_raw[row] & mask) && !(cooked[row] & mask)) {
                if (debounce_fast_mode && is_fast_debounce_position(row, col)) {
                    eager[row] |= mask;
                } else {
                    eager[row] &= ~mask;
                }
            }

            bool is_eager    = (eager[row] & mask) != 0;
            bool is_cooldown = (cooldown[row] & mask) != 0;
            if (*counter) {
                // A deferred candidate needs a full stable interval after every
                // raw edge, including an edge on the expiry scan.
                if (!is_cooldown && (edges & mask)) {
                    *counter = is_eager ? FAST_RELEASE_DEBOUNCE : DEBOUNCE;
                } else {
                    *counter = *counter > elapsed ? *counter - elapsed : 0;
                }
                if (!*counter) {
                    cooldown[row] &= ~mask;
                    if (!is_cooldown && ((raw[row] ^ cooked[row]) & mask)) {
                        cooked[row] ^= mask;
                        cooked_changed = true;
                    }
                }
            }

            if (is_cooldown && *counter) {
                // Never cancel eager lockout merely because raw == cooked.
                timers_active = true;
                continue;
            }

            if ((raw[row] ^ cooked[row]) & mask) {
                if (!*counter) {
                    if (is_eager && (down || !FAST_DEBOUNCE_DEFER_RELEASE)) {
                        cooked[row] ^= mask;
                        cooked_changed = true;
                        *counter = down ? FAST_PRESS_DEBOUNCE : FAST_RELEASE_DEBOUNCE;
                        cooldown[row] |= mask;
                    } else {
                        *counter = is_eager ? FAST_RELEASE_DEBOUNCE : DEBOUNCE;
                    }
                }
            } else if (!(cooldown[row] & mask)) {
                *counter = 0; // A deferred candidate returned to cooked state.
            }
            timers_active |= *counter != 0;
        }
        previous_raw[row] = raw[row];
    }
    return cooked_changed;
}
