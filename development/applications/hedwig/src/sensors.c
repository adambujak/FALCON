#include "sensors.h"

#include <stdbool.h>

#include "baro.h"
#include "falcon_common.h"
#include "flightController.h"
#include "flight_control.h"
#include "i2c.h"
#include "imu.h"
#include "bsp.h"
#include "rtwtypes.h"
#include "system_time.h"
#include "persistent_data.h"

#define IMU_SAMPLE_RATE  (200) // Hz
#define BARO_SAMPLE_RATE (10)  // Hz

#define IMU_SAMPLE_PERIOD  (1000 / IMU_SAMPLE_RATE)
#define BARO_SAMPLE_PERIOD (1000 / BARO_SAMPLE_RATE)

#define IMU_SAMPLE_TIMEOUT (IMU_SAMPLE_PERIOD + (IMU_SAMPLE_PERIOD / 10))
#define BARO_SKIP_COUNT  (IMU_SAMPLE_RATE / BARO_SAMPLE_RATE)

static TaskHandle_t sensors_task_handle = NULL;

static float gyro_data[3] = {0.0F, 0.0F, 0.0F};
static float accel_data[3] = {0.0F, 0.0F, 0.0F};
static float mag_data[3] = {0.0F, 0.0F, 0.0F};
static float quat_data[4] = {1.0F, 0.0F, 0.0F, 0.0F};
static float alt_data = 0.0F;

static bool calibration_required = false;

void sensors_imu_int_cb(void)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  xTaskNotifyFromISR(sensors_task_handle,
                     0,
                     eSetBits,
                     &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void imu_calibration_complete_cb(void)
{
  float gyro_bias[3];
  float accel_bias[3];

  imu_get_bias(accel_bias, gyro_bias);

  persistent_data_imu_bias_set(accel_bias, gyro_bias);
  persistent_data_write();
}

static void calibrate(void)
{
  FLN_ERR_CHECK(baro_calibrate());
  FLN_ERR_CHECK(imu_start_calibration(imu_calibration_complete_cb));

  calibration_required = false;
}

void sensors_calibrate(void)
{
  calibration_required = true;
}

static void sensors_task(void *pvParameters)
{
  LOG_DEBUG("SENSORS TASK STARTED\r\n");

  fe_flight_mode_t initial_flight_mode;
  if (flight_control_get_mode(&initial_flight_mode) == FLN_OK) {

    flight_control_set_mode(FE_FLIGHT_MODE_CALIBRATING);
    FLN_ERR_CHECK(baro_calibrate());
    flight_control_set_mode(initial_flight_mode);
  } else {
    LOG_ERROR("error getting flight mode\r\n");
    return;
  }
  LOG_DEBUG("Sensors Calibrated\r\n");

  rtos_delay_ms(200);

  BaseType_t sensor_notification;

  uint32_t baro_skip_count = 1;
  uint32_t old_time;
  while (1) {

    /* Wait to be notified of an interrupt. */
    sensor_notification = xTaskNotifyWait(pdFALSE,
                                         0xFFFFFFFF,
                                         NULL,
                                         MS_TO_TICKS(IMU_SAMPLE_TIMEOUT));

    if (sensor_notification == pdPASS) {

      FLN_ERR_CHECK(imu_get_data(accel_data, gyro_data, mag_data));

      if (baro_skip_count++ >= BARO_SKIP_COUNT) {
        baro_get_altitude(&alt_data);
        baro_skip_count = 1;
      }
      // LOG_INFO("%u  ", system_time_cmp_us(old_time, system_time_get()));
      old_time = system_time_get();
      // LOG_INFO("\tp,q,r:\t %7.4f\t %7.4f\t %7.4f\t accel:\t %7.4f\t %7.4f\t %7.4f\t mag:\t %7.4f\t %7.4f\t %7.4f\t alt:\t %7.4f\t\r\n",        
      //       gyro_data[0],
      //       gyro_data[1],
      //       gyro_data[2],
      //       accel_data[0],
      //       accel_data[1],
      //       accel_data[2],
      //       mag_data[0],
      //       mag_data[1],
      //       mag_data[2],
      //       alt_data);

      flight_control_set_sensor_data(gyro_data, accel_data, quat_data, alt_data);
    }
    else {
      LOG_DEBUG("sensor notification not received\r\n");
      error_handler();
    }

    // Calibrate if requested
    if (calibration_required) {
      calibrate();
    }
  }
}

void sensors_task_setup(void)
{
  FLN_ERR_CHECK(imu_init(IMU_SAMPLE_PERIOD));
  FLN_ERR_CHECK(baro_init(BARO_SAMPLE_RATE));
  FLN_ERR_CHECK(bsp_imu_int_init(sensors_imu_int_cb));

  float accel_bias[3];
  float gyro_bias[3];
  
  if (persistent_data_imu_bias_get(accel_bias, gyro_bias)) {
    imu_set_bias(accel_bias, gyro_bias);  
  } 

  LOG_DEBUG("Sensors Initialized\r\n");
}

void sensors_task_start(void)
{
  BaseType_t task_status = xTaskCreate(sensors_task,
                                       "sensors_task",
                                       SENSORS_STACK_SIZE,
                                       NULL,
                                       sensors_TASK_PRIORITY,
                                       &sensors_task_handle);

  RTOS_ERR_CHECK(task_status);
}
