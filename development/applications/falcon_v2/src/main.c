#include "bsp.h"
#include "falcon_common.h"

#include "leds.h"
#include "logger.h"
#include "motors.h"
#include "device_com.h"

#include <stdint.h>

#define led_TASK_PRIORITY    (tskIDLE_PRIORITY + 2)
#define logger_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define device_com_TASK_PRIORITY  (tskIDLE_PRIORITY + 3)

int main(void)
{
  int32_t taskStatus;
  bsp_board_bringup();
  logger_init();
  motors_init();

  leds_task_setup();
  taskStatus = xTaskCreate(leds_task,
                           "led_task",
                            2*configMINIMAL_STACK_SIZE,
                            NULL,
                            led_TASK_PRIORITY,
                            NULL);

  RTOS_ERR_CHECK(taskStatus);

  DEBUG_LOG("Falcon V2 Started \r\n");

  device_com_setup();
  taskStatus = xTaskCreate(device_com_task,
                        "device_com_task",
                        4*configMINIMAL_STACK_SIZE,
                        NULL,
                        device_com_TASK_PRIORITY,
                        NULL);

  RTOS_ERR_CHECK(taskStatus);


  vTaskStartScheduler();

  /* Should never reach here */
  while (1);
}
