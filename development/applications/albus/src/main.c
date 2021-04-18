#include "falcon_common.h"
#include "board.h"
#include "uart.h"
#include "gpio.h"
#include "spi.h"
#include "logger.h"
#include "device_com.h"
#include "system_time.h"

#include <stdbool.h>

#define ERROR_LED_PORT GPIOE
#define ERROR_LED_PIN  LL_GPIO_PIN_0

#define PRIORITYGROUP  ((uint32_t)0x00000003)

static bool os_started = false;

void delay_us(uint32_t us)
{
  uint32_t start_time = system_time_get();
  while (system_time_cmp_us(start_time, system_time_get()) < us);
}

void delay_ms(uint32_t ms)
{
  uint32_t start_time = system_time_get();
  while (system_time_cmp_ms(start_time, system_time_get()) < ms);
}

void rtos_delay_ms(uint32_t ms)
{
  if (os_started) {
    vTaskDelay(ms);
  }
  else {
    delay_ms(ms);
  }
}

void sysclk_init(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);

  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_3);

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_EnableBypass();
  LL_RCC_HSE_Enable();

  while(LL_RCC_HSE_IsReady() != 1);

  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 384, LL_RCC_PLLP_DIV_4);
  LL_RCC_PLL_Enable();

  while(LL_RCC_PLL_IsReady() != 1);

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

  LL_Init1msTick(SYSCLK_FREQ);
  LL_SetSystemCoreClock(SYSCLK_FREQ);
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

static void board_bringup(void)
{
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(PRIORITYGROUP);

  sysclk_init();

  gpio_init();
  spi_init();
  uart_init();
  system_time_init();
}

static void os_start(void)
{
  os_started = true;
  vTaskStartScheduler();
}

int main(void)
{
  board_bringup();

  device_com_setup();
  device_com_start();

  LOG_DEBUG("Starting scheduler\r\n");

  os_start();

  /* Should never reach here */
  while (1);
}

void albus_sysTickHandler(void)
{
  if (os_started) {
    OSSysTickHandler();
  }
}

void error_handler(void)
{
  LOG_ERROR("Error Handler\r\n");
  while (1) {
    led_toggle();
    for (uint32_t i = 0; i < 1000000; i++);
  }
}
