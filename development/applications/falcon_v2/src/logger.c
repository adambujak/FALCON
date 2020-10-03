#include "logger.h"

#include "falcon_common.h"
#include "stm32f4xx_hal.h"
#include "stm32412g_discovery.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF7_USART2
#define USARTx_RX_PIN                    GPIO_PIN_3
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF7_USART2

UART_HandleTypeDef UartHandle;

#define UART_TX_BUFFER_SIZE 128

typedef struct {
  uint8_t txBuffer[UART_TX_BUFFER_SIZE];
  uint16_t txByteCnt;
  uint16_t txReadIndex;
  uint16_t txWriteIndex;
} state_t;

static state_t s;

void logger_write (char *string)
{
  if (s.txByteCnt >= (UART_TX_BUFFER_SIZE - 1)) {
    return;
  }

  while (*string != '\0') {
    s.txBuffer[s.txWriteIndex] = *string;
    s.txWriteIndex = (s.txWriteIndex + 1) % UART_TX_BUFFER_SIZE;

    s.txByteCnt ++;
    string++;
    if (s.txByteCnt >= UART_TX_BUFFER_SIZE - 1) {
      break;
    }
  }

  s.txBuffer[s.txWriteIndex] = '\0';
}

void logger_task_setup (void)
{
  s.txByteCnt = 0;
  s.txWriteIndex = 0;
  s.txReadIndex = 0;

  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();


  /* Enable USARTx clock */
  USARTx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = USARTx_TX_AF;

  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USARTx_RX_PIN;
  GPIO_InitStruct.Alternate = USARTx_RX_AF;

  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);  

  UartHandle.Instance        = USARTx;

  UartHandle.Init.BaudRate   = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_9B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_ODD;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&UartHandle) != HAL_OK) {
    /* Initialization Error */
    while(1);
  }
}

void logger_task (void *pvParameters)
{
  while(1) {
    if (s.txByteCnt > 0) {
      HAL_UART_Transmit(&UartHandle, &(s.txBuffer[s.txReadIndex]), 1, 0xFFFF);
      s.txReadIndex= (s.txReadIndex + 1) % UART_TX_BUFFER_SIZE;
      s.txByteCnt--;
    }
  }
}
