#ifndef FALCON_COMMON_H
#define FALCON_COMMON_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include <stdio.h>
#include <stdlib.h>

#include "logger.h"

#define FLN_OK  0
#define FLN_ERR -1

#ifdef DEBUG
#define DEBUG_LOG(fmt, ...)              \
  do {                                   \
    char str[128];                       \
    sprintf(str, (fmt), ## __VA_ARGS__); \
    logger_write(str);                   \
  } while (0)
#else
#define DEBUG_LOG(...) do {} while (0)
#endif

#define ASSERT(val)         \
  do {                      \
    if((val)) {             \
      error_handler();      \
    }                       \
  } while (0)

#define RTOS_ERR_CHECK(x)   \
  do {                      \
    int retval = (x);       \
    if (retval != pdPASS) { \
      error_handler();      \
    }                       \
  } while (0)

#define FLN_ERR_CHECK(x)    \
  do {                      \
    int retval = (x);       \
    if (retval != FLN_OK) { \
      error_handler();      \
    }                       \
  } while (0)

void error_handler(void);

#endif  // FALCON_COMMON_H
