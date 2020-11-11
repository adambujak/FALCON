#ifndef SENSORS_H
#define SENSORS_H

typedef struct {
  float gyro_data_SI[3];
  float accel_data_SI[3];
  float quat_data[4];
} sensor_data_t;

sensor_data_t sensor_data;

void sensors_task_setup(void);
void sensors_task(void *pvParameters);

#endif
