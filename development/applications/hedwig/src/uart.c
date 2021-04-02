#include "uart.h"
#include "board.h"

#include "falcon_common.h"

static UART_HandleTypeDef uart_instance;

int uart_init(void)
{
  GPIO_InitTypeDef gpio_config;

  UART_TX_GPIO_CLK_ENABLE();
  UART_RX_GPIO_CLK_ENABLE();

  UART_CLK_ENABLE();

  gpio_config.Pin       = UART_TX_PIN;
  gpio_config.Mode      = GPIO_MODE_AF_PP;
  gpio_config.Pull      = GPIO_PULLUP;
  gpio_config.Speed     = GPIO_SPEED_HIGH;
  gpio_config.Alternate = UART_TX_AF;

  HAL_GPIO_Init(UART_TX_GPIO_PORT, &gpio_config);

  gpio_config.Pin       = UART_RX_PIN;
  gpio_config.Alternate = UART_RX_AF;

  HAL_GPIO_Init(UART_RX_GPIO_PORT, &gpio_config);

  uart_instance.Instance = UART;

  uart_instance.Init.BaudRate     = UART_BAUDRATE;
  uart_instance.Init.WordLength   = UART_WORDLENGTH_8B;
  uart_instance.Init.StopBits     = UART_STOPBITS_1;
  uart_instance.Init.Parity       = UART_PARITY_NONE;
  uart_instance.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  uart_instance.Init.Mode         = UART_MODE_TX_RX;
  uart_instance.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&uart_instance) != HAL_OK) {
    return FLN_ERR;
  }
  return FLN_OK;
}

int uart_write(uint8_t *data, uint32_t length)
{
  if (HAL_UART_Transmit(&uart_instance, data, length, 0xFFFF) != HAL_OK) {
    return FLN_ERR;
  }
  return FLN_OK;
}

bool uart_is_writing(void)
{
  return false;
}
