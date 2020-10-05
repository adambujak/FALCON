#include "logger.h"

#include "bsp.h"
#include "falcon_common.h"
#include "stm32f4xx_hal.h"
#include "stm32412g_discovery.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

fln_uart_handle_t UartHandle;

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

  FLN_ERR_CHECK(bsp_uart_init(&UartHandle));
}

void logger_task (void *pvParameters)
{
  while(1) {
    if (s.txByteCnt > 0) {
      bsp_uart_write(&UartHandle, &(s.txBuffer[s.txReadIndex]), 1);
      s.txReadIndex= (s.txReadIndex + 1) % UART_TX_BUFFER_SIZE;
      s.txByteCnt--;
    }
  }
}
