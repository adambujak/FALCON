#ifndef HEDWIG_H
#define HEDWIG_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include <stdint.h>

#include "falcon_common.h"

#define led_TASK_PRIORITY    (tskIDLE_PRIORITY + 2)
#define frf_TASK_PRIORITY  (tskIDLE_PRIORITY + 3)
#define sensors_TASK_PRIORITY (tskIDLE_PRIORITY + 4)
#define device_com_TASK_PRIORITY  (tskIDLE_PRIORITY + 5)

#include "logger.h"
#ifdef DEBUG
#define DEBUG_LOG(fmt, ...)              \
  do {                                   \
    char str[128];                       \
    sprintf(str, (fmt), ## __VA_ARGS__); \
    logger_write(str);                   \
  } while (0)
#else
#define DEBUG_LOG(...) do {} while (0)
#endif // DEBUG

#define RTOS_ERR_CHECK(x)   \
  do {                      \
    int retval = (x);       \
    if (retval != pdPASS) { \
      error_handler();      \
    }                       \
  } while (0)

extern void hedwig_delay(uint32_t ms);
extern void hedwig_sysTickHandler(void);
void OSSysTickHandler(void);

#endif  // HEDWIG_H
