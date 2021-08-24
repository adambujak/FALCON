#ifndef IMU_H
#define IMU_H

int imu_init(unsigned short data_output_period_ms);
int imu_get_data(float *accel_float, float *gyro_float, float *compass_float);
int imu_get_bias(float *accel_bias_out, float *gyro_bias_out);
int imu_set_bias(float *accel_bias_in, float *gyro_bias_in);
int imu_start_calibration(void (*complete_cb)(void));

#endif // IMU_H
