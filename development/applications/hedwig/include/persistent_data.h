#ifndef PERSISTENT_DATA_H
#define PERSISTENT_DATA_H

#include "falcon_packet.h"

#include <stdbool.h>

void persistent_data_init(void);
void persistent_data_clear(void);
void persistent_data_write(void);

void persistent_data_imu_bias_set(float accel_bias[3], float gyro_bias[3]);
bool persistent_data_imu_bias_get(float accel_bias[3], float gyro_bias[3]);
bool persistent_data_controller_params_get(
       ft_fcs_att_control_params_t *att_control_params,
       ft_fcs_yaw_control_params_t *yaw_control_params,
       ft_fcs_alt_control_params_t *alt_control_params);
void persistent_data_controller_params_set(
       ft_fcs_att_control_params_t *att_control_params,
       ft_fcs_yaw_control_params_t *yaw_control_params,
       ft_fcs_alt_control_params_t *alt_control_params);

#endif // PERSISTENT_DATA_H
