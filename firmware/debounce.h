#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#define MINUTE_THRESHOLD 10

uint16_t debounce_minute(uint16_t existing, uint16_t adc_value);

#endif
