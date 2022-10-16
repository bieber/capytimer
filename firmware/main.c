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

#include "write_pixels.h"

int main(void) {
	DDRA = 0b00000011;

	uint8_t buf[24];
	for (int i = 0; i < sizeof(buf); i++) {
		buf[i] = 0;
	}

	while (1) {
		PORTA = 0b00000001;
		_delay_ms(100);
		for (int i = 0; i < sizeof(buf); i++) {
			buf[i] += i * 5;
			if (buf[i] == 255) {
				buf[i] = 0;
			}
		}
		write_pixels(buf, sizeof(buf));
		PORTA = 0b00000000;
		_delay_ms(100);
	}

	return 0;
}
