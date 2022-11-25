#include "flight_control.h"

#include "falcon_common.h"
#include "motors.h"
#include "falcon_packet.h"
#include "fp_encode.h"
#include "fp_decode.h"
#include "sensors.h"
#include "device_com.h"
#include <stdbool.h>

#include "flightController.h"
#include "rtwtypes.h"

#define FC_PERIOD_TICKS    MS_TO_TICKS(10)
#define RTOS_TIMEOUT_TICKS MS_TO_TICKS(5)

#define PRINT_PARAMS       1

static RT_MODEL rtM_;
static RT_MODEL *const rtM = &rtM_; /* Real-time model */
static DW rtDW;                     /* Observable states */

/* '<Root>/Commands' */
static FCS_command_t rtU_Commands;

/* '<Root>/Sensors' */
static sensor_data_t rtU_Sensors;

/* '<Root>/Bias' */
static sensor_bias_t rtU_Bias;

/* '<Root>/State_Estim' */
static states_estimate_t rtY_State_Estim;

/* '<Root>/Throttle' */
static uint16_T rtY_Throttle[4];

static ft_fcs_att_control_params_t att_control_params;
static ft_fcs_yaw_control_params_t yaw_control_params;
static ft_fcs_alt_control_params_t alt_control_params;

static bool spin_up_flag = false;
static int spin_up_counter = 0;

static TimerHandle_t flight_control_timer;
static BaseType_t FC_timerStatus;

static TaskHandle_t flight_control_task_handle = NULL;

static SemaphoreHandle_t sensorDataMutex;
static SemaphoreHandle_t commandDataMutex;
static SemaphoreHandle_t outputDataMutex;
static SemaphoreHandle_t modeMutex;

static fe_flight_mode_t flight_control_mode = FE_FLIGHT_MODE_IDLE;
static bool calibration_required = false;

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

static inline BaseType_t lock_output_data(void)
{
  return xSemaphoreTake(outputDataMutex, RTOS_TIMEOUT_TICKS);
}

static inline void unlock_output_data(void)
{
  xSemaphoreGive(outputDataMutex);
}

static inline BaseType_t lock_mode(void)
{
  return xSemaphoreTake(modeMutex, RTOS_TIMEOUT_TICKS);
}

static inline void unlock_mode(void)
{
  xSemaphoreGive(modeMutex);
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

static inline void createOutputDataMutex(void)
{
  outputDataMutex = xSemaphoreCreateMutex();
  if (outputDataMutex == NULL) {
    error_handler();
  }
}

static inline void createModeMutex(void)
{
  modeMutex = xSemaphoreCreateMutex();
  if (modeMutex == NULL) {
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
      if (lock_output_data() == pdTRUE) {

        flightController_step(rtM, &rtU_Commands, &rtU_Bias, &rtU_Sensors, &rtY_State_Estim, rtY_Throttle);
        if (flight_control_mode == FE_FLIGHT_MODE_FLY) {

          if (spin_up_flag) {
            for (int i = 0; i <= 3; i++) {
              if (rtY_Throttle[i] > (spin_up_counter * 10)) {
                rtY_Throttle[i] = spin_up_counter * 10;
              }
            }
          }
          motors_set_motor_us(MOTOR_1, rtY_Throttle[0]);
          motors_set_motor_us(MOTOR_2, rtY_Throttle[1]);
          motors_set_motor_us(MOTOR_3, rtY_Throttle[2]);
          motors_set_motor_us(MOTOR_4, rtY_Throttle[3]);
        }

        unlock_output_data();
      }
      else {
        LOG_WARN("outputDataMutex take failed\r\n");
        error_handler();
      }
      unlock_command_data();
    }
    else {
      LOG_WARN("commandDataMutex take failed\r\n");
      error_handler();
    }
    unlock_sensor_data();
  }
  else {
    LOG_WARN("sensorDataMutex take failed\r\n");
    error_handler();
  }

  /* Indicate task complete */
  OverrunFlag = false;

  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}

