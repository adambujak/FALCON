#include "falcon_common.h"
#include "uart.h"
#include "gpio.h"
#include "spi.h"
#include "logger.h"
#include "device_com.h"
#include "system_time.h"

#include "board.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_tim.h"

#ifndef NVIC_PRIORITYGROUP_4
#define NVIC_PRIORITYGROUP_4  ((uint32_t)0x00000003)
#endif

void delay_us(uint32_t us) {
  uint32_t start_time = system_time_get();
  while(system_time_cmp_us(start_time, system_time_get()) < us);
}

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
  LL_Init1msTick(SYSCLK_FREQ);
  LL_SetSystemCoreClock(SYSCLK_FREQ);
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

static void led_pin_init(void)
{
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
  LL_GPIO_InitTypeDef gpio_config = {0};

  LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_0);
  gpio_config.Pin = LL_GPIO_PIN_0;
  gpio_config.Mode = LL_GPIO_MODE_OUTPUT;
  gpio_config.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  gpio_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  gpio_config.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOE, &gpio_config);
}

static void test_pin_init(void)
{
  LL_GPIO_InitTypeDef gpio_config = {0};

  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);
  gpio_config.Pin = LL_GPIO_PIN_1;
  gpio_config.Mode = LL_GPIO_MODE_OUTPUT;
  gpio_config.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  gpio_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  gpio_config.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOB, &gpio_config);
}

static void board_bringup(void)
{
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  sysclk_init();

  gpio_init();
  spi_init();
  uart_init();
  system_time_init();
}

int main(void)
{
  board_bringup();
  led_pin_init();
  test_pin_init();

  device_com_setup();
  device_com_start();

  vTaskStartScheduler();

  /* Should never reach here */
  while (1);
}

void error_handler(void)
{
  LOG_ERROR("Error Handler\r\n");
  while (1) {
    // ToDo: Blink LED
    LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_1);
    for (uint32_t i = 0; i < 1000000; i++);
  }
}
