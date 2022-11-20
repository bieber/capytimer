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

#define REST_DISPLAY_TIME_MS 5000
#define BUZZ_TIME_REST_MS 800
#define BUZZ_TIME_WORK_MS 1000
#define BUZZ_ALTERNATE_MS 40

enum State {STARTUP, WORK, REST, PAUSE};

struct Time work_time = {0, 0};
struct Time rest_time = {0, 0};
uint8_t brightness = 0;

volatile struct Time running_time = {0, 0};
volatile uint8_t current_round = 0;

volatile uint16_t display_rest_cycles = 0;

volatile uint16_t buzz_cycles = 0;
volatile uint16_t buzz_alternate_cycles = 0;
volatile uint8_t buzz_alternate = 0;

volatile enum State state = STARTUP;
volatile enum State paused_state;

void start();
void pause();
void resume();
void reset();
void advance_state();

void buzz_work();
void buzz_rest();
void buzz_off();

int main(void) {
	uint8_t pixels[SCREEN_SIZE];
	setup(pixels);

	uint8_t initial_time_set = 0;

	struct Time new_work;
	struct Time new_rest;

	while (1) {
		brightness = debounce_brightness(brightness, ADC_BRIGHTNESS);
		uint8_t time_set = 0;

		if (!(PORT_DIAGNOSTIC & (1 << PIN_DIAGNOSTIC))) {
			// In diagnostic mode, we just display the raw value from
			// the work minutes potentiometer.  We can use this to
			// profile dials for the final product
			//set_number(pixels, read_adc(ADC_WORK_MINUTES), 255, 255, 255);
			set_number(pixels, PINC, 255, 255, 255);
			write_pixels(pixels, sizeof(pixels));
			_delay_ms(250);
			continue;
		}

		switch (state) {
		case STARTUP:
			new_work = debounce_time(
				work_time,
				ADC_WORK_MINUTES,
				ADC_WORK_SECONDS
			);
			if (!time_eq(new_work, work_time)) {
				work_time = new_work;
				TCCR0B &= TIMER_0_OFF;
				display_rest_cycles = 0;
			}

			new_rest = debounce_time(
				rest_time,
				ADC_REST_MINUTES,
				ADC_REST_SECONDS
			);
			if (!time_eq(new_rest, rest_time)) {
				rest_time = new_rest;
				if (initial_time_set) {
					display_rest_cycles = REST_DISPLAY_TIME_MS / 10;
					TCCR0B |= TIMER_0_ON;
				}
			}

			if (!initial_time_set) {
				initial_time_set = 1;
			}
			if (display_rest_cycles > 0) {
				set_time(pixels, rest_time, brightness, 0, 0);
			} else {
				set_time(pixels, work_time, 0, brightness, 0);
			}

			if (debounce_button(BUTTON_START) || debounce_button(RF_START)) {
				start();
			}
			break;

		case WORK:
			set_time(pixels, running_time, 0, brightness, 0);
			time_set = 1;
			// fallthrough
		case REST:
			if (!time_set) {
				set_time(pixels, running_time, brightness, 0, 0);
			}
			if (debounce_button(BUTTON_STOP) || debounce_button(RF_STOP)) {
				pause();
			}
			break;

		case PAUSE:
			if (debounce_button(BUTTON_START) || debounce_button(RF_START)) {
				resume();
			}
			if (debounce_button(BUTTON_STOP) || debounce_button(RF_STOP)) {
				reset();
				set_time(pixels, work_time, 0, brightness, 0);
			}
			break;
		}

		set_round(pixels, current_round, 0, 0, brightness);
		write_pixels(pixels, sizeof(pixels));
		_delay_ms(1);
	}

	return 0;
}

ISR(TIMER1_COMPA_vect) {
	if (running_time.minutes == 0 && running_time.seconds <= 1) {
		advance_state();
	} else if (running_time.seconds == 0) {
		running_time.minutes--;
		running_time.seconds = 59;
	} else {
		running_time.seconds--;
	}
}

ISR(TIMER0_COMPA_vect) {
	display_rest_cycles--;
	if (display_rest_cycles == 0) {
		TCCR0B &= TIMER_0_OFF;
	}
}

ISR(TIMER2_COMPA_vect) {
	buzz_cycles--;
	if (buzz_cycles == 0) {
		buzz_off();
		return;
	}
	if (buzz_alternate) {
		buzz_alternate_cycles--;
		if (buzz_alternate_cycles == 0) {
			buzz_alternate_cycles = BUZZ_ALTERNATE_MS / 10;
			PORT_BUZZ ^= (1 << PIN_BUZZ);
		}
	}
}

void start() {
	if (time_empty(work_time) && time_empty(rest_time)) {
		// We need at least one of our two periods to be greater than
		// 0, otherwise the buzzer will just run nonstop
		return;
	}


	if (time_empty(work_time)) {
		// If there's no work time set, we'll have to go straight to
		// the rest time
		running_time = rest_time;
		state = REST;
		buzz_rest();
	} else {
		running_time = work_time;
		state = WORK;
		buzz_work();
	}

	current_round = 1;
	TCCR1B |= TIMER_1_ON;
}

void pause() {
	TCCR1B &= TIMER_1_OFF;
	paused_state = state;
	state = PAUSE;
	buzz_off();
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
		if (time_empty(rest_time)) {
			// If there's no rest time, just reset the work time
			// counter
			running_time = work_time;
			current_round = (current_round + 1) % 100;
			buzz_work();
		} else {
			running_time = rest_time;
			state = REST;
			buzz_rest();
		}
	} else if (state == REST) {
		current_round = (current_round + 1) % 100;
		if (time_empty(work_time)) {
			// If there's no work time, just reset the rest time
			// counter
			running_time = rest_time;
			buzz_rest();
		} else {
			running_time = work_time;
			state = WORK;
			buzz_work();
		}
	}
}

void buzz_work() {
	buzz_cycles = BUZZ_TIME_WORK_MS / 10;
	buzz_alternate_cycles = BUZZ_ALTERNATE_MS / 10;
	buzz_alternate = 1;
	PORT_BUZZ |= (1 << PIN_BUZZ);
	TCCR2B |= TIMER_2_ON;
}

void buzz_rest() {
	buzz_cycles = BUZZ_TIME_REST_MS / 10;
	buzz_alternate = 0;
	PORT_BUZZ |= (1 << PIN_BUZZ);
	TCCR2B |= TIMER_2_ON;
}

void buzz_off() {
	TCCR2B &= TIMER_2_OFF;
	PORT_BUZZ &= ~(1 << PIN_BUZZ);
}
