#include "sensors.h"

#include "falcon_common.h"
#include "bsp.h"
#include "logger.h"
#include "leds.h"
#include "fimu.h"
#include "fbaro.h"
#include "FreeRTOS.h"
#include "timers.h"

fln_i2c_handle_t i2cHandle;

#define IMU_SAMPLE_RATE (100.f)
#define BARO_SAMPLE_RATE (100.f)

#define BARO_DELAY_COUNT ( IMU_SAMPLE_RATE / BARO_SAMPLE_RATE )

int baro_delay_count = BARO_DELAY_COUNT;

float gyro_bias[3] = { 0.0F, 0.0F, 0.0F };
float accel_bias[3] = { 0.0F, 0.0F, 0.0F };
float quat_bias[4] = { 0.0F, 0.0F, 0.0F, 0.0F };

static volatile bool imuDataReady = false;
void IMU_data_ready_cb(void)
{
  imuDataReady = true;
}

void sensors_calibrate(void)
{
  FLN_ERR_CHECK(fimu_calibrate_DMP());

  FLN_ERR_CHECK(fimu_calibrate_offset());

  fimu_calibrate(gyro_bias, accel_bias, quat_bias);
}

void sensors_task_setup(void)
{
    fimu_config_t IMU_config = {
    .i2cHandle = &i2cHandle,
    .gyro_fsr = MPU_FS_2000dps,
    .accel_fsr = MPU_FS_16G,
    .output_data_rate = IMU_SAMPLE_RATE
  };

  fbaro_config_t baro_config = {
    .i2cHandle = &i2cHandle,
    .chip_id = 0xC4,
    .sample_rate = BARO_SAMPLE_RATE
  };

  FLN_ERR_CHECK(bsp_i2c_init(&i2cHandle));

  FLN_ERR_CHECK(fimu_init(IMU_config));

  FLN_ERR_CHECK(fbaro_init(&baro_config));
}

void sensors_task(void *pvParameters)
{
	DEBUG_LOG("SENSORS TASK STARTED\r\n");

  FLN_ERR_CHECK(fbaro_calibrate());

  FLN_ERR_CHECK(fimu_start(IMU_config));

  fimu_calibrate(gyro_bias, accel_bias, quat_bias);

  bsp_IMU_int_init(gyro_data_ready_cb);

  while (1) {
    if (imuDataReady) {
      imuDataReady = false;
      fimu_fifo_handler(rtU_Sensors.gyro_data_SI, rtU_Sensors.accel_data_SI, rtU_Sensors.quat_data);
        for(int i=0; i<3; i++) {
        rtU_Sensors.accel_data_SI[i] -= accel_bias[i];
        rtU_Sensors.gyro_data_SI[i] -= gyro_bias[i];
      }

      if(baro_delay_count == BARO_DELAY_COUNT)
      {
        fbaro_get_altitude(&rtU_Sensors.alt_data_SI);        
        baro_delay_count = 0;
      }
    }
  }
}
