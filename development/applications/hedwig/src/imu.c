#include "imu.h"

#include <stdint.h>

#include "falcon_common.h"
#include "i2c.h"
#include "icm20948_api.h"

static float accel_scale_factor_array[4] = {
  16384.f,
  8192.f,
  4096.f,
  2048.f
};

static float gyro_scale_factor_array[4] = {
  131.f,
  65.5f,
  32.8f,
  16.4f
};

static icm20948_settings_t settings;

static int8_t icm_write(uint8_t addr, const uint8_t *data, const uint32_t len) {
  icm20948_return_code_t ret = ICM20948_RET_OK;
  if (i2c_imu_write(ICM20948_SLAVE_ADDR, addr, data, len) != FLN_OK) {
    return ICM20948_RET_GEN_FAIL;
  }
  return ret;
}

static int8_t icm_read(uint8_t addr, uint8_t *data, uint32_t len) {
  icm20948_return_code_t ret = ICM20948_RET_OK;
  if (i2c_imu_read(ICM20948_SLAVE_ADDR, addr, data, len) != FLN_OK) {
    return ICM20948_RET_GEN_FAIL;
  }
  return ret;
}

int imu_calibrate(float *gyro_bias, float *accel_bias)
{
  // TODO: add this
  return FLN_OK;
}

int imu_get_data(float *accel, float *gyro)
{
  icm20948_gyro_t gyro_data;
  icm20948_accel_t accel_data;  

  int ret = 0;
  ret |= icm20948_getGyroData(&gyro_data);
  ret |= icm20948_getAccelData(&accel_data);

  if (ret == ICM20948_RET_OK) {
    accel[0] = accel_data.x / accel_scale_factor_array[settings.accel.fs];
    accel[1] = accel_data.y / accel_scale_factor_array[settings.accel.fs];
    accel[2] = accel_data.z / accel_scale_factor_array[settings.accel.fs];
    gyro[0] = gyro_data.x / gyro_scale_factor_array[settings.gyro.fs];
    gyro[1] = gyro_data.y / gyro_scale_factor_array[settings.gyro.fs];
    gyro[2] = gyro_data.z / gyro_scale_factor_array[settings.gyro.fs];
    return FLN_OK;
  }

  return FLN_ERR;
}

int imu_init(void) {
  icm20948_return_code_t ret = ICM20948_RET_OK;

  // Init the device function pointers
  ret = icm20948_init(icm_read, icm_write, delay_us);

  settings.gyro.en = ICM20948_MOD_ENABLED;
  settings.gyro.fs = ICM20948_GYRO_FS_SEL_2000DPS;
  settings.accel.en = ICM20948_MOD_ENABLED;
  settings.accel.fs = ICM20948_ACCEL_FS_SEL_16G;
  ret |= icm20948_applySettings(&settings);

  if (ret == ICM20948_RET_OK) {
    LOG_DEBUG("IMU configured\r\n");
    return FLN_OK;
  }

  LOG_ERROR("Error applying IMU settings\r\n");
  return FLN_ERR;
}
