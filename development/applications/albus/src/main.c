#include "bsp.h"
#include "falcon_common.h"
#include "stm32f4xx_it.h"

#include "uart.h"
#include "device_com.h"
#include "falcon_packet.h"
#include "ff_encoder.h"

#include <stdint.h>
#include <string.h>

#define logger_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define device_com_TASK_PRIORITY  (tskIDLE_PRIORITY + 2)

static uint8_t startedOS= 0;

static void startOS(void)
{
  startedOS = 1;
  vTaskStartScheduler();
}

void albus_delay(uint32_t ms)
{
  if (startedOS) {
    vTaskDelay(MS_TO_TICKS(ms));
  }
  else {
    HAL_Delay(ms);
  }
}

void albus_sysTickHandler(void)
{
  if (startedOS) {
    OSSysTickHandler();
  }
}

int main(void)
{
  int32_t taskStatus;
  bsp_board_bringup();
  uart_init();

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

  DEBUG_LOG("Starting tasks... \r\n");

  startOS();
  /* Should never reach here */
  while (1);
}
