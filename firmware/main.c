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

#define F_CPU 20000000L

#include <avr/io.h>
#include <util/delay.h>

#include "digits.h"
#include "write_pixels.h"

int main(void) {
	DDRA = 0b00000011;

	uint8_t buf[63];
	for (int i = 0; i < sizeof(buf); i++) {
		buf[i] = 0;
	}

	uint8_t digit = 0;
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	while (1) {
		PORTA = 0b00000001;
		_delay_ms(250);
		set_digit(buf, digit, r, g, b);
		write_pixels(buf, sizeof(buf));
		PORTA = 0b00000000;
		_delay_ms(250);

		digit = (digit + 1) % 10;
		r += 25;
		g += 80;
		b += 100;
	}

	return 0;
}
