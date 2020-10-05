#include "bsp.h"

void bsp_board_bringup(void)
{
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