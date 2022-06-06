/******************************************************************************
* @file     baro.c
* @brief    Barometer Source File (MPL3115A2)
* @version  1.0
* @date     2020-11-11
* @author   Devin Bell, Adam Bujak
******************************************************************************/
#include "baro.h"

#include <stdbool.h>
#include "falcon_common.h"
#include "i2c.h"
#include "MPL3115A2.h"

#define CHIP_ID    0xC4

typedef struct
{
  bool         is_active;
  OverSample_t sample_setting;
  float        start_alt;
} state_t;

static state_t state;

int baro_init(int sample_rate)
{
  unsigned char id = MPL3115A2_ID();

  if (id != CHIP_ID) {
    LOG_DEBUG("invalid chip id: %x\r\n", id);
    return FLN_ERR;
  }

  state.is_active = true;

  MPL3115A2_Initialize();     //Initialize the sensor

  MPL3115A2_ActiveMode();     //Configure the sensor for active mode

  MPL3115A2_AltimeterMode();

  int sample_delay_ms = (1000 / sample_rate);

  if (sample_delay_ms < 10) {
    state.sample_setting = SR1;
  }
  else if (sample_delay_ms >= 10 && sample_delay_ms < 18) {
    state.sample_setting = SR2;
  }
  else if (sample_delay_ms >= 18 && sample_delay_ms < 34) {
    state.sample_setting = SR3;
  }
  else if (sample_delay_ms >= 34 && sample_delay_ms < 66) {
    state.sample_setting = SR4;
  }
  else if (sample_delay_ms >= 66 && sample_delay_ms < 130) {
    state.sample_setting = SR5;
  }
  else if (sample_delay_ms >= 130 && sample_delay_ms < 258) {
    state.sample_setting = SR6;
  }
  else if (sample_delay_ms >= 258 && sample_delay_ms < 512) {
    state.sample_setting = SR7;
  }
  else {
    state.sample_setting = SR8;
  }

  MPL3115A2_OutputSampleRate(state.sample_setting);

  return FLN_OK;
}

void baro_get_altitude(float *alt_data)
{
  *alt_data = MPL3115A2_ReadAltitude() - state.start_alt;
}

int baro_calibrate(void)
{
  if (!state.is_active) {
    return FLN_ERR;
  }

  LOG_DEBUG("Starting Altitude Cal\r\n");

  MPL3115A2_OutputSampleRate(SR3);

  rtos_delay_ms(50);

  int samples = 25;

  float test_data = 0;

  for (int i = 0; i < samples; i++) {
    float temp = MPL3115A2_ReadAltitude();
    LOG_DEBUG("Reading %d: %7.5f\r\n", i, temp);

    if (temp != 0) {
      test_data += temp;
    }
    else {
      i--;
    }
    rtos_delay_ms(20);
  }

  state.start_alt = test_data / samples;

  MPL3115A2_OutputSampleRate(state.sample_setting);

  rtos_delay_ms(50);

  return FLN_OK;
}

unsigned char MPL3115A2_ReadByte(uint8_t reg)
{
  unsigned char data;

  i2c_baro_read(MPL3115A2_ADDRESS, reg, &data, 1U);
  return data;
}

void MPL3115A2_ReadByteArray(uint8_t reg, uint8_t *buffer, uint8_t length)
{
  i2c_baro_read(MPL3115A2_ADDRESS, reg, buffer, length);
}

void MPL3115A2_WriteByte(uint8_t reg, uint8_t value)
{
  i2c_baro_write(MPL3115A2_ADDRESS, reg, &value, 1);
}

void MPL3115A2_WriteByteArray(uint8_t reg, uint8_t *buffer, uint8_t length)
{
  i2c_baro_write(MPL3115A2_ADDRESS, reg, buffer, length);
}