static void flight_control_callback(TimerHandle_t xTimer)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  xTaskNotifyFromISR(flight_control_task_handle,
                     0,
                     eSetBits,
                     &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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
    LOG_WARN("commandDataMutex take failed\r\n");
  }
}

static void save_controller_params(void)
{
  uint8_t changed = 0;

  changed |= PID_pitch_P != att_control_params.PID_pitch_P;
  changed |= PID_pitch_roll_I != att_control_params.PID_pitch_roll_I;
  changed |= PID_pitch_D != att_control_params.PID_pitch_D;

  changed |= PID_yaw_P != yaw_control_params.PID_yaw_P;
  changed |= PID_yaw_D != yaw_control_params.PID_yaw_D;

  changed |= PID_alt_P != alt_control_params.PID_alt_P;
  changed |= PID_alt_D != alt_control_params.PID_alt_D;
  changed |= Alt_Hover_Const != alt_control_params.Alt_Hover_Const;

  att_control_params.PID_pitch_P = PID_pitch_P;
  att_control_params.PID_pitch_roll_I = PID_pitch_roll_I;
  att_control_params.PID_pitch_D = PID_pitch_D;

  yaw_control_params.PID_yaw_P = PID_yaw_P;
  yaw_control_params.PID_yaw_D = PID_yaw_D;

  alt_control_params.PID_alt_P = PID_alt_P;
  alt_control_params.PID_alt_D = PID_alt_D;
  alt_control_params.Alt_Hover_Const = Alt_Hover_Const;

  if (changed) {
    persistent_data_controller_params_set(&att_control_params, &yaw_control_params, &alt_control_params);
    persistent_data_write();
  }
}

static bool load_controller_params(void)
{
  if (persistent_data_controller_params_get(&att_control_params, &yaw_control_params, &alt_control_params)) {
    PID_pitch_P = att_control_params.PID_pitch_P;
    PID_pitch_roll_I = att_control_params.PID_pitch_roll_I;
    PID_pitch_D = att_control_params.PID_pitch_D;

    PID_yaw_P = yaw_control_params.PID_yaw_P;
    PID_yaw_D = yaw_control_params.PID_yaw_D;

    PID_alt_P = alt_control_params.PID_alt_P;
    PID_alt_D = alt_control_params.PID_alt_D;
    Alt_Hover_Const = alt_control_params.Alt_Hover_Const;

    return true;
  }
  else {
    return false;
  }
}

static void send_params_back(void)
{
  uint8_t buffer[MAX_PACKET_SIZE];
  uint8_t length = fpc_attitude_params_encode(buffer, &att_control_params);
  device_com_send_packet(buffer, length);

  length = fpc_yaw_params_encode(buffer, &yaw_control_params);
  device_com_send_packet(buffer, length);

  length = fpc_alt_params_encode(buffer, &alt_control_params);
  device_com_send_packet(buffer, length);
}

void flight_control_set_controller_params(uint8_t *data, fp_type_t packetType)
{
  if (flight_control_mode <= FE_FLIGHT_MODE_FCS_READY) {
    if(lock_command_data() == pdTRUE) {
      switch (packetType) {
        case FPT_ATTITUDE_PARAMS_COMMAND: {
          fpc_attitude_params_t attParams = {};
          fpc_attitude_params_decode(data, &attParams);
          PID_pitch_P = attParams.fcsAttParams.PID_pitch_P;
          PID_pitch_roll_I = attParams.fcsAttParams.PID_pitch_roll_I;
          PID_pitch_D = attParams.fcsAttParams.PID_pitch_D;
        } break;
        case FPT_YAW_PARAMS_COMMAND: {
          fpc_yaw_params_t yawParams = {};
          fpc_yaw_params_decode(data, &yawParams);
          PID_yaw_P = yawParams.fcsYawParams.PID_yaw_P;
          PID_yaw_D = yawParams.fcsYawParams.PID_yaw_D;
        } break;
        case FPT_ALT_PARAMS_COMMAND: {
          fpc_alt_params_t altParams = {};
          fpc_alt_params_decode(data, &altParams);
          PID_alt_P = altParams.fcsAltParams.PID_alt_P;
          PID_alt_D = altParams.fcsAltParams.PID_alt_D;
          Alt_Hover_Const = altParams.fcsAltParams.Alt_Hover_Const;
        } break;
        default:
          break;
      }
      save_controller_params();
      unlock_command_data();
    }
    else {
      LOG_WARN("commandDataMutex take failed\r\n");
    }
  }
  else {
    send_params_back();
  }
}

