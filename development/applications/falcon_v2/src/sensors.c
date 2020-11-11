#include "sensors.h"

#include "falcon_common.h"
#include "bsp.h"
#include "logger.h"
#include "fimu.h"

fln_i2c_handle_t i2cHandle;

volatile unsigned char new_gyro;
void gyro_data_ready_cb(void)
{
  new_gyro = 1;
}

void sensors_task_setup(void)
{
  FLN_ERR_CHECK(bsp_i2c_init(&i2cHandle));
}

void sensors_task(void *pvParameters)
{
	DEBUG_LOG("TASK STARTED\r\n");

  fimu_config_t IMU_config = {
    .i2cHandle = &i2cHandle,
    .gyro_fsr = MPU_FS_2000dps,
    .accel_fsr = MPU_FS_2G,
    .output_data_rate = 200
  };

  FLN_ERR_CHECK(fimu_init(IMU_config));

  FLN_ERR_CHECK(fimu_calibrate());

  bsp_IMU_int_init(gyro_data_ready_cb);

  FLN_ERR_CHECK(fimu_start(IMU_config));

  while (1) {

    if (new_gyro == 1) {
      new_gyro = 0;
      fimu_fifo_handler(sensor_data.gyro_data_SI, sensor_data.accel_data_SI, sensor_data.quat_data);
      //DEBUG_LOG("Gyro Data\t %7.5f, %7.5f, %7.5f\r\n", sensor_data.gyro_data_SI[0], sensor_data.gyro_data_SI[1], sensor_data.gyro_data_SI[2]);
      DEBUG_LOG("Accel Data\t %7.5f, %7.5f, %7.5f\r\n", sensor_data.accel_data_SI[0], sensor_data.accel_data_SI[1], sensor_data.accel_data_SI[2]);
      // DEBUG_LOG("%7.5f\t%7.5f\t%7.5f\t%7.5f\r\n", sensor_data.quat_data[0], sensor_data.quat_data[1], sensor_data.quat_data[2], sensor_data.quat_data[3]);
      HAL_GPIO_TogglePin(GPIOE, 4U);
      HAL_GPIO_TogglePin(GPIOE, 4U);
    }
  }
}
