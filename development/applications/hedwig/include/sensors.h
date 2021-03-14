#ifndef SENSORS_H
#define SENSORS_H

#include "flightController.h"          /* Model's header file */
#include "rtwtypes.h"

void sensors_task_setup(void);
void sensors_task(void *pvParameters);

#endif