void flight_control_get_outputs(fpr_status_t *status_response)
{
  if (lock_mode() == pdTRUE) {
    status_response->status.mode = flight_control_mode;
    unlock_mode();
  }
  else {
    LOG_WARN("modeMutex take failed\r\n");
  }
  if(lock_output_data() == pdTRUE) {
    status_response->status.motor.motor1 = rtY_Throttle[0];
    status_response->status.motor.motor2 = rtY_Throttle[1];
    status_response->status.motor.motor3 = rtY_Throttle[2];
    status_response->status.motor.motor4 = rtY_Throttle[3];
    status_response->status.states.z = rtY_State_Estim.z;
    status_response->status.states.dz = rtY_State_Estim.dz;
    status_response->status.states.yaw = rtY_State_Estim.yaw;
    status_response->status.states.pitch = rtY_State_Estim.pitch;
    status_response->status.states.roll = rtY_State_Estim.roll;
    status_response->status.states.p = rtY_State_Estim.p;
    status_response->status.states.q = rtY_State_Estim.q;
    status_response->status.states.r = rtY_State_Estim.r;
    unlock_output_data();
  }
  else {
    LOG_WARN("commandDataMutex take failed\r\n");
  }
}

static void flight_control_reset(void)
{
  if (flight_control_mode != FE_FLIGHT_MODE_FLY) {
    if (lock_sensor_data() == pdTRUE ) {
      if (lock_command_data() == pdTRUE) {
        if (lock_output_data() == pdTRUE) {
          flightController_initialize(rtM, &rtU_Commands, &rtU_Bias, &rtU_Sensors, &rtY_State_Estim, rtY_Throttle);
          unlock_output_data();
        }
        else {
          LOG_WARN("outputDataMutex take failed\r\n");
          error_handler();
        }
        unlock_command_data();
      }
      else {
        LOG_WARN("commandDataMutex take failed\r\n");
        error_handler();
      }
      unlock_sensor_data();
    }
    else {
      LOG_WARN("sensorDataMutex take failed\r\n");
      error_handler();
    }
  }
  else {
    LOG_DEBUG("Cannot reset while flying");
  }
}

int flight_control_set_mode(fe_flight_mode_t new_mode)
{
  switch (new_mode) {
    case FE_FLIGHT_MODE_IDLE:
      if (flight_control_mode != FE_FLIGHT_MODE_CALIBRATING) {
        motors_off();
        flight_control_mode = FE_FLIGHT_MODE_IDLE;
        return FLN_OK;
      }
      break;
    case FE_FLIGHT_MODE_CALIBRATING:
      if (flight_control_mode <= FE_FLIGHT_MODE_FCS_READY) {
        flight_control_mode = FE_FLIGHT_MODE_CALIBRATING;
        return FLN_OK;
      }
      break;
    case FE_FLIGHT_MODE_FCS_READY:
      if (flight_control_mode == FE_FLIGHT_MODE_FLY) {
        motors_off();
        flight_control_mode = FE_FLIGHT_MODE_FCS_READY;
        return FLN_OK;
      }
      else {
        flight_control_mode = FE_FLIGHT_MODE_FCS_READY;
        return FLN_OK;
      }
      break;
    case FE_FLIGHT_MODE_FLY:
      if (flight_control_mode == FE_FLIGHT_MODE_FCS_READY) {
        flight_control_reset();
        flight_control_mode = FE_FLIGHT_MODE_FLY;
        spin_up_flag = true;
        return FLN_OK;
      }
      break;
  }
  return FLN_ERR;
}

