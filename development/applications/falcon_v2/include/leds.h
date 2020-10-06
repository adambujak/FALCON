#ifndef LEDS_H
#define LEDS_H

void leds_toggle(void);
void leds_task_setup(void);
void leds_task(void *pvParameters);

#endif  // LEDS_H
