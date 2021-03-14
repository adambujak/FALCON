#ifndef ALBUS_H
#define ALBUS_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "falcon_common.h"

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

extern void albus_delay(uint32_t ms);
extern void albus_sysTickHandler(void);
extern void OSSysTickHandler(void);

#endif  // ALBUS_H
