#include "bsp.h"
#include "falcon_common.h"
#include "stm32f4xx_it.h"

#include "leds.h"
#include "logger.h"
#include "motors.h"

#include <stdint.h>

#define led_TASK_PRIORITY    (tskIDLE_PRIORITY + 2)
#define logger_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

int main(void)
{
  bsp_board_bringup();
  leds_task_setup();
  logger_init();
  motors_init();

  int32_t taskStatus;
  taskStatus = xTaskCreate(leds_task,
                           "led_task",
                            2*configMINIMAL_STACK_SIZE,
                            NULL,
                            led_TASK_PRIORITY,
                            NULL);

  RTOS_ERR_CHECK(taskStatus);

  OSStarted();
  vTaskStartScheduler();

  /* Should never reach here */
  while (1);
}
