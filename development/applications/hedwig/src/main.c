#include "bsp.h"
#include "falcon_common.h"
#include "stm32f4xx_it.h"

#include "leds.h"
#include "logger.h"
#include "motors.h"
#include "device_com.h"
#include "sensors.h"
#include "flight_control.h"

#include <stdint.h>

#define INCLUDE_LEDS 0
#define INCLUDE_MOTORS 0
#define INCLUDE_SENSORS 1
#define INCLUDE_DEVICE_COM 1
#define INCLUDE_FLIGHT_CONTROL 1

static uint8_t startedOS= 0;

static void startOS(void)
{
  startedOS = 1;
  vTaskStartScheduler();
}

void hedwig_delay(uint32_t ms)
{
  if (startedOS) {
    vTaskDelay(MS_TO_TICKS(ms));
  }
  else {
    HAL_Delay(ms);
  }
}

void hedwig_sysTickHandler(void)
{
  if (startedOS) {
    OSSysTickHandler();
  }
}

int main(void)
{
  int32_t taskStatus;
  bsp_board_bringup();

  logger_init();

  DEBUG_LOG("Hedwig Started \r\n");

  #if INCLUDE_MOTORS
  motors_init();
  #endif
  #if INCLUDE_LEDS
  leds_task_setup();
  #endif
  #if INCLUDE_SENSORS
  sensors_task_setup();
  #endif
  #if INCLUDE_DEVICE_COM
  device_com_setup();
  #endif
  #if INCLUDE_FLIGHT_CONTROL
  flight_control_setup();
  #endif

  #if INCLUDE_LEDS
  leds_task_tart();
  #endif
  #if INCLUDE_DEVICE_COM
  device_com_start();
  #endif
  #if INCLUDE_SENSORS
  sensors_task_start();
  #endif
  #if INCLUDE_FLIGHT_CONTROL
  flight_control_task_start();
  #endif

  startOS();

  /* Should never reach here */
  while (1);
}