int flight_control_get_mode(fe_flight_mode_t *mode)
{
  if (lock_mode() == pdTRUE) {
    *mode = flight_control_mode;
    unlock_mode();
    return FLN_OK;
  }
  return FLN_ERR;
}

void flight_control_calibrate_sensors(void)
{
  calibration_required = true;
}

static fe_calib_request_t calibrate_sensors(void)
{
  fe_flight_mode_t prevMode = flight_control_mode;
  if (flight_control_set_mode(FE_FLIGHT_MODE_CALIBRATING) == FLN_OK) {
    sensors_calibrate();
    rtos_delay_ms(3000);
    flight_control_reset();
    flight_control_set_mode(prevMode);
    return FE_CALIBRATE_SUCCESS;
  }
  return FE_CALIBRATE_FAILED;
}

static void flight_control_task(void *pvParameters)
{

  PID_alt_P = 0;
  PID_alt_D = 0;

  PID_pitch_P = 3.6;
  PID_pitch_roll_I = 0.2;
  PID_pitch_D = 0.18;

  PID_yaw_P = 0;
  PID_yaw_D = 0;

  if (load_controller_params()) {
    send_params_back();
  }

  FC_timerStatus = xTimerStart( flight_control_timer, 0 );
  RTOS_ERR_CHECK(FC_timerStatus);

  BaseType_t flightTimerNotification;

  while(1)
  {
    if (calibration_required) {
      fpr_calibrate_t response = {calibrate_sensors()};
      uint8_t buffer[MAX_PACKET_SIZE];
      uint8_t length = fpr_calibrate_encode(buffer, &response);
      device_com_send_packet(buffer, length);
      calibration_required = false;
    }

    if (flight_control_mode >= FE_FLIGHT_MODE_FCS_READY) {
      /* Wait to be notified of an interrupt. */
      flightTimerNotification = xTaskNotifyWait(pdFALSE,
                                           0xFFFFFFFF,
                                           NULL,
                                           MS_TO_TICKS(11));

      if (flightTimerNotification == pdPASS) {

        rt_OneStep(rtM);

        if (spin_up_flag == true) {
          if (spin_up_counter <= 100) {
            spin_up_counter++;
          }
          else {
            spin_up_counter = 0;
            spin_up_flag = false;
          }
        }

#if (PRINT_PARAMS == 1)
        LOG_DEBUG("z: %7.4f dz: %7.4f yaw, pitch, roll: %7.4f, %7.4f, %7.4f p, q, r: %7.4f, %7.4f, %7.4f motors: %u, %u, %u, %u ALT_PD: %7.4f, %7.4f, %7.4f ATT_PID: %7.4f, %7.4f, %7.4f YAW_PD: %7.4f, %7.4f\r\n", rtY_State_Estim.z,
            rtY_State_Estim.dz,
            rtY_State_Estim.yaw,
            rtY_State_Estim.pitch,
            rtY_State_Estim.roll,
            rtY_State_Estim.p,
            rtY_State_Estim.q,
            rtY_State_Estim.r,
            rtY_Throttle[0],
            rtY_Throttle[1],
            rtY_Throttle[2],
            rtY_Throttle[3],
            PID_alt_P,
            PID_alt_D,
            Alt_Hover_Const,
            PID_pitch_P,
            PID_pitch_roll_I,
            PID_pitch_D,
            PID_yaw_P,
            PID_yaw_D);
#endif
      }
      else {
        LOG_DEBUG("timer notif not received\r\n");
        error_handler();
      }
    }
    else {
      rtos_delay_ms(1);
    }
  }
}

void flight_control_setup(void)
{
  /* Pack model data into RTM */
  rtM->dwork = &rtDW;

  /* Initialize model */
  flightController_initialize(rtM, &rtU_Commands, &rtU_Bias, &rtU_Sensors, &rtY_State_Estim, rtY_Throttle);

  flight_control_timer = xTimerCreate("flight_control_timer", FC_PERIOD_TICKS, pdTRUE, 0, flight_control_callback);

  createSensorDataMutex();
  createCommandDataMutex();
  createOutputDataMutex();
  createModeMutex();
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
