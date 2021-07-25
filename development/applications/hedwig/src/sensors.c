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
#include "attitude.h"
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

static void quat2ypr(float quat[4], float *RPY)
{
  RPY[0]  = atan2(2.0 * (quat[3] * quat[2] + quat[0] * quat[1]) , 1.0 - 2.0 * (quat[1] * quat[1] + quat[2] * quat[2]));
  RPY[1] = asin(2.0 * (quat[2] * quat[0] - quat[3] * quat[1]));
  RPY[2]   = atan2(2.0 * (quat[3] * quat[0] + quat[1] * quat[2]) , - 1.0 + 2.0 * (quat[0] * quat[0] + quat[1] * quat[1]));
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

  attitude_init_axis();
  
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

      attitude_compute_estimation(system_time_get(), accel_data, gyro_data, NULL);

      quat_data[0] = q.w;
      quat_data[1] = q.x;
      quat_data[2] = q.y;
      quat_data[3] = q.z;

      // LOG_INFO("%u  ", system_time_cmp_us(old_time, system_time_get()));
      LOG_DEBUG("RPY: %7.4f, %7.4f, %7.4f p, q, r: %7.4f, %7.4f, %7.4f accel: %7.4f, %7.4f, %7.4f alt: %7.4f \r\n",
            attitude.roll,
            attitude.pitch,
            attitude.yaw,            
            gyro_data[0],
            gyro_data[1],
            gyro_data[2],
            accel_data[0],
            accel_data[1],
            accel_data[2],
            alt_data);

      flight_control_set_sensor_data(gyro_data, accel_data, quat_data, alt_data);
      old_time = system_time_get();
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
