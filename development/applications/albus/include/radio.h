#ifndef RADIO_H
#define RADIO_H

#include <stdint.h>

typedef void (*exti_cb_t)(void);

void radio_init(void);

#endif  // RADIO_H
