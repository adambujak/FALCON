/******************************************************************************
 * @file     fbaro.h 
 * @brief    Falcon Barometer
 * @version  1.0
 * @date     2020-11-11
 * @author   Devin Bell
 ******************************************************************************/

#ifndef FBARO_H
#define FBARO_H

#include "falcon_common.h"
#include "bsp.h"

#include "MPL3115A2.h"

typedef struct
{
	fln_i2c_handle_t *i2cHandle;
  unsigned char chip_id;
  float sample_rate;
} fbaro_config_t;

typedef struct
{
  uint8_t active;
  OverSample_t sample_setting;
  float start_alt;
} fbaro_t;

fbaro_t barometer;

int fbaro_init(fbaro_config_t *config);

int fbaro_calibrate(void);

void fbaro_get_altitude(float *alt_data);

#endif // FBARO_H
