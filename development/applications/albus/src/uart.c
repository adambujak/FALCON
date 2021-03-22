#include "uart.h"

#include "bsp.h"
#include "falcon_common.h"

#include <string.h>

#define UART_TX_BUFFER_SIZE  512

void uart_log(char *string)
{
  size_t length = strlen(string);

  if (length > UART_TX_BUFFER_SIZE) {
    string[length-1] = '\0';
  }

  bsp_uart_write((uint8_t *)string, (uint16_t)length);
}

void uart_init(void)
{
  FLN_ERR_CHECK(bsp_uart_init());
}