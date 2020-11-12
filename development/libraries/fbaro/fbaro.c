/******************************************************************************
 * @file     fbaro.c 
 * @brief    Falcon Barometer
 * @version  1.0
 * @date     2020-11-11
 * @author   Devin Bell
 ******************************************************************************/

#include "fbaro.h"

int fbaro_init(fbaro_config_t *config)
{
  MPL3115A2_i2c_connect(config->i2cHandle);

  unsigned char id = MPL3115A2_ID();

  if (id != config->chip_id) {
    return FLN_ERR;
  }

  MPL3115A2_Initialize();     //Initialize the sensor
  MPL3115A2_ActiveMode();     //Configure the sensor for active mode

  barometer.active = TRUE;

  MPL3115A2_AltimeterMode();

  float sample_delay_ms = (1000/config->sample_rate);

  if (sample_delay_ms < 10) {
    barometer.sample_setting = SR1;
  }
  else if (sample_delay_ms >= 10 && sample_delay_ms < 18) {
    barometer.sample_setting = SR2;
  }
  else if (sample_delay_ms >= 18 && sample_delay_ms < 34) {
    barometer.sample_setting = SR3;
  }
  else if (sample_delay_ms >= 34 && sample_delay_ms < 66) {
    barometer.sample_setting = SR4;
  }
  else if (sample_delay_ms >= 66 && sample_delay_ms < 130) {
    barometer.sample_setting = SR5;
  }
  else if (sample_delay_ms >= 130 && sample_delay_ms < 258) {
    barometer.sample_setting = SR6;
  }
  else if (sample_delay_ms >= 258 && sample_delay_ms < 512) {
    barometer.sample_setting = SR7;
  }
  else {
    barometer.sample_setting = SR8;
  }

  MPL3115A2_OutputSampleRate(barometer.sample_setting);

  return FLN_OK;
}

void fbaro_get_altitude(float *alt_data)
{
  *alt_data = MPL3115A2_ReadAltitude() - barometer.start_alt;
}

int fbaro_calibrate(void)
{
  if (barometer.active != TRUE) {
    return FLN_ERR;
  }

  DEBUG_LOG("Starting Altitude Cal\r\n");

  MPL3115A2_OutputSampleRate(OS_1);

  vTaskDelay(100);

  int samples = 500;

  float test_data;

  for(int i=0; i<samples; i++)
  {
    float temp = MPL3115A2_ReadAltitude();
    DEBUG_LOG("Reading %d: %7.5f\r\n",i,temp);
    test_data += temp;
    vTaskDelay(10);
  }

  barometer.start_alt = test_data/samples;

  MPL3115A2_OutputSampleRate(barometer.sample_setting);

  return FLN_OK;
}
