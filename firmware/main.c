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

#include "adc.h"
#include "debounce.h"
#include "digits.h"
#include "pins.h"
#include "setup.h"
#include "time.h"
#include "write_pixels.h"

enum State {STARTUP, WORK, REST, PAUSE};

volatile struct Time work_time = {0, 0};
volatile struct Time rest_time = {0, 0};

volatile struct Time running_time = {0, 0};
volatile uint8_t round = 0;

volatile enum State state = STARTUP;
volatile enum State paused_state;

int main(void) {
	uint8_t pixels[SCREEN_SIZE];
	setup(pixels);

	uint8_t initial_time_set = 0;

	struct Time new_work;
	struct Time new_rest;

	while (1) {
		uint8_t time_set = 0;

		switch (state) {
		case STARTUP:
			new_work = debounce_seconds(
				work_time,
				read_adc(ADC_WORK_SECONDS)
			);
			if (new_work.seconds != work_time.seconds) {
				work_time.seconds = new_work.seconds;
				set_time(pixels, work_time, 0, 255, 0);
			}

			new_rest = debounce_seconds(
				rest_time,
				read_adc(ADC_REST_SECONDS)
			);
			if (new_rest.seconds != rest_time.seconds) {
				rest_time = new_rest;
				set_time(pixels, rest_time, 255, 0, 0);
			}

			if (!initial_time_set) {
				initial_time_set = 1;
				set_time(pixels, work_time, 0, 255, 0);
			}

			if (debounce_button(BUTTON_START)) {
				running_time = work_time;
				TCCR1B |= TIMER_1_ON;
				state = WORK;
			}
			break;

		case WORK:
			set_time(pixels, running_time, 0, 255, 0);
			time_set = 1;
			// fallthrough
		case REST:
			if (!time_set) {
				set_time(pixels, running_time, 255, 0, 0);
			}
			if (debounce_button(BUTTON_STOP)) {
				TCCR1B &= TIMER_1_OFF;
				paused_state = state;
				state = PAUSE;
				while (!(PORT_STOP & (1 << PIN_STOP)));
			}
			break;

		case PAUSE:
			if (debounce_button(BUTTON_START)) {
				state = paused_state;
				TCCR1B |= TIMER_1_ON;
			}
			if (debounce_button(BUTTON_STOP)) {
				set_time(pixels, work_time, 0, 255, 0);
				TCNT1 = 0;
				state = STARTUP;
			}
			break;

		}

		set_round(pixels, round, 0, 0, 255);
		write_pixels(pixels, sizeof(pixels));
		_delay_ms(1);
	}

	return 0;
}

ISR(TIMER1_COMPA_vect) {
	PORT_BUZZ &= ~(1 << PIN_BUZZ);
	if (running_time.minutes == 0 && running_time.seconds == 1) {
		if (state == WORK) {
			running_time = rest_time;
			state = REST;
		} else if (state == REST) {
			running_time = work_time;
			state = WORK;
		}
		PORT_BUZZ |= (1 << PIN_BUZZ);
	} else if (running_time.seconds == 0) {
		running_time.minutes--;
		running_time.seconds = 59;
	} else {
		running_time.seconds--;
	}
}
