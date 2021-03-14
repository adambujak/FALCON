#include "bsp.h"
#include "falcon_common.h"
#include "stm32f4xx_it.h"

#include "leds.h"
#include "logger.h"
#include "motors.h"
#include "device_com.h"
#include "sensors.h"

#include <stdint.h>

#define INCLUDE_LEDS 0
#define INCLUDE_MOTORS 0
#define INCLUDE_SENSORS 0
#define INCLUDE_DEVICE_COM 1

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

  #if INCLUDE_LEDS
  taskStatus = xTaskCreate(leds_task,
                           "led_task",
                            2*configMINIMAL_STACK_SIZE,
                            NULL,
                            led_TASK_PRIORITY,
                            NULL);

  RTOS_ERR_CHECK(taskStatus);
  #endif

  #if INCLUDE_DEVICE_COM
  taskStatus = xTaskCreate(device_com_task,
                        "device_com_task",
                        4*configMINIMAL_STACK_SIZE,
                        NULL,
                        device_com_TASK_PRIORITY,
                        NULL);

  RTOS_ERR_CHECK(taskStatus);
  #endif

  #if INCLUDE_SENSORS
  taskStatus = xTaskCreate(sensors_task,
                          "sensors_task",
                          512,
                          NULL,
                          sensors_TASK_PRIORITY,
                          NULL);

  RTOS_ERR_CHECK(taskStatus);
  #endif

  startOS();

  /* Should never reach here */
  while (1);
}
