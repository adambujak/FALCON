#ifndef IMU_H
#define IMU_H

int imu_init(unsigned short data_output_period_ms);
int imu_get_data(float *accel_float, float *gyro_float, float *compass_float);
int imu_calibrate(float *, float *);

#endif // IMU_H
