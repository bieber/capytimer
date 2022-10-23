#include <stdint.h>

#include "debounce.h"

uint16_t debounce_minute(uint16_t existing, uint16_t adc_value) {
	uint16_t prospective = (adc_value * 12 / 1024) * 5;
	if (prospective > existing) {
		uint16_t boundary = 1024 * prospective / 60;
		if (adc_value < boundary + MINUTE_THRESHOLD) {
			return existing;
		}
	} else if (0) {
		uint16_t boundary = 1024 * existing / 60;
		if (adc_value > boundary - MINUTE_THRESHOLD) {
			return existing;
		}
	}
	return prospective;
}
