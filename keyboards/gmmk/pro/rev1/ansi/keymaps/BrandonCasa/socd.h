// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "quantum.h"

// Keep values stable: VIA may already store these custom keycodes.
enum custom_keycodes { L_W = SAFE_RANGE, L_S, L_A, L_D, DB_TOG };

bool process_socd(uint16_t keycode, keyrecord_t *record);
// Memory only: safe in the ChibiOS wakeup ISR. The caller sends if appropriate.
void reset_socd(void);
