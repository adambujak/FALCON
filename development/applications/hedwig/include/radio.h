#ifndef RADIO_H
#define RADIO_H

#include <stdint.h>

typedef void (*exti_cb_t)(void);

void radio_init(void);
void radio_process(void);
uint32_t radio_get_data(uint8_t *dest, uint32_t length);
uint32_t radio_send_data(uint8_t *source, uint32_t length);

#endif  // RADIO_H
