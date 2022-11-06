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

#include <avr/interrupt.h>

#include "setup.h"

void setup(uint8_t pixels[SCREEN_SIZE]) {
	for (int i = 0; i < SCREEN_SIZE; i++) {
		pixels[i] = 0;
	}

	// Clear out all the DDR and PORT registers
	DDR_SCREEN = 0;
	DDR_BUZZ = 0;
	PULLUP_START = 0;
	PULLUP_STOP = 0;

	// Set outputs and pullups on the input ports
	DDR_SCREEN |= (1 << PIN_SCREEN);
	DDR_BUZZ |= (1 << PIN_BUZZ);
	PULLUP_START |= (1 << PIN_START);
	PULLUP_STOP |= (1 << PIN_STOP);

	// Use AVCC for ADC reference
    ADMUX = (1<<REFS0);
	// Enable ADC and set f/128 prescaler for accuracy
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

	// Timer 1 operates in CTC mode counting off seconds and sending
	// interrupts
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	OCR1A = 19531;

	// Turn on interrupts
	sei();
}
