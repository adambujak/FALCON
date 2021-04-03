#ifndef SPI_H
#define SPI_H

#include <stdint.h>

typedef void (*exti_cb_t)(void);

void spi_init(void);
int spi_transfer(uint8_t *tx_buffer, uint16_t tx_size,
	             uint8_t *rx_buffer, uint16_t rx_size);

#endif  // SPI_H
