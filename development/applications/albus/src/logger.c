#include "logger.h"
#include "uart.h"

#include <string.h>

void logger_write(char *string)
{
  size_t length = strlen(string);

  if (length > UART_TX_BUFFER_SIZE) {
    string[length-1] = '\0';
  }

  uart_write((uint8_t *)string, (uint32_t)length);
}