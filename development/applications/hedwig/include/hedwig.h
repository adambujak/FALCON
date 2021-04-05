#ifndef HEDWIG_H
#define HEDWIG_H

#include "falcon_common.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "logger.h"

#include <stdint.h>


#define led_TASK_PRIORITY         (tskIDLE_PRIORITY + 2)
#define sensors_TASK_PRIORITY     (tskIDLE_PRIORITY + 4)
#define device_com_TASK_PRIORITY  (tskIDLE_PRIORITY + 5)


#define RTOS_ERR_CHECK(x)   \
  do {                      \
    int retval = (x);       \
    if (retval != pdPASS) { \
      error_handler();      \
    }                       \
  } while (0)

void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void rtos_delay_ms(uint32_t ms);

extern void hedwig_sysTickHandler(void);
void OSSysTickHandler(void);

#endif  // HEDWIG_H
