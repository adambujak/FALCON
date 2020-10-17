#include "bsp.h"

static int SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef  ret = HAL_OK;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM       = 8;
  RCC_OscInitStruct.PLL.PLLN       = 200;
  RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ       = 7;
  RCC_OscInitStruct.PLL.PLLR       = 2;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if (ret != HAL_OK) {
    return FLN_ERR;
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
  if (ret != HAL_OK) {
    return FLN_ERR;
  }
}

void bsp_board_bringup(void)
{
  HAL_Init();

  FLN_ERR_CHECK(SystemClock_Config());
}

int bsp_rf_spi_init(void)
{
  return FLN_OK;
}

int bsp_leds_init(uint32_t ledsMask)
{
  LEDx_GPIO_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  GPIO_InitStruct.Pin = ledsMask;

  HAL_GPIO_Init(FLN_LED_PORT, &GPIO_InitStruct);

  return FLN_OK;
}

void bsp_leds_toggle(uint32_t ledsMask)
{
  HAL_GPIO_TogglePin(FLN_LED_PORT, ledsMask);
}

int bsp_uart_init(fln_uart_handle_t *handle)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();

  /* Enable USARTx clock */
  USARTx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = FLN_USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = FLN_USARTx_TX_AF;

  HAL_GPIO_Init(FLN_USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = FLN_USARTx_RX_PIN;
  GPIO_InitStruct.Alternate = FLN_USARTx_RX_AF;

  HAL_GPIO_Init(FLN_USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

  handle->Instance = FLN_UART;

  handle->Init.BaudRate     = 115200;
  handle->Init.WordLength   = UART_WORDLENGTH_9B;
  handle->Init.StopBits     = UART_STOPBITS_1;
  handle->Init.Parity       = UART_PARITY_ODD;
  handle->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  handle->Init.Mode         = UART_MODE_TX_RX;
  handle->Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(handle) != HAL_OK) {
    return FLN_ERR;
  }
  return FLN_OK;
}

void bsp_uart_write(fln_uart_handle_t *handle, uint8_t *data, uint16_t length)
{
  HAL_UART_Transmit(handle, data, length, 0xFFFF);
}

int bsp_i2c_init(fln_i2c_handle_t *handle)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable GPIO TX/RX clock */
  FLN_SENSORS_I2C_SCL_GPIO_CLK_ENABLE();
  FLN_SENSORS_I2C_SDA_GPIO_CLK_ENABLE();
  /* Enable I2Cx clock */
  FLN_SENSORS_I2C_CLK_ENABLE();

  FLN_SENSORS_I2C_FORCE_RESET();
  FLN_SENSORS_I2C_RELEASE_RESET();

  /* I2C TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = FLN_SENSORS_I2C_SCL_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = FLN_SENSORS_I2C_SCL_SDA_AF;
  HAL_GPIO_Init(FLN_SENSORS_I2C_SCL_GPIO_PORT, &GPIO_InitStruct);

  /* I2C RX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = FLN_SENSORS_I2C_SDA_PIN;
  GPIO_InitStruct.Alternate = FLN_SENSORS_I2C_SCL_SDA_AF;
  HAL_GPIO_Init(FLN_SENSORS_I2C_SDA_GPIO_PORT, &GPIO_InitStruct);

  handle->Instance             = FLN_SENSORS_I2C;
  handle->Init.ClockSpeed      = FLN_SENSORS_I2C_SPEEDCLOCK;
  handle->Init.DutyCycle       = FLN_SENSORS_I2C_DUTYCYCLE;
  handle->Init.OwnAddress1     = 0xFF;
  handle->Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  handle->Init.OwnAddress2     = 0xFF;
  handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  handle->Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

  if(HAL_I2C_Init(handle) != HAL_OK) {
    return FLN_ERR;
  }
  return FLN_OK;
}

int bsp_i2c_write(fln_i2c_handle_t *handle,
             uint8_t slave_addr,
             uint8_t reg_addr,
             uint16_t length,
             uint8_t *data)
{
  while(HAL_I2C_Mem_Write(handle, ((uint16_t)slave_addr<<1), reg_addr, 1U, data, length, 25U)) {
    if (HAL_I2C_GetError(handle) != HAL_I2C_ERROR_AF) {
      return FLN_ERR;
    }
  }
  return FLN_OK;
}

int bsp_i2c_read(fln_i2c_handle_t *handle,
             uint8_t slave_addr,
             uint8_t reg_addr,
             uint16_t length,
             uint8_t *data)
{
  while(HAL_I2C_Mem_Read(handle, ((uint16_t)slave_addr<<1), reg_addr, 1U, data, length, 25U)) {
    if (HAL_I2C_GetError(handle) != HAL_I2C_ERROR_AF) {
      return FLN_ERR;
    }
  }
  return FLN_OK;
}


void error_handler(void)
{
  while (1);
}

void vApplicationTickHook(void)
{
  HAL_IncTick();
}
