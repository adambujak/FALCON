/******************************************************************************
 * @file     fimu.h 
 * @brief    Falcon IMU
 * @version  1.0
 * @date     2020-11-02
 * @author   Devin Bell
 ******************************************************************************/

#ifndef FIMU_H
#define FIMU_H

#include "falcon_common.h"
#include "bsp.h"

#include "inv_mems.h"

typedef struct
{
  fln_i2c_handle_t *i2cHandle;
  float output_data_rate;
  int gyro_fsr;
  int accel_fsr;
} fimu_config_t;

int fimu_init(fimu_config_t config);

int fimu_start(fimu_config_t config);

void fimu_fifo_handler(float *gyro_float, float *linAccFloat, float *rv_float);

int fimu_calibrate(void);

#endif // FIMU_H
