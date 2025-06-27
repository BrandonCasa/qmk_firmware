/*  File: custom.c  (lives in your keymap folder)
 *  Custom symmetric per-key debounce with runtime eager toggle
 *
 *  - Default: All keys use sym_defer_pk (delay & validate)
 *  - Toggle mode: WASD use eager flip + ignore
 *
 *  Works with `DEBOUNCE_TYPE = custom`
 *  No dynamic allocation → compatible with all ChibiOS heap configs.
 */

#define TAP_CODE_DELAY 10
#define FAST_TYPE_ENABLE

#include "debounce.h"
#include "timer.h"
#include <stdbool.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/*              Tweak these four lines for your board layout          */
/* ------------------------------------------------------------------ */
#define ROW_W 2
#define COL_W 2

#define ROW_A 3
#define COL_A 1

#define ROW_S 3
#define COL_S 2

#define ROW_D 3
#define COL_D 3

#define ROW_SPC 5
#define COL_SPC 3
/* ------------------------------------------------------------------ */

#ifndef DEBOUNCE
#    define DEBOUNCE 5       /* ms, 1–255 */
#endif

#if DEBOUNCE > UINT8_MAX
#    undef  DEBOUNCE
#    define DEBOUNCE UINT8_MAX
#endif

#define ROW_SHIFTER ((matrix_row_t)1)
typedef uint8_t debounce_counter_t;
#define DEBOUNCE_ELAPSED 0

/* External toggle flag from keymap.c */
extern volatile bool debounce_fast_mode;

/* static → no malloc needed */
static debounce_counter_t debounce_counters[MATRIX_ROWS * MATRIX_COLS];
static fast_timer_t       last_time;
static bool               counters_need_update;
static bool               cooked_changed;

/* prototypes required by QMK */
void debounce_init(uint8_t num_rows);
bool debounce(matrix_row_t raw[], matrix_row_t cooked[],
              uint8_t num_rows, bool changed);

/* internal helpers */
static void update_counters_and_transfer_if_expired(
    matrix_row_t raw[], matrix_row_t cooked[],
    uint8_t num_rows, uint8_t elapsed);
static void arm_counters_for_changes(
    matrix_row_t raw[], matrix_row_t cooked[], uint8_t num_rows);

static inline bool is_eager_key(uint8_t row, uint8_t col) {
    if (!debounce_fast_mode) return false;
    return (row == ROW_W && col == COL_W) ||
           (row == ROW_A && col == COL_A) ||
           (row == ROW_S && col == COL_S) ||
           (row == ROW_D && col == COL_D) ||
           (row == ROW_SPC && col == COL_SPC);
}

/* ------------------------------------------------------------------ */
void debounce_init(uint8_t num_rows) {
    (void)num_rows;
    for (uint16_t i = 0; i < MATRIX_ROWS * MATRIX_COLS; ++i)
        debounce_counters[i] = DEBOUNCE_ELAPSED;

    counters_need_update = false;
    cooked_changed       = false;
    last_time            = timer_read_fast();
}

bool debounce(matrix_row_t raw[], matrix_row_t cooked[],
              uint8_t num_rows, bool changed) {
    bool updated_last = false;
    cooked_changed    = false;

    if (counters_need_update) {
        fast_timer_t now          = timer_read_fast();
        fast_timer_t elapsed_fast = TIMER_DIFF_FAST(now, last_time);
        uint8_t      elapsed      = (elapsed_fast > UINT8_MAX)
                                        ? UINT8_MAX
                                        : (uint8_t)elapsed_fast;

        last_time     = now;
        updated_last  = true;

        if (elapsed)
            update_counters_and_transfer_if_expired(
                raw, cooked, num_rows, elapsed);
    }

    if (changed) {
        if (!updated_last)
            last_time = timer_read_fast();
        arm_counters_for_changes(raw, cooked, num_rows);
    }

    return cooked_changed;
}

/* ------------------------------------------------------------------ */
static void update_counters_and_transfer_if_expired(
    matrix_row_t raw[], matrix_row_t cooked[],
    uint8_t num_rows, uint8_t elapsed) {

    counters_need_update = false;
    debounce_counter_t *ptr = debounce_counters;

    for (uint8_t row = 0; row < num_rows; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col, ++ptr) {
            if (*ptr == DEBOUNCE_ELAPSED) continue;

            if (*ptr <= elapsed) {
                *ptr = DEBOUNCE_ELAPSED;

                if (!is_eager_key(row, col)) {  // defer-mode only
                    matrix_row_t mask = ROW_SHIFTER << col;
                    matrix_row_t next = (cooked[row] & ~mask) |
                                        (raw[row]    &  mask);
                    cooked_changed   |= (cooked[row] ^ next);
                    cooked[row]       = next;
                }
            } else {
                *ptr                 -= elapsed;
                counters_need_update = true;
            }
        }
    }
}

static void arm_counters_for_changes(
    matrix_row_t raw[], matrix_row_t cooked[], uint8_t num_rows) {

    debounce_counter_t *ptr = debounce_counters;

    for (uint8_t row = 0; row < num_rows; ++row) {
        matrix_row_t delta = raw[row] ^ cooked[row];

        for (uint8_t col = 0; col < MATRIX_COLS; ++col, ++ptr) {
            if (!(delta & (ROW_SHIFTER << col))) {
                *ptr = DEBOUNCE_ELAPSED;  // stable → reset timer
                continue;
            }

            if (is_eager_key(row, col)) {
                cooked[row]         ^= (ROW_SHIFTER << col); // eager flip
                cooked_changed       = true;
                *ptr                 = DEBOUNCE;              // ignore chatter
                counters_need_update = true;
            } else {
                if (*ptr == DEBOUNCE_ELAPSED) {
                    *ptr                 = DEBOUNCE;
                    counters_need_update = true;
                }
            }
        }
    }
}
