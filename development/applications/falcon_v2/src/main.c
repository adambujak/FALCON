#include "bsp.h"
#include "falcon_common.h"
#include "stm32f4xx_it.h"

#include "leds.h"
#include "logger.h"
#include "motors.h"
#include "device_com.h"
#include "sensors.h"

#include <stdint.h>

#define led_TASK_PRIORITY    (tskIDLE_PRIORITY + 2)
#define device_com_TASK_PRIORITY  (tskIDLE_PRIORITY + 3)
#define sensors_TASK_PRIORITY (tskIDLE_PRIORITY + 4)

int main(void)
{
  int32_t taskStatus;
  bsp_board_bringup();
  logger_init();
  motors_init();

  DEBUG_LOG("Falcon V2 Started \r\n");

  leds_task_setup();
  device_com_setup();
  sensors_task_setup();

  taskStatus = xTaskCreate(leds_task,
                           "led_task",
                            2*configMINIMAL_STACK_SIZE,
                            NULL,
                            led_TASK_PRIORITY,
                            NULL);

  RTOS_ERR_CHECK(taskStatus);

  taskStatus = xTaskCreate(device_com_task,
                        "device_com_task",
                        4*configMINIMAL_STACK_SIZE,
                        NULL,
                        device_com_TASK_PRIORITY,
                        NULL);

  RTOS_ERR_CHECK(taskStatus);

  taskStatus = xTaskCreate(sensors_task,
                          "sensors_task",
                          512,
                          NULL,
                          sensors_TASK_PRIORITY,
                          NULL);

  RTOS_ERR_CHECK(taskStatus);

  OSStarted();
  vTaskStartScheduler();

  /* Should never reach here */
  while (1);
}
