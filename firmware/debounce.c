#include <stdint.h>

#include "debounce.h"
#include "time.h"

struct Time debounce_seconds(struct Time existing, uint16_t adc_value) {
	uint16_t prospective = (adc_value * 12 / 1024) * 5;
	if (prospective > existing.seconds) {
		uint16_t boundary = 1024 * prospective / 60;
		if (adc_value < boundary + SECOND_THRESHOLD) {
			return existing;
		}
	} else if (0) {
		uint16_t boundary = 1024 * existing.seconds / 60;
		if (adc_value > boundary - SECOND_THRESHOLD) {
			return existing;
		}
	}
	existing.seconds = prospective;
	return existing;
}

uint8_t debounce_start(uint8_t value) {
	static uint8_t count = 0;
	static uint8_t last_value = 0;

	if (value == last_value) {
		count++;
	} else {
		count = 0;
		last_value = value;
	}

	if (count > BUTTON_COUNT) {
		return last_value;
	}
	return 0;
}

uint8_t debounce_stop(uint8_t value) {
	static uint8_t count = 0;
	static uint8_t last_value = 0;

	if (value == last_value) {
		count++;
	} else {
		count = 0;
		last_value = value;
	}

	if (count > BUTTON_COUNT) {
		return last_value;
	}
	return 0;
}
