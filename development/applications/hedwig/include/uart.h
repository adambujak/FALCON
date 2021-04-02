#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

#define UART_TX_BUFFER_SIZE  512

int uart_init(void);
int uart_write(uint8_t *data, uint32_t length);
bool uart_is_writing(void);

#endif  // UART_H
