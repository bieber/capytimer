#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include "time.h"

#define BUTTON_COUNT 20
#define SECOND_THRESHOLD 15

struct Time debounce_seconds(struct Time existing, uint16_t adc_value);

uint8_t debounce_start(uint8_t value);
uint8_t debounce_stop(uint8_t value);

#endif
