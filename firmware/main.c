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

#include "debounce.h"
#include "digits.h"
#include "write_pixels.h"

int main(void) {
	DDRB = 0b00000011;

	uint8_t buf[384];
	for (int i = 0; i < sizeof(buf); i++) {
		buf[i] = 0;
	}

    ADMUX = (1<<REFS0);
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

	uint16_t work = 0;
	uint16_t rest = 0;
	while (1) {
		ADMUX = ADMUX & ~0b11111;
		ADCSRA |= (1 <<ADSC);
		while (ADCSRA & (1 << ADSC));

		uint16_t new_work = debounce_minute(work, ADC);
		if (new_work != work) {
			work = new_work;
			set_digit(DIGIT_ADDRESS(buf, 0), work / 10, 0, 255, 0);
			set_digit(DIGIT_ADDRESS(buf, 1), work % 10, 0, 255, 0);
			write_pixels(buf, sizeof(buf));
		}

		ADMUX = (ADMUX & ~0b11111) | 1;
		ADCSRA |= (1 <<ADSC);
		while (ADCSRA & (1 << ADSC));

		uint16_t new_rest = debounce_minute(rest, ADC);
		if (new_rest != rest) {
			rest = new_rest;
			set_digit(DIGIT_ADDRESS(buf, 0), rest / 10, 255, 0, 0);
			set_digit(DIGIT_ADDRESS(buf, 1), rest % 10, 255, 0, 0);
			write_pixels(buf, sizeof(buf));
		}
	}

	return 0;
}
