#ifndef FLIGHT_CONTROL_H
#define FLIGHT_CONTROL_H

void flight_control_task_start(void);
void flight_control_setup (void);

void flight_control_set_sensor_data(float *gyro_data, float *accel_data, float *quat_data, float alt_data);

#endif // FLIGHT_CONTROL_H