#ifndef ALBUS_H
#define ALBUS_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "logger.h"

#include <stdint.h>

#define DISABLE_IRQ()        \
  uint32_t prim;             \
  prim = __get_PRIMASK();    \
  __disable_irq();           \

#define ENABLE_IRQ()         \
  if (!prim) {               \
    __enable_irq();          \
  }                          \

#define SYSCLK_FRQ 96000000

#define US_TO_SYSTICK(us) (SYSCLK_FRQ / 1000000) * (us)

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void rtos_delay_ms(uint32_t ms);

#endif  // ALBUS_H
