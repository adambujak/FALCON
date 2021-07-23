#ifndef IMU_H
#define IMU_H

int imu_init(void);
int imu_get_data(float *accel, float *gyro);
int imu_calibrate(float *, float *);

#endif // IMU_H
