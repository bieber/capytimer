#ifndef DIGITS_H
#define DIGITS_H

#include <stdint.h>

#define DIGIT_ADDRESS(start, digit) \
	(digit < 3 ? start + digit * 21 : 48 + 21 * (digit - 2))

void set_digit(uint8_t *buf, uint8_t value, uint8_t r, uint8_t g, uint8_t b);

#endif
