#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef void (*exti_cb_t)(void);

void gpio_init(void);
void gpio_rf_irq_register(exti_cb_t callback);
void gpio_rf_ce_write(uint8_t value);
void gpio_rf_cs_write(uint8_t value);

#endif  // GPIO_H
