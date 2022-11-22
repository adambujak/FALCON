#include "logger.h"

#include "falcon_common.h"
#include "fifo.h"
#include "uart.h"

#include <string.h>

#if (LOG_MODE == LOG_MODE_ASYNC)
static uint8_t logger_buffer[LOG_BUFFER_SIZE];
static uint8_t logger_fifo_buffer[LOG_BUFFER_SIZE];
static fifo_t logger_fifo;
#endif // LOG_MODE_ASYNC

void logger_write(char *string)
{
  size_t length = strlen(string);

  if (length > UART_TX_BUFFER_SIZE) {
    string[length-1] = '\0';
  }
#if (LOG_MODE == LOG_MODE_BLOCKING)
  uart_write((uint8_t *)string, (uint32_t)length);
  while(uart_is_writing());
#endif // LOG_MODE_BLOCKING

#if (LOG_MODE == LOG_MODE_ASYNC)
  fifo_push(&logger_fifo, (uint8_t *)string, length);
#endif
}

void logger_process(void)
{
#if (LOG_MODE == LOG_MODE_ASYNC)
  uint32_t length = fifo_available(&logger_fifo);
  if (fifo_pop(&logger_fifo, logger_buffer, length) != length) {
    error_handler();
  }
  uart_write((uint8_t *)logger_buffer, (uint32_t)length);
#endif // LOG_MODE_ASYNC
}

void logger_init(void)
{
#if (LOG_MODE == LOG_MODE_ASYNC)
  FLN_ERR_CHECK(fifo_init(&logger_fifo, logger_fifo_buffer, LOG_BUFFER_SIZE));
#endif // LOG_MODE_ASYNC
}
