#include "sensors.h"

#include "falcon_common.h"
#include "bsp.h"
#include "logger.h"
#include "fimu.h"
#include "fbaro.h"

fln_i2c_handle_t i2cHandle;

#define IMU_SAMPLE_RATE (200.f)
#define BARO_SAMPLE_RATE (100.f)

#define BARO_DELAY_COUNT ( IMU_SAMPLE_RATE / BARO_SAMPLE_RATE )

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
    .output_data_rate = IMU_SAMPLE_RATE
  };

  fbaro_config_t baro_config = {
    .i2cHandle = &i2cHandle,
    .chip_id = 0xC4,
    .sample_rate = BARO_SAMPLE_RATE
  };

  FLN_ERR_CHECK(fimu_init(IMU_config));

  FLN_ERR_CHECK(fbaro_init(&baro_config));

  FLN_ERR_CHECK(fimu_calibrate());

  bsp_IMU_int_init(gyro_data_ready_cb);

  FLN_ERR_CHECK(fbaro_calibrate());

  FLN_ERR_CHECK(fimu_start(IMU_config));

  int baro_delay_count = BARO_DELAY_COUNT;
  while (1) {

    if (new_gyro == 1) {
      new_gyro = 0;
      fimu_fifo_handler(sensor_data.gyro_data_SI, sensor_data.accel_data_SI, sensor_data.quat_data);
      if(baro_delay_count == BARO_DELAY_COUNT)
      {
        fbaro_get_altitude(&sensor_data.alt_data_SI);
        baro_delay_count = 0;
        bsp_leds_toggle(FLN_LED_3);
      }
//      DEBUG_LOG("Gyro Data\t %7.5f, %7.5f, %7.5f\r\n", sensor_data.gyro_data_SI[0], sensor_data.gyro_data_SI[1], sensor_data.gyro_data_SI[2]);
//      DEBUG_LOG("Accel Data\t %7.5f, %7.5f, %7.5f\r\n", sensor_data.accel_data_SI[0], sensor_data.accel_data_SI[1], sensor_data.accel_data_SI[2]);
//      DEBUG_LOG("%7.5f\t%7.5f\t%7.5f\t%7.5f\r\n", sensor_data.quat_data[0], sensor_data.quat_data[1], sensor_data.quat_data[2], sensor_data.quat_data[3]);
      DEBUG_LOG("Alt Data\t %7.5f\r\n", sensor_data.alt_data_SI);
      baro_delay_count++;
    }


  }
}
