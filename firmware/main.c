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

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "debounce.h"
#include "digits.h"
#include "time.h"
#include "write_pixels.h"

enum State {STARTUP, WORK, REST, PAUSE};

volatile struct Time work_time = {0, 0};
volatile struct Time rest_time = {0, 0};

volatile struct Time running_time = {0, 0};

volatile enum State state = STARTUP;
volatile enum State paused_state;

int main(void) {
	sei();
	DDRB = 0b00010011;
	PORTB = 0b00001100;

	uint8_t buf[384];
	for (int i = 0; i < sizeof(buf); i++) {
		buf[i] = 0;
	}

	// Use AVCC for ADC reference
    ADMUX = (1<<REFS0);
	// Enable ADC and set f/128 prescaler for accuracy
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

	// Timer 1 operates in CTC mode counting off seconds and sending
	// interrupts
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	OCR1A = 19531;

	// Turn timer 1 on with a 1024 prescaler
	uint8_t timer_1_on = (1 << CS12) | (1 << CS10);
	uint8_t timer_1_off = ~((1 << CS10) | (1 << CS11) | (1 << CS12));

	uint8_t initial_time_set = 0;

	while (1) {
		switch (state) {
		case STARTUP:
			ADMUX = ADMUX & ~0b11111;
			ADCSRA |= (1 <<ADSC);
			while (ADCSRA & (1 << ADSC));

			struct Time new_work = debounce_seconds(work_time, ADC);
			if (new_work.seconds != work_time.seconds) {
				work_time.seconds = new_work.seconds;
				set_time(buf, work_time, 0, 255, 0);
			}

			ADMUX = (ADMUX & ~0b11111) | 1;
			ADCSRA |= (1 <<ADSC);
			while (ADCSRA & (1 << ADSC));

			struct Time new_rest = debounce_seconds(rest_time, ADC);
			if (new_rest.seconds != rest_time.seconds) {
				rest_time = new_rest;
				set_time(buf, rest_time, 255, 0, 0);
			}

			if (!initial_time_set) {
				initial_time_set = 1;
				set_time(buf, work_time, 0, 255, 0);
			}

			if (debounce_start(!(PINB & (1 << PB2)))) {
				running_time = work_time;
				TCCR1B |= timer_1_on;
				state = WORK;
			}
			break;

		case WORK:
			set_time(buf, running_time, 0, 255, 0);
			if (debounce_stop(!(PINB & (1 << PB3)))) {
				TCCR1B &= timer_1_off;
				paused_state = state;
				state = PAUSE;
				while (!(PINB & (1 << PB3)));
			}
			break;

		case REST:
			set_time(buf, running_time, 255, 0, 0);
			if (debounce_stop(!(PINB & (1 << PB3)))) {
				TCCR1B &= timer_1_off;
				paused_state = state;
				state = PAUSE;
				while (!(PINB & (1 << PB3)));
			}
			break;

		case PAUSE:
			if (debounce_start(!(PINB & (1 << PB2)))) {
				state = paused_state;
				TCCR1B |= timer_1_on;
			}
			if (debounce_stop(!(PINB & (1 << PB3)))) {
				set_time(buf, work_time, 0, 255, 0);
				TCNT1 = 0;
				state = STARTUP;
			}
			break;

		}

		write_pixels(buf, sizeof(buf));
	}

	return 0;
}

ISR(TIMER1_COMPA_vect) {
	PORTB &= ~(1 << PB4);
	if (running_time.minutes == 0 && running_time.seconds == 1) {
		if (state == WORK) {
			running_time = rest_time;
			state = REST;
		} else if (state == REST) {
			running_time = work_time;
			state = WORK;
		}
		PORTB |= (1 << PB4);
	} else if (running_time.seconds == 0) {
		running_time.minutes--;
		running_time.seconds = 59;
	} else {
		running_time.seconds--;
	}
}
