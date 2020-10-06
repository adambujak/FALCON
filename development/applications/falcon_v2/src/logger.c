#include "logger.h"

#include "bsp.h"
#include "falcon_common.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

fln_uart_handle_t UartHandle;

#define UART_TX_BUFFER_SIZE  128
#define MUTEX_RETRY_TICK_CNT 10

typedef struct {
  uint8_t  txBuffer[UART_TX_BUFFER_SIZE];
  uint16_t txByteCnt;
  uint16_t txReadIndex;
  uint16_t txWriteIndex;

  SemaphoreHandle_t rxBufferMutex;
} state_t;

static state_t s;

void logger_write(char *string)
{
  while (xSemaphoreTake(s.rxBufferMutex, (TickType_t) MUTEX_RETRY_TICK_CNT) == pdFALSE);

  if (s.txByteCnt >= (UART_TX_BUFFER_SIZE - 1)) {
    return;
  }

  while (*string != '\0') {
    s.txBuffer[s.txWriteIndex] = *string;
    s.txWriteIndex = (s.txWriteIndex + 1) % UART_TX_BUFFER_SIZE;

    s.txByteCnt++;
    string++;
    if (s.txByteCnt >= UART_TX_BUFFER_SIZE - 1) {
      break;
    }
  }

  s.txBuffer[s.txWriteIndex] = '\0';
  xSemaphoreGive(s.rxBufferMutex);
}

void logger_task_setup(void)
{
  s.txByteCnt = 0;
  s.txWriteIndex = 0;
  s.txReadIndex = 0;
  s.rxBufferMutex = xSemaphoreCreateMutex();

  if (s.rxBufferMutex == NULL) {
    error_handler();
  }

  FLN_ERR_CHECK(bsp_uart_init(&UartHandle));
}

void logger_task(void *pvParameters)
{
  while(1) {
    if (xSemaphoreTake(s.rxBufferMutex, (TickType_t) MUTEX_RETRY_TICK_CNT) == pdFALSE) {
      continue;
    }

    if (s.txByteCnt > 0) {
      bsp_uart_write(&UartHandle, &(s.txBuffer[s.txReadIndex]), 1);
      s.txReadIndex = (s.txReadIndex + 1) % UART_TX_BUFFER_SIZE;
      s.txByteCnt--;
    }
    xSemaphoreGive(s.rxBufferMutex);
  }
}
