#ifndef UART_H
#define UART_H

#include <stdint.h>

#define UART_TX_BUFFER_SIZE  512
#define UART_RX_BUFFER_SIZE  512

void uart_init(void);
void uart_write(uint8_t *data, uint32_t length);


#endif  // UART_H
