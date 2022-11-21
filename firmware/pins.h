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

#define DDR_STATUS DDRB
#define PORT_STATUS PORTB
#define PIN_STATUS PB0

#define DDR_SCREEN DDRB
#define PORT_SCREEN PORTB
#define PIN_SCREEN PB1

#define DDR_BUZZ DDRB
#define PORT_BUZZ PORTB
#define PIN_BUZZ PB2

#define DDR_START DDRD
#define PULLUP_START PORTD
#define PORT_START PIND
#define PIN_START PD0

#define DDR_STOP DDRD
#define PULLUP_STOP PORTD
#define PORT_STOP PIND
#define PIN_STOP PD1

#define DDR_RF_START DDRD
#define PULLUP_RF_START PORTD
#define PORT_RF_START PIND
#define PIN_RF_START PD4

#define DDR_RF_STOP DDRD
#define PULLUP_RF_STOP PORTD
#define PORT_RF_STOP PIND
#define PIN_RF_STOP PD5

#define DDR_DIAGNOSTIC DDRD
#define PULLUP_DIAGNOSTIC PORTD
#define PORT_DIAGNOSTIC PIND
#define PIN_DIAGNOSTIC PD3

#define ADC_WORK_MINUTES 0
#define ADC_WORK_SECONDS 1
#define ADC_REST_MINUTES 2
#define ADC_REST_SECONDS 3
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
