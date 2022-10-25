#ifndef DIGITS_H
#define DIGITS_H

#include <stdint.h>

#include "time.h"

#define DIGIT_ADDRESS(start, digit) \
	(digit < 3 ? start + digit * 63 : start + 195 + 63 * (digit - 2))

void set_digit(uint8_t *buf, uint8_t value, uint8_t r, uint8_t g, uint8_t b);
void set_time(uint8_t *buf, struct Time time, uint8_t r, uint8_t g, uint8_t b);

#endif
