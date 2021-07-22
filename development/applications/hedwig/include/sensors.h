#ifndef SENSORS_H
#define SENSORS_H

#include "flightController.h"

void sensors_task_start(void);
void sensors_task_setup(void);
void sensors_calibrate(void);
void sensors_get_bias(sensor_bias_t *bias);

#endif
