#include "bsp.h"
#include "falcon_common.h"

#include "leds.h"
#include "logger.h"

#include <stdint.h>

#define led_TASK_PRIORITY    (tskIDLE_PRIORITY + 2)
#define logger_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

static void SystemClock_Config(void);

int main(void)
{
  bsp_board_bringup();
  leds_task_setup();
  logger_task_setup();

  int32_t taskStatus;
  taskStatus = xTaskCreate(leds_task,
                           "led_task",
                            configMINIMAL_STACK_SIZE,
                            NULL,
                            led_TASK_PRIORITY,
                            NULL);

  RTOS_ERR_CHECK(taskStatus);

  taskStatus = xTaskCreate(logger_task,
                           "logger_task",
                           configMINIMAL_STACK_SIZE,
                           NULL,
                           logger_TASK_PRIORITY,
                           NULL);

  RTOS_ERR_CHECK(taskStatus);

  vTaskStartScheduler();

  /* Should never reach here */
  while (1);
}