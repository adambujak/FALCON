#include "leds.h"

#include "falcon_common.h"
#include "stm32f4xx_hal.h"
#include "stm32412g_discovery.h"

void led_task_setup (void)
{
  LEDx_GPIO_CLK_ENABLE();

  GPIO_InitTypeDef  GPIO_InitStruct;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  GPIO_InitStruct.Pin = LED1_PIN | LED2_PIN;
  HAL_GPIO_Init(LEDx_GPIO_PORT, &GPIO_InitStruct);
}

void led_task (void *pvParameters)
{
  while(1) {
    HAL_GPIO_TogglePin(LEDx_GPIO_PORT, LED1_PIN);
    vTaskDelay(500);
    HAL_GPIO_TogglePin(LEDx_GPIO_PORT, LED2_PIN);
  }
}


