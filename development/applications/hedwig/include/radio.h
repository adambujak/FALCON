#ifndef RADIO_H
#define RADIO_H

#include "radio_common.h"
#include "frf.h"

#include <stdint.h>

typedef void (*exti_cb_t)(void);

void radio_init(void);
void radio_process(void);
void radio_reset(void);
uint32_t radio_rx_cnt_get(void);
uint32_t radio_status_get(void);
uint32_t radio_data_get(uint8_t *dest, uint32_t length);
uint32_t radio_data_send(uint8_t *source, uint32_t length);

#endif  // RADIO_H
