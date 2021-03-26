#ifndef ALBUS_H
#define ALBUS_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "logger.h"

#include <stdint.h>
#include <stdio.h>

#define DISABLE_IRQ()        \
  uint32_t prim;             \
  prim = __get_PRIMASK();    \
  __disable_irq();           \

#define ENABLE_IRQ()         \
  if (!prim) {               \
    __enable_irq();          \
  }                          \

#endif  // ALBUS_H
