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

#define BUTTON_THRESHOLD 6
#define MINUTE_THRESHOLD 20
#define SECOND_THRESHOLD 20
#define BRIGHTNESS_THRESHOLD 50

enum Button {
	BUTTON_START,
	BUTTON_STOP,
	RF_START,
	RF_STOP,

	BUTTON_COUNT
};

enum RFButton {
	RF_BUTTON_START,
	RF_BUTTON_STOP,

	RF_BUTTON_COUNT,
};

struct Time debounce_time(
	struct Time existing,
	uint8_t minutes_port,
	uint8_t seconds_port
);

uint8_t debounce_button(enum Button button);

uint8_t debounce_brightness(uint8_t existing, uint8_t port);

#endif
