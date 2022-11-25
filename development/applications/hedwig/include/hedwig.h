#ifndef HEDWIG_H
#define HEDWIG_H

#include "falcon_common.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "logger.h"

#include <stdint.h>

#define led_TASK_PRIORITY               (tskIDLE_PRIORITY + 2)
#define device_com_TASK_PRIORITY        (tskIDLE_PRIORITY + 4)
#define sensors_TASK_PRIORITY           (tskIDLE_PRIORITY + 5)
#define flight_control_TASK_PRIORITY    (tskIDLE_PRIORITY + 5)

#define DEVICE_COM_STACK_SIZE           512
#define SENSORS_STACK_SIZE              512
#define LED_STACK_SIZE                  128
#define FLIGHT_CONTROL_STACK_SIZE       512

#define SYS_TICK_FREQ                   1000
#define MS_TO_TICKS(MS)                 (MS)
#define TICKS_TO_MS(ticks)              (ticks)

#define DISABLE_IRQ()     \
  uint32_t prim;          \
  prim = __get_PRIMASK(); \
  __disable_irq();        \

#define ENABLE_IRQ() \
  if (!prim) {       \
    __enable_irq();  \
  }                  \

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
