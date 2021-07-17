#include "imu.h"

#include <stdint.h>

#include "falcon_common.h"
#include "i2c.h"
#include "icm20948_api.h"

static int8_t i2c_write(uint8_t addr, const uint8_t *data, const uint32_t len) {
  icm20948_return_code_t ret = ICM20948_RET_OK;
  if (i2c_imu_write(ICM20948_SLAVE_ADDR, addr, data, len) != FLN_OK) {
    return ICM20948_RET_GEN_FAIL;
  }
  return ret;
}

static int8_t i2c_read(uint8_t addr, uint8_t *data, uint32_t len) {
  icm20948_return_code_t ret = ICM20948_RET_OK;
  if (i2c_imu_read(ICM20948_SLAVE_ADDR, addr, data, len) != FLN_OK) {
    return ICM20948_RET_GEN_FAIL;
  }
  return ret;
}

int imu_get_data(void)
{
  icm20948_gyro_t gyro_data;
  icm20948_accel_t accel_data;
  uint8_t config;

  int ret = 0;
  ret |= icm20948_getGyroData(&gyro_data);
  ret |= icm20948_getAccelData(&accel_data);

  ret |= icm20948_getAccelConfig(&config);
  LOG_DEBUG("accel: %d, %d, %d, %d, %d\r\n", accel_data.x, accel_data.y, accel_data.z, config, ret);
  return ret;
}

int imu_init(void) {
  icm20948_return_code_t ret = ICM20948_RET_OK;
  icm20948_settings_t settings;

  // Init the device function pointers
  ret = icm20948_init(i2c_read, i2c_write, delay_us);

  // Check if we successfully stored the function poiners provided
  if (ret == ICM20948_RET_OK) {
      settings.gyro.en = ICM20948_MOD_ENABLED;
      settings.gyro.fs = ICM20948_GYRO_FS_SEL_2000DPS;
      settings.accel.en = ICM20948_MOD_ENABLED;
      settings.accel.fs = ICM20948_ACCEL_FS_SEL_16G;
      ret = icm20948_applySettings(&settings);
  }

  ret |= icm20948_setAccelConfig();
  if (ret == ICM20948_RET_OK) {
    LOG_DEBUG("IMU configured\r\n");
    return FLN_OK;
  }

  LOG_ERROR("Error applying IMU settings\r\n");
  return FLN_ERR;
}
