#ifndef FALCON_COMMON_H
#define FALCON_COMMON_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"


#include "logger.h"

#define FLN_OK   0
#define FLN_ERR -1

#define DEBUG_LOG(str) logger_write(str)

#define RTOS_ERR_CHECK(err) do {} while((err) != pdPASS)

#define FLN_ERR_CHECK(x) do { \
  int retval = (x); \
  if (retval != FLN_OK) { \
    while(1);\
  } \
} while (0)

#endif // FALCON_COMMON_H
