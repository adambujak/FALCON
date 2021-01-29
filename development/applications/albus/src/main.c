#include "bsp.h"
#include "albus.h"
#include "stm32f4xx_it.h"

#include "logger.h"
#include "device_com.h"

#include <stdint.h>

#define logger_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define device_com_TASK_PRIORITY  (tskIDLE_PRIORITY + 2)

int main(void)
{
  int32_t taskStatus;
  bsp_board_bringup();
  logger_init();

  DEBUG_LOG("Albus Started\r\n");

  device_com_setup();

  BSP_LED_Init(LED1);
  BSP_LED_On(LED1);
  taskStatus = xTaskCreate(device_com_task,
                        "device_com_task",
                        4*configMINIMAL_STACK_SIZE,
                        NULL,
                        device_com_TASK_PRIORITY,
                        NULL);

  RTOS_ERR_CHECK(taskStatus);

  OSStarted();
  vTaskStartScheduler();

  /* Should never reach here */
  while (1);
}
