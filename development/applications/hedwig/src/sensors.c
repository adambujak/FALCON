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

volatile unsigned char new_gyro;
void gyro_data_ready_cb(void)
{
  new_gyro = 1;
}

static RT_MODEL rtM_;
static RT_MODEL *const rtMPtr = &rtM_; /* Real-time model */
static DW rtDW;                        /* Observable states */

/* '<Root>/Commands' */
static FCS_command_t rtU_Commands;

/* '<Root>/Sensors' */
static sensor_data_t rtU_Sensors;

/* '<Root>/State_Estim' */
static states_estimate_t rtY_State_Estim;

/* '<Root>/Throttle' */
static uint16_T rtY_Throttle[4];

void rt_OneStep(RT_MODEL *const rtM);
void rt_OneStep(RT_MODEL *const rtM)
{
  static boolean_T OverrunFlag = false;

  /* Disable interrupts here */

  /* Check for overrun */
  if (OverrunFlag) {
    return;
  }

  OverrunFlag = true;

  /* Save FPU context here (if necessary) */
  /* Re-enable timer or interrupt here */
  /* Set model inputs here */

  /* Step the model */
  flightController_step(rtM, &rtU_Commands, &rtU_Sensors, &rtY_State_Estim, rtY_Throttle);

  /* Get model outputs here */

  /* Indicate task complete */
  OverrunFlag = false;

  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}

static void FC_callback( TimerHandle_t xTimer )
{
  rt_OneStep(rtMPtr);
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
    .accel_fsr = MPU_FS_16G,
    .output_data_rate = IMU_SAMPLE_RATE
  };

  fbaro_config_t baro_config = {
    .i2cHandle = &i2cHandle,
    .chip_id = 0xC4,
    .sample_rate = BARO_SAMPLE_RATE
  };

  FLN_ERR_CHECK(fimu_init(IMU_config));

  FLN_ERR_CHECK(fbaro_init(&baro_config));

  // FLN_ERR_CHECK(fimu_calibrate_DMP());

  FLN_ERR_CHECK(fimu_calibrate_offset());

  FLN_ERR_CHECK(fbaro_calibrate());

  FLN_ERR_CHECK(fimu_start(IMU_config));

  // fimu_calibrate(gyro_bias, accel_bias, quat_bias);

  rtMPtr->dwork = &rtDW;

  /* Initialize model */
  flightController_initialize(rtMPtr, &rtU_Commands, &rtU_Sensors, &rtY_State_Estim, rtY_Throttle);

  TimerHandle_t FC_timer = xTimerCreate("FC_timer", 10, pdTRUE, 0, FC_callback);

  BaseType_t FC_timerStarted = xTimerStart( FC_timer, 0 );

  RTOS_ERR_CHECK(FC_timerStarted);

  bsp_IMU_int_init(gyro_data_ready_cb);

  while (1) {
    if (new_gyro == 1) {
      new_gyro = 0;
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

      DEBUG_LOG("z: %7.4f dz: %7.4f yaw, pitch, roll: %7.4f, %7.4f, %7.4f p, q, r: %7.4f, %7.4f, %7.4f\r\n",
                rtY_State_Estim.z,
                rtY_State_Estim.dz,
                rtY_State_Estim.yaw,
                rtY_State_Estim.pitch,
                rtY_State_Estim.roll,
                rtY_State_Estim.p,
                rtY_State_Estim.q,
                rtY_State_Estim.r);

    }
  }
}
