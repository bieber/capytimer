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

#ifndef DIGITS_H
#define DIGITS_H

#include <stdint.h>

#include "time.h"

#define DIGIT_ADDRESS(start, digit) \
	(digit < 3 ? start + digit * 63 : start + 195 + 63 * (digit - 2))

void set_digit(uint8_t *buf, uint8_t value, uint8_t r, uint8_t g, uint8_t b);
void set_time(uint8_t *buf, struct Time time, uint8_t r, uint8_t g, uint8_t b);

#endif
