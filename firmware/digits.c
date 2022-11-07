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

#include "digits.h"

void set_digit(uint8_t *buf, uint8_t value, uint8_t r, uint8_t g, uint8_t b) {
	static uint8_t lut[10] = {
		0b11111100,
		0b01100000,
		0b11011010,
		0b11110010,
		0b01100110,
		0b10110110,
		0b10111110,
		0b11100000,
		0b11111110,
		0b11110110,
	};

	uint8_t key = lut[value];
	for (uint8_t i = 0; i < 7; i++) {
		for (uint8_t j = 0; j < 9; j++) {
			switch (j % 3) {
			case 0:
				buf[9*i + j] = key & (1 << 7) ? g : 0;
				break;
			case 1:
				buf[9*i + j] = key & (1 << 7) ? r : 0;
				break;
			case 2:
				buf[9*i + j] = key & (1 << 7) ? b : 0;
				break;
			}
		}

		key <<= 1;
	}
}

void set_divider(uint8_t *buf, uint8_t r, uint8_t g, uint8_t b) {
	for (uint8_t i = 0; i < 2; i++) {
		buf[3*i] = g;
		buf[3*i + 1] = r;
		buf[3*i + 2] = b;
	}
}

void set_time(uint8_t *buf, struct Time time, uint8_t r, uint8_t g, uint8_t b) {
	uint8_t tens_minute = time.minutes / 10;
	if (tens_minute > 0) {
		set_digit(DIGIT_ADDRESS(buf, 0), tens_minute, r, g, b);
	} else {
		set_digit(DIGIT_ADDRESS(buf, 0), 0, 0, 0, 0);
	}
	set_digit(DIGIT_ADDRESS(buf, 1), time.minutes % 10, r, g, b);
	set_digit(DIGIT_ADDRESS(buf, 2), time.seconds / 10, r, g, b);
	set_digit(DIGIT_ADDRESS(buf, 3), time.seconds % 10, r, g, b);
	set_divider(DIVIDER_ADDRESS(buf), r, g, b);
}

void set_number(uint8_t *buf, uint16_t x, uint8_t r, uint8_t g, uint8_t b) {
	set_divider(DIVIDER_ADDRESS(buf), 0, 0, 0);
	set_digit(DIGIT_ADDRESS(buf, 3), x % 10, r, g, b);
	x /= 10;
	set_digit(DIGIT_ADDRESS(buf, 2), x % 10, r, g, b);
	x /= 10;
	set_digit(DIGIT_ADDRESS(buf, 1), x % 10, r, g, b);
	x /= 10;
	set_digit(DIGIT_ADDRESS(buf, 0), x % 10, r, g, b);
}

void set_round(uint8_t *buf, uint8_t round, uint8_t r, uint8_t g, uint8_t b) {
	set_digit(DIGIT_ADDRESS(buf, 4), round / 10, r, g, b);
	set_digit(DIGIT_ADDRESS(buf, 5), round % 10, r, g, b);
}
