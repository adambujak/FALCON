#ifndef FALCON_COMMON_H
#define FALCON_COMMON_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"


#include "logger.h"

#define DEBUG_LOG(str) logger_write(str)

#define ERR_CHECK(err) do {} while((err) != pdPASS) 

#endif // FALCON_COMMON_H
