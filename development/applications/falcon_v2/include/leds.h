#ifndef LEDS_H
#define LEDS_H

void leds_toggle (void);
void led_task_setup (void);
void led_task (void *pvParameters);

#endif // LEDS_H
