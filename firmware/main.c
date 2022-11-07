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
volatile uint8_t current_round = 0;

volatile enum State state = STARTUP;
volatile enum State paused_state;

void start();
void pause();
void resume();
void reset();
void advance_state();

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
			new_work = debounce_time(
				work_time,
				ADC_WORK_MINUTES,
				ADC_WORK_SECONDS
			);
			if (!time_eq(new_work, work_time)) {
				work_time = new_work;
				set_time(pixels, work_time, 0, 255, 0);
			}

			new_rest = debounce_time(
				rest_time,
				ADC_REST_MINUTES,
				ADC_REST_SECONDS
			);
			if (!time_eq(new_rest, rest_time)) {
				rest_time = new_rest;
				set_time(pixels, rest_time, 255, 0, 0);
			}

			if (!initial_time_set) {
				initial_time_set = 1;
				set_time(pixels, work_time, 0, 255, 0);
			}

			if (debounce_button(BUTTON_START)) {
				start();
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
				pause();
				while (!(PORT_STOP & (1 << PIN_STOP)));
			}
			break;

		case PAUSE:
			if (debounce_button(BUTTON_START)) {
				resume();
			}
			if (debounce_button(BUTTON_STOP)) {
				reset();
				set_time(pixels, work_time, 0, 255, 0);
			}
			break;
		}

		set_round(pixels, current_round, 0, 0, 255);
		write_pixels(pixels, sizeof(pixels));
		_delay_ms(1);
	}

	return 0;
}

ISR(TIMER1_COMPA_vect) {
	PORT_BUZZ &= ~(1 << PIN_BUZZ);
	if (running_time.minutes == 0 && running_time.seconds <= 1) {
		advance_state();
	} else if (running_time.seconds == 0) {
		running_time.minutes--;
		running_time.seconds = 59;
	} else {
		running_time.seconds--;
	}
}

void start() {
	if (time_empty(work_time) && time_empty(rest_time)) {
		// We need at least one of our two periods to be greater than
		// 0, otherwise the buzzer will just run nonstop
		return;
	}

	running_time = work_time;
	current_round = 1;
	TCCR1B |= TIMER_1_ON;
	state = WORK;
}

void pause() {
	TCCR1B &= TIMER_1_OFF;
	paused_state = state;
	state = PAUSE;
}

void resume() {
	state = paused_state;
	TCCR1B |= TIMER_1_ON;
}

void reset() {
	TCNT1 = 0;
	state = STARTUP;
	current_round = 0;
}

void advance_state() {
	if (state == WORK) {
		running_time = rest_time;
		state = REST;
	} else if (state == REST) {
		running_time = work_time;
		state = WORK;
		current_round = (current_round + 1) % 100;
	}

	PORT_BUZZ |= (1 << PIN_BUZZ);
}
