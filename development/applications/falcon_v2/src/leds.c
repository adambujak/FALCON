#include "leds.h"

#include "logger.h"
#include "falcon_common.h"
#include "stm32f4xx_hal.h"
#include "stm32412g_discovery.h"

void leds_toggle (void)
{
  HAL_GPIO_TogglePin(LEDx_GPIO_PORT, LED3_PIN);
}

void led_task_setup (void)
{
  LEDx_GPIO_CLK_ENABLE();

  GPIO_InitTypeDef  GPIO_InitStruct;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  GPIO_InitStruct.Pin = LED1_PIN | LED2_PIN | LED3_PIN;
  HAL_GPIO_Init(LEDx_GPIO_PORT, &GPIO_InitStruct);
}

void led_task (void *pvParameters)
{
  while(1) {
    DEBUG_LOG("BLINKING LED\r\n");
    HAL_GPIO_TogglePin(LEDx_GPIO_PORT, LED1_PIN);
    vTaskDelay(500);
    HAL_GPIO_TogglePin(LEDx_GPIO_PORT, LED2_PIN);
  }
}


