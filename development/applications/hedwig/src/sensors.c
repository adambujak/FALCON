#include "sensors.h"

#include <stdbool.h>

#include "falcon_common.h"
#include "flightController.h"
#include "flight_control.h"
#include "i2c.h"
#include "imu.h"
#include "rtwtypes.h"

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

static TaskHandle_t sensors_task_handle = NULL;

static void sensor_timer_cb(void)
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
  //fimu_calibrate(gyro_bias, accel_bias, quat_bias);
  //FLN_ERR_CHECK(fbaro_calibrate());

  //calibration_required = false;
}

void sensors_calibrate(void)
{
  //calibration_required = true;
}

static void sensors_task(void *pvParameters)
{
  LOG_DEBUG("SENSORS TASK STARTED\r\n");

  fe_flight_mode_t initial_flight_mode;

  if (flight_control_get_mode(&initial_flight_mode) != FLN_OK) {
    LOG_ERROR("error getting flight mode\r\n");
    return;
  }

  flight_control_set_mode(FE_FLIGHT_MODE_CALIBRATING);

  //FLN_ERR_CHECK(fbaro_calibrate());
  //FLN_ERR_CHECK(fimu_start(imu_config));

  flight_control_set_mode(initial_flight_mode);

  rtos_delay_ms(200);

  BaseType_t sensorNotification;

  while (1) {
    if (calibration_required) {
      calibrate();
    }
    /* Wait to be notified of an interrupt. */
   // sensorNotification = xTaskNotifyWait(pdFALSE,
   //                                      0xFFFFFFFF,
   //                                      NULL,
   //                                      MS_TO_TICKS(10));

   //
    sensorNotification = pdPASS;
    if (sensorNotification == pdPASS) {
      //flight_control_set_sensor_data(gyro_data, accel_data, quat_data, alt_data);
      LOG_DEBUG("get data\r\n");
      imu_get_data();
      rtos_delay_ms(100);
    }
    else {
      LOG_DEBUG("sensor notification not received\r\n");
      error_handler();
    }
  }
}

void sensors_task_setup(void)
{
  FLN_ERR_CHECK(i2c_init());
  FLN_ERR_CHECK(imu_init());
  //FLN_ERR_CHECK(fbaro_init(&baro_config));
  //bsp_imu_int_init(imu_data_ready_cb);
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
