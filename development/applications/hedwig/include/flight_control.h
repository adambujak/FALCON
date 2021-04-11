#ifndef FLIGHT_CONTROL_H
#define FLIGHT_CONTROL_H

#include "falcon_packet.h"

void flight_control_task_start(void);
void flight_control_setup (void);

void flight_control_set_sensor_data(float *gyro_data, float *accel_data, float *quat_data, float alt_data);
void flight_control_set_command_data(fpc_flight_control_t *control_input);
void flight_control_get_outputs(fpr_status_t *status_response);
void flight_control_set_mode(fe_falcon_mode_t new_mode);


#endif // FLIGHT_CONTROL_H