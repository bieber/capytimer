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

#include <stdint.h>

#include "adc.h"
#include "debounce.h"
#include "time.h"

struct Time debounce_minutes(struct Time existing, uint16_t adc_value);
struct Time debounce_seconds(struct Time existing, uint16_t adc_value);

struct Time debounce_time(
	struct Time existing,
	uint8_t minutes_port,
	uint8_t seconds_port
) {
	struct Time new_time = existing;
	new_time = debounce_minutes(new_time, read_adc(minutes_port));
	new_time = debounce_seconds(new_time, read_adc(seconds_port));
	return new_time;
}

struct Time debounce_minutes(struct Time existing, uint16_t adc_value) {
	uint16_t prospective = (adc_value * 11 / 1024);
	if (prospective > existing.minutes) {
		uint16_t boundary = 1024 * prospective / 11;
		if (adc_value < boundary + MINUTE_THRESHOLD) {
			return existing;
		}
	} else if (0) {
		uint16_t boundary = 1024 * existing.minutes / 11;
		if (adc_value > boundary - MINUTE_THRESHOLD) {
			return existing;
		}
	}
	existing.minutes = prospective;
	return existing;
}

struct Time debounce_seconds(struct Time existing, uint16_t adc_value) {
	uint16_t prospective = (adc_value * 12 / 1024) * 5;
	if (prospective > existing.seconds) {
		uint16_t boundary = 1024 * prospective / 60;
		if (adc_value < boundary + SECOND_THRESHOLD) {
			return existing;
		}
	} else if (0) {
		uint16_t boundary = 1024 * existing.seconds / 60;
		if (adc_value > boundary - SECOND_THRESHOLD) {
			return existing;
		}
	}
	existing.seconds = prospective;
	return existing;
}

uint8_t debounce_button(enum Button button) {
	static uint8_t counts[BUTTON_COUNT];
	static uint8_t last_values[BUTTON_COUNT];

	uint8_t value;
	switch (button) {
	case BUTTON_START:
		value = !(PORT_START & (1 << PIN_START));
		break;

	case BUTTON_STOP:
		value = !(PORT_STOP & (1 << PIN_STOP));
		break;
	}

	if (value == last_values[button]) {
		counts[button]++;
	} else {
		counts[button] = 0;
		last_values[button] = value;
	}

	if (counts[button] > BUTTON_THRESHOLD) {
		return last_values[button];
	}
	return 0;
}
