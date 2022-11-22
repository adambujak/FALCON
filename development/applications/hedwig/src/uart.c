#include "uart.h"

#include "falcon_common.h"
#include "board.h"
#include <fifo.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define IS_POWER_OF_TWO(num) (((num) & ((num) - 1)) == 0) ? true : false

static uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static uint8_t tx_buffer[UART_TX_BUFFER_SIZE];

static fifo_t rx_fifo;
static fifo_t tx_fifo;

static bool writing = false;

static void hw_init(void)
{
  LL_USART_InitTypeDef uart_config = {0};
  LL_GPIO_InitTypeDef gpio_config = {0};

  UART_CLK_EN();
  GPIO_UART_CLK_EN();

  gpio_config.Pin = UART_RX_PIN | UART_TX_PIN;
  gpio_config.Mode = LL_GPIO_MODE_ALTERNATE;
  gpio_config.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  gpio_config.Pull = LL_GPIO_PULL_NO;
  gpio_config.Alternate = UART_GPIO_AF;
  LL_GPIO_Init(UART_GPIO_PORT, &gpio_config);

  uart_config.BaudRate = UART_BAUDRATE;
  uart_config.DataWidth = LL_USART_DATAWIDTH_8B;
  uart_config.StopBits = LL_USART_STOPBITS_1;
  uart_config.Parity = LL_USART_PARITY_NONE;
  uart_config.TransferDirection = LL_USART_DIRECTION_TX_RX;
  uart_config.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  uart_config.OverSampling = LL_USART_OVERSAMPLING_16;

  LL_USART_Init(UART, &uart_config);

  LL_USART_ConfigAsyncMode(UART);
  LL_USART_Enable(UART);

  LL_USART_EnableIT_RXNE(UART);
  LL_USART_EnableIT_TC(UART);

  NVIC_SetPriority(UART_IRQn, 0);
  NVIC_EnableIRQ(UART_IRQn);
}

static inline void tx(void)
{
  uint8_t write_byte;
  if (fifo_pop(&tx_fifo, &write_byte, 1) == 1) {
    LL_USART_TransmitData8(UART, write_byte);
    writing = true;
  }
}

bool uart_is_writing(void)
{
  return writing;
}

void uart_write(uint8_t *data, uint32_t length)
{
  DISABLE_IRQ();
  fifo_push(&tx_fifo, data, length);
  if (!writing) {
    tx();
  }
  ENABLE_IRQ();
}

int uart_read(uint8_t *data, uint32_t length)
{
  DISABLE_IRQ();
  int ret = fifo_pop(&rx_fifo, data, length);
  ENABLE_IRQ();
  return ret;
}

int uart_init(void)
{
  hw_init();
  FLN_ERR_CHECK(fifo_init(&rx_fifo, rx_buffer, UART_RX_BUFFER_SIZE));
  FLN_ERR_CHECK(fifo_init(&tx_fifo, tx_buffer, UART_TX_BUFFER_SIZE));
  uint8_t mychar = (uint8_t)'a';
  uart_write(&mychar, 1);
  return FLN_OK;
}

void UART_IRQHandler(void)
{
  DISABLE_IRQ();
  uint8_t data;
  if (LL_USART_IsActiveFlag_RXNE(UART)) {
    data = LL_USART_ReceiveData8(UART);
    LL_USART_ClearFlag_RXNE(UART);
    fifo_push(&rx_fifo, &data, 1);
  }
  if (LL_USART_IsActiveFlag_TC(UART)) {
    writing = false;
    LL_USART_ClearFlag_TC(UART);
    tx();
  }
  ENABLE_IRQ();
}
