/*
 * Copyright (C) 2022, Robert Bieber
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include "pins.h"
#include "time.h"

#define BUTTON_THRESHOLD 20
#define SECOND_THRESHOLD 15

enum Button {
	BUTTON_START,
	BUTTON_STOP,

	BUTTON_COUNT
};

struct Time debounce_seconds(struct Time existing, uint16_t adc_value);

uint8_t debounce_button(enum Button button);

#endif
