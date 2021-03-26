#include "falcon_common.h"
#include "uart.h"
#include "logger.h"

#include "board.h"

#ifndef NVIC_PRIORITYGROUP_4
#define NVIC_PRIORITYGROUP_4  ((uint32_t)0x00000003)
#endif

void sysclk_init(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_3)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_EnableBypass();
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 384, LL_RCC_PLLP_DIV_4);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(96000000);
  LL_SetSystemCoreClock(96000000);
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

static void gpio_init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_14);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void board_bringup(void)
{
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  sysclk_init();

  gpio_init();
  uart_init();
}

static void main_task(void *pvParameters)
{
  uint32_t last_tick = xTaskGetTickCount();
  uint32_t now = xTaskGetTickCount();

  while (1) {
    now = xTaskGetTickCount();
    if ((now - last_tick) > 2000) {
      LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_14);
      last_tick = now;
      LOG_DEBUG("hello\r\n");
    }
  }
}

int main(void)
{
  board_bringup();

  int32_t taskStatus;

  taskStatus = xTaskCreate(main_task,
                        "main_task",
                        configMINIMAL_STACK_SIZE,
                        NULL,
                        tskIDLE_PRIORITY + 1,
                        NULL);

  ASSERT(taskStatus == pdTRUE);

  vTaskStartScheduler();
  /* Should never reach here */
  while (1);
}

void error_handler(void)
{
  LOG_ERROR("Error Handler\r\n");
  while (1) {
    LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_14);
    for (uint32_t i = 0; i < 1000000; i++);
  }
}