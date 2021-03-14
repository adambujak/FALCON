#include "logger.h"

#include "bsp.h"
#include "falcon_common.h"

#include <string.h>

fln_uart_handle_t UartHandle;

#define UART_TX_BUFFER_SIZE  512

void logger_write(char *string)
{
  size_t length = strlen(string);

  if (length > UART_TX_BUFFER_SIZE) {
    string[length-1] = '\0';
  }

  bsp_uart_write(&UartHandle, (uint8_t *)string, (uint16_t)length);
}

void logger_init(void)
{
  #ifdef DEBUG
  FLN_ERR_CHECK(bsp_uart_init(&UartHandle));
  #endif
}
