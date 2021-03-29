#include "spi.h"

#include "falcon_common.h"
#include "board.h"

#include <stdint.h>
#include <stdbool.h>

#define SPI_ERROR_MASK (LL_SPI_SR_MODF | LL_SPI_SR_OVR | LL_SPI_SR_FRE)

static bool error_detected(SPI_TypeDef *spi)
{
  if ((spi->SR & SPI_ERROR_MASK) != 0) {
    return true;
  }
  return false;
}

static void hw_init(void)
{
  LL_SPI_InitTypeDef spi_config = {0};
  LL_GPIO_InitTypeDef gpio_config = {0};

  RF_SPI_GPIO_CLK_EN();

  gpio_config.Pin = RF_SPI_SCK_PIN | RF_SPI_MISO_PIN | RF_SPI_MOSI_PIN;
  gpio_config.Mode = LL_GPIO_MODE_ALTERNATE;
  gpio_config.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  gpio_config.Pull = LL_GPIO_PULL_NO;
  gpio_config.Alternate = RF_SPI_GPIO_AF;
  LL_GPIO_Init(RF_SPI_GPIO_PORT, &gpio_config);

  spi_config.TransferDirection = LL_SPI_FULL_DUPLEX;
  spi_config.Mode = LL_SPI_MODE_MASTER;
  spi_config.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  spi_config.ClockPolarity = LL_SPI_POLARITY_LOW;
  spi_config.ClockPhase = LL_SPI_PHASE_1EDGE;
  spi_config.NSS = LL_SPI_NSS_SOFT;
  spi_config.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV256;
  spi_config.BitOrder = LL_SPI_MSB_FIRST;
  spi_config.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  spi_config.CRCPoly = 10;
  LL_SPI_Init(RF_SPI, &spi_config);
  LL_SPI_SetStandard(RF_SPI, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_Enable(RF_SPI);
}

static int transfer_byte(uint8_t *tx_byte, uint8_t *rx_byte)
{
  LL_SPI_TransmitData8(RF_SPI, *tx_byte);

  while (!LL_SPI_IsActiveFlag_TXE(RF_SPI)) {
    if (error_detected(RF_SPI)) {
      return -1;
    }
  }

  if (rx_byte != NULL) {
    while (!LL_SPI_IsActiveFlag_RXNE(RF_SPI)) {
      if (error_detected(RF_SPI)) {
        return -1;
      }
    }
    *rx_byte = LL_SPI_ReceiveData8(RF_SPI);
  }
  return 0;
}

void spi_init(void)
{
  hw_init();
}

int spi_transfer(uint8_t *tx_buf, uint32_t tx_len,
              uint8_t *rx_buf, uint32_t rx_len)
{
  uint32_t bytes_transferred = 0;

  for (uint32_t i = 0; i < tx_len; i++) {
    if (transfer_byte(&tx_buf[i], &rx_buf[i]) == -1) {
      return -1;
    }
    bytes_transferred++;
  }

  uint8_t tx_byte = 0xFF;
  for (uint32_t i = bytes_transferred; i < rx_len; i++) {
    if (transfer_byte(&tx_byte, &rx_buf[i]) == -1) {
      return -1;
    }
    bytes_transferred++;
  }
  return 0;
}
