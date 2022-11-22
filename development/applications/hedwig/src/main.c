#include "falcon_common.h"

#include "board.h"
#include "stm32f4xx_it.h"
#include "i2c.h"
#include "uart.h"

#include "device_com.h"
#include "flight_control.h"
#include "leds.h"
#include "motors.h"
#include "sensors.h"
#include "system_time.h"
#include "persistent_data.h"

#include <stdint.h>

#define PRIORITYGROUP          ((uint32_t)0x00000003)

#define INCLUDE_LEDS           0
#define INCLUDE_MOTORS         1
#define INCLUDE_SENSORS        1
#define INCLUDE_DEVICE_COM     1
#define INCLUDE_FLIGHT_CONTROL 1

static uint8_t os_started = 0;

static void os_start(void)
{
  os_started = 1;
  vTaskStartScheduler();
}

void delay_us(uint32_t us)
{
  uint32_t start_time = system_time_get();
  while(system_time_cmp_us(start_time, system_time_get()) < us);
}

void delay_ms(uint32_t ms)
{
  uint32_t start_time = system_time_get();
  while(system_time_cmp_ms(start_time, system_time_get()) < ms);
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    error_handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
    error_handler();
  }
}

static void board_bringup(void)
{
  NVIC_SetPriorityGrouping(PRIORITYGROUP);

  HAL_Init();
  sysclk_init();

  uart_init();
  logger_init();
  i2c_init();
  system_time_init();
  persistent_data_init();
}

int main(void)
{
  board_bringup();
  LOG_DEBUG("Hedwig Started!\r\n");

#if INCLUDE_MOTORS
  motors_init();
#endif
#if INCLUDE_LEDS
  leds_task_setup();
#endif
#if INCLUDE_SENSORS
  sensors_task_setup();
#endif
#if INCLUDE_DEVICE_COM
  device_com_setup();
#endif
#if INCLUDE_FLIGHT_CONTROL
  flight_control_setup();
#endif

#if INCLUDE_LEDS
  leds_task_start();
#endif
#if INCLUDE_DEVICE_COM
 device_com_start();
#endif
#if INCLUDE_SENSORS
  sensors_task_start();
#endif
#if INCLUDE_FLIGHT_CONTROL
  flight_control_task_start();
#endif

  os_start();

  /* Should never reach here */
  while (1);
}

void hedwig_sysTickHandler(void)
{
  if (os_started) {
    OSSysTickHandler();
  }
}

void error_handler(void)
{
  motors_emergency_stop();
  LOG_ERROR("Error Handler\r\n");
  logger_process();
  vTaskSuspendAll();
  while (1);
}
