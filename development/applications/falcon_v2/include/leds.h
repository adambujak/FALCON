#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

void leds_toggle(void);
void leds_set_color(uint8_t r, uint8_t g, uint8_t b);
void leds_task_setup(void);
void leds_task(void *pvParameters);

#endif  // LEDS_H
