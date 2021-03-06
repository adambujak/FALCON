#include "sensors.h"

#include "falcon_common.h"
#include "bsp.h"
#include "fimu.h"
#include "fbaro.h"
#include "flight_control.h"
#include <stdbool.h>

#include "flightController.h"
#include "rtwtypes.h"

fln_i2c_handle_t i2cHandle;

static bool calibration_required = false;

#define IMU_SAMPLE_RATE  (200.f)
#define BARO_SAMPLE_RATE (10.f)

#define BARO_DELAY_COUNT (IMU_SAMPLE_RATE / BARO_SAMPLE_RATE)

int baro_delay_count = BARO_DELAY_COUNT;

static float gyro_bias[3] = {0.0F, 0.0F, 0.0F};
static float accel_bias[3] = {0.0F, 0.0F, 0.0F};
static float quat_bias[4] = {1.0F, 0.0F, 0.0F, 0.0F};

static float gyro_data[3] = {0.0F, 0.0F, 0.0F};
static float accel_data[3] = {0.0F, 0.0F, 0.0F};
static float quat_data[4] = {0.0F, 0.0F, 0.0F, 0.0F};
static float alt_data = 0.0F;

static fimu_config_t IMU_config = {
  .i2cHandle = &i2cHandle,
  .gyro_fsr = MPU_FS_2000dps,
  .accel_fsr = MPU_FS_16G,
  .output_data_rate = IMU_SAMPLE_RATE
};

static fbaro_config_t baro_config = {
  .i2cHandle = &i2cHandle,
  .chip_id = 0xC4,
  .sample_rate = BARO_SAMPLE_RATE
};

static TaskHandle_t sensors_task_handle = NULL;

void IMU_data_ready_cb(void)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  xTaskNotifyFromISR(sensors_task_handle,
                     0,
                     eSetBits,
                     &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void sensors_calibrate(void)
{
  calibration_required = true;
}

void sensors_get_bias(sensor_bias_t *bias)
{
  memcpy(bias->gyro_bias, gyro_bias, sizeof(bias->gyro_bias));
  memcpy(bias->accel_bias, accel_bias, sizeof(bias->accel_bias));
}

static void calibrate(void)
{
  fimu_calibrate(gyro_bias, accel_bias, quat_bias);

  FLN_ERR_CHECK(fbaro_calibrate());

  calibration_required = false;
}

static void sensors_task(void *pvParameters)
{
  LOG_DEBUG("SENSORS TASK STARTED\r\n");

  flight_control_set_mode(FE_FLIGHT_MODE_CALIBRATING);

  FLN_ERR_CHECK(fbaro_calibrate());
  FLN_ERR_CHECK(fimu_start(IMU_config));

  flight_control_set_mode(FE_FLIGHT_MODE_IDLE);

  rtos_delay_ms(200);

  BaseType_t sensorNotification;

  while (1) {
    if (calibration_required) {
      calibrate();
    }
    /* Wait to be notified of an interrupt. */
    sensorNotification = xTaskNotifyWait(pdFALSE,
                                         0xFFFFFFFF,
                                         NULL,
                                         MS_TO_TICKS(10));

    if (sensorNotification == pdPASS) {
      fimu_fifo_handler(gyro_data, accel_data, quat_data);

      if (baro_delay_count == BARO_DELAY_COUNT) {
        fbaro_get_altitude(&alt_data);
        baro_delay_count = 0;
      }
      baro_delay_count++;

      flight_control_set_sensor_data(gyro_data, accel_data, quat_data, alt_data);

      rtos_delay_ms(1);
    }
    else {
      LOG_DEBUG("sensor notification not received\r\n");
      error_handler();
    }
  }
}

void sensors_task_setup(void)
{
  FLN_ERR_CHECK(bsp_i2c_init(&i2cHandle));
  FLN_ERR_CHECK(fimu_init(IMU_config));
  FLN_ERR_CHECK(fbaro_init(&baro_config));
  bsp_IMU_int_init(IMU_data_ready_cb);
}

void sensors_task_start(void)
{
  BaseType_t taskStatus = xTaskCreate(sensors_task,
                          "sensors_task",
                          SENSORS_STACK_SIZE,
                          NULL,
                          sensors_TASK_PRIORITY,
                          &sensors_task_handle);

  RTOS_ERR_CHECK(taskStatus);
}
