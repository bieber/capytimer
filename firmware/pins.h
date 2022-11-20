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

#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

#define SCREEN_SIZE 384

#define DDR_SCREEN DDRB
#define PORT_SCREEN PORTB
#define PIN_SCREEN PB1

#define DDR_BUZZ DDRB
#define PORT_BUZZ PORTB
#define PIN_BUZZ PB4

#define DDR_START DDRB
#define PULLUP_START PORTB
#define PORT_START PINB
#define PIN_START PB2

#define DDR_STOP DDRB
#define PULLUP_STOP PORTB
#define PORT_STOP PINB
#define PIN_STOP PB3

#define DDR_RF_START DDRC
#define PULLUP_RF_START PORTC
#define PORT_RF_START PINC
#define PIN_RF_START PC6

#define DDR_RF_STOP DDRC
#define PULLUP_RF_STOP PORTC
#define PORT_RF_STOP PINC
#define PIN_RF_STOP PC5

#define DDR_DIAGNOSTIC DDRC
#define PULLUP_DIAGNOSTIC PORTC
#define PORT_DIAGNOSTIC PINC
#define PIN_DIAGNOSTIC PC7

#define ADC_WORK_MINUTES 2
#define ADC_WORK_SECONDS 0
#define ADC_REST_MINUTES 3
#define ADC_REST_SECONDS 1
#define ADC_BRIGHTNESS 4

// Turn timer 0 on with a 1024 prescaler
#define TIMER_0_ON ((1 << CS02) | (1 << CS00))
#define TIMER_0_OFF (~((1 << CS02) | (1 << CS01) | (1 << CS00)))

// Turn timer 1 on with a 1024 prescaler
#define TIMER_1_ON ((1 << CS12) | (1 << CS10))
#define TIMER_1_OFF (~((1 << CS12) | (1 << CS11) | (1 << CS10)))

// Turn timer 2 on with a 1024 prescaler
#define TIMER_2_ON ((1 << CS22) | (1 << CS21) | (1 << CS00))
#define TIMER_2_OFF (~((1 << CS22) | (1 << CS21) | (1 << CS20)))

#endif
