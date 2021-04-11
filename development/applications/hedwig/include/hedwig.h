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

#define led_TASK_PRIORITY            (tskIDLE_PRIORITY + 2)
#define device_com_TASK_PRIORITY     (tskIDLE_PRIORITY + 4)
#define sensors_TASK_PRIORITY        (tskIDLE_PRIORITY + 7)
#define flight_control_TASK_PRIORITY (tskIDLE_PRIORITY + 5)

#define DEVICE_COM_STACK_SIZE        512
#define SENSORS_STACK_SIZE           512
#define LED_STACK_SIZE               128
#define FLIGHT_CONTROL_STACK_SIZE    512

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
