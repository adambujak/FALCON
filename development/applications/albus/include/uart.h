#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>

void uart_init(void);
void uart_log(char *string);
int  uart_process(void);

#endif  // LOGGER_H
