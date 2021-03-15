#include "flight_control.h"

#include "falcon_common.h"
#include "motors.h"
#include "falcon_packet.h"

#include "flightController.h"
#include "rtwtypes.h"

#define FC_PERIOD_TICKS    MS_TO_TICKS(10)
#define RTOS_TIMEOUT_TICKS MS_TO_TICKS(5)

static RT_MODEL rtM_;
static RT_MODEL *const rtM = &rtM_; /* Real-time model */
static DW rtDW;                     /* Observable states */

/* '<Root>/Commands' */
static FCS_command_t rtU_Commands;

/* '<Root>/Sensors' */
static sensor_data_t rtU_Sensors;

/* '<Root>/State_Estim' */
static states_estimate_t rtY_State_Estim;

/* '<Root>/Throttle' */
static uint16_T rtY_Throttle[4];

static TimerHandle_t flight_control_timer;
static BaseType_t FC_timerStatus;

static TaskHandle_t flight_control_task_handle = NULL;

static SemaphoreHandle_t sensorDataMutex;
static SemaphoreHandle_t commandDataMutex;

static inline BaseType_t lock_sensor_data(void)
{
  return xSemaphoreTake(sensorDataMutex, RTOS_TIMEOUT_TICKS);
}

static inline void unlock_sensor_data(void)
{
  xSemaphoreGive(sensorDataMutex);
}

static inline BaseType_t lock_command_data(void)
{
  return xSemaphoreTake(commandDataMutex, RTOS_TIMEOUT_TICKS);
}

static inline void unlock_command_data(void)
{
  xSemaphoreGive(commandDataMutex);
}

static inline void createSensorDataMutex(void)
{
  sensorDataMutex = xSemaphoreCreateMutex();
  if (sensorDataMutex == NULL) {
    error_handler();
  }
}

static inline void createCommandDataMutex(void)
{
  commandDataMutex = xSemaphoreCreateMutex();
  if (commandDataMutex == NULL) {
    error_handler();
  }
}

/*
 * Associating rt_OneStep with a real-time clock or interrupt service routine
 * is what makes the generated code "real-time".  The function rt_OneStep is
 * always associated with the base rate of the model.  Subrates are managed
 * by the base rate from inside the generated code.  Enabling/disabling
 * interrupts and floating point context switches are target specific.  This
 * example code indicates where these should take place relative to executing
 * the generated code step function.  Overrun behavior should be tailored to
 * your application needs.  This example simply sets an error status in the
 * real-time model and returns from rt_OneStep.
 */
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
  if (lock_sensor_data() == pdTRUE ) {
    if (lock_command_data() == pdTRUE) {
      flightController_step(rtM, &rtU_Commands, &rtU_Sensors, &rtY_State_Estim, rtY_Throttle);
      unlock_command_data();
    }
    else {
      DEBUG_LOG("commandDataMutex take failed\r\n");
      error_handler();
    }
    unlock_sensor_data();
  }
  else {
    LOG_DEBUG("sensorDataMutex take failed\r\n");
    error_handler();
  }

  /* Get model outputs here */
  // if(mode == motors_on) {
  //   motors_set_motor_us(MOTOR_1, rtY_Throttle[0]);
  //   motors_set_motor_us(MOTOR_2, rtY_Throttle[1]);
  //   motors_set_motor_us(MOTOR_3, rtY_Throttle[2]);
  //   motors_set_motor_us(MOTOR_4, rtY_Throttle[3]);
  // }

  /* Indicate task complete */
  OverrunFlag = false;

  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}

static void flight_control_callback(TimerHandle_t xTimer)
{
  rt_OneStep(rtM);

  LOG_DEBUG("z: %7.4f dz: %7.4f yaw, pitch, roll: %7.4f, %7.4f, %7.4f p, q, r: %7.4f, %7.4f, %7.4f\r\n",
            rtY_State_Estim.z, rtY_State_Estim.dz, rtY_State_Estim.yaw, rtY_State_Estim.pitch, rtY_State_Estim.roll,
            rtY_State_Estim.p, rtY_State_Estim.q, rtY_State_Estim.r);
}

void flight_control_set_sensor_data(float *gyro_data, float *accel_data, float *quat_data, float alt_data)
{
  if (lock_sensor_data() == pdTRUE) {
    memcpy(rtU_Sensors.gyro_data_SI, gyro_data, sizeof(rtU_Sensors.gyro_data_SI));
    memcpy(rtU_Sensors.accel_data_SI, accel_data, sizeof(rtU_Sensors.accel_data_SI));
    memcpy(rtU_Sensors.quat_data, quat_data, sizeof(rtU_Sensors.quat_data));
    rtU_Sensors.alt_data_SI = alt_data;
    unlock_sensor_data();
  }
  else {
    LOG_WARN("sensorDataMutex take failed\r\n");
  }
}

void flight_control_set_command_data(fpc_flight_control_t *control_input)
{
  if(lock_command_data() == pdTRUE) {
    rtU_Commands.control_input.yaw_cmd = control_input->fcsControlCmd.yaw;
    rtU_Commands.control_input.pitch_cmd = control_input->fcsControlCmd.pitch;
    rtU_Commands.control_input.roll_cmd = control_input->fcsControlCmd.roll;
    rtU_Commands.control_input.alt_cmd = control_input->fcsControlCmd.alt;
    unlock_command_data();
  }
  else {
    DEBUG_LOG("commandDataMutex take failed\r\n");
  }
}

void flight_control_start(void)
{
  FC_timerStatus = xTimerStart( flight_control_timer, 0 );
  RTOS_ERR_CHECK(FC_timerStatus);
}

// int flight_control_stop(void)
// {
//   if (somethingSomethingMode != flying) {
//     FC_timerStatus = xTimerStop( flight_control_timer, 0 );
//     RTOS_ERR_CHECK(FC_timerStatus);
//     return 0;
//   }
//   else {
//     LOG_DEBUG("NO STOP WHILE FLYING\r\n");
//     return 1;
//   }
// }

// int flight_control_reset(void)
// {
//   if (somethingSomethingMode != flying) {
//     if (somethingSomethingMode == running) {
//       flight_control_stop();
//     }

//     flightController_initialize(rtM, &rtU_Commands, &rtU_Sensors, &rtY_State_Estim, rtY_Throttle);
//     FC_timerStatus = xTimerReset( flight_control_timer, 0 );
//     RTOS_ERR_CHECK(FC_timerStatus);
//     return 0;
//   }
//   else {
//     LOG_DEBUG("NO RESET WHILE FLYING\r\n");
//     return 1;
//   }
// }

static void flight_control_task(void *pvParameters)
{

  vTaskDelay(10000);

  flight_control_start();

  while(1)
  {
    // getShitFromSensorsANDCom();
    vTaskDelay(2);
  }
}

void flight_control_setup(void)
{
  /* Pack model data into RTM */
  rtM->dwork = &rtDW;

  /* Initialize model */
  flightController_initialize(rtM, &rtU_Commands, &rtU_Sensors, &rtY_State_Estim, rtY_Throttle);

  flight_control_timer = xTimerCreate("flight_control_timer", FC_PERIOD_TICKS, pdTRUE, 0, flight_control_callback);

  createSensorDataMutex();
  createCommandDataMutex();
}

void flight_control_task_start(void)
{
  BaseType_t taskStatus = xTaskCreate(flight_control_task,
                          "flight control task",
                          FLIGHT_CONTROL_STACK_SIZE,
                          NULL,
                          flight_control_TASK_PRIORITY,
                          &flight_control_task_handle);

  RTOS_ERR_CHECK(taskStatus);
}
