#ifndef SPI_H
#define SPI_H

#include <stdint.h>

typedef void (*exti_cb_t)(void);

void spi_init(void);
int spi_transfer(uint8_t *tx_buf, uint32_t tx_len,
              uint8_t *rx_buf, uint32_t rx_len);

#endif  // SPI_H
