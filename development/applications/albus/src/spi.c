#include "spi.h"

#include "falcon_common.h"
#include "board.h"

#include <stdint.h>
#include <stdbool.h>

#define max(a, b)    (a) > (b)?a:b

static SPI_HandleTypeDef spi_instance;

int hw_init(void)
{
  spi_instance.Instance = RF_SPI;
  spi_instance.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  spi_instance.Init.Direction         = SPI_DIRECTION_2LINES;
  spi_instance.Init.CLKPhase          = SPI_PHASE_1EDGE;
  spi_instance.Init.CLKPolarity       = SPI_POLARITY_LOW;
  spi_instance.Init.DataSize          = SPI_DATASIZE_8BIT;
  spi_instance.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  spi_instance.Init.TIMode            = SPI_TIMODE_DISABLE;
  spi_instance.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  spi_instance.Init.CRCPolynomial     = 7;
  spi_instance.Init.NSS  = SPI_NSS_SOFT;
  spi_instance.Init.Mode = SPI_MODE_MASTER;

  GPIO_InitTypeDef gpio_config;

  RF_SPI_SCK_GPIO_CLK_ENABLE();
  RF_SPI_MISO_GPIO_CLK_ENABLE();
  RF_SPI_MOSI_GPIO_CLK_ENABLE();
  RF_SPI_CLK_ENABLE();

  if (HAL_SPI_Init(&spi_instance) != HAL_OK) {
    /* Initialization Error */
    return FLN_ERR;
  }

  gpio_config.Pin       = RF_SPI_SCK_PIN;
  gpio_config.Mode      = GPIO_MODE_AF_PP;
  gpio_config.Pull      = GPIO_PULLDOWN;
  gpio_config.Speed     = GPIO_SPEED_HIGH;
  gpio_config.Alternate = RF_SPI_SCK_AF;
  HAL_GPIO_Init(RF_SPI_SCK_GPIO_PORT, &gpio_config);

  gpio_config.Pin       = RF_SPI_MISO_PIN;
  gpio_config.Alternate = RF_SPI_MISO_AF;
  HAL_GPIO_Init(RF_SPI_MISO_GPIO_PORT, &gpio_config);

  gpio_config.Pin       = RF_SPI_MOSI_PIN;
  gpio_config.Alternate = RF_SPI_MOSI_AF;
  HAL_GPIO_Init(RF_SPI_MOSI_GPIO_PORT, &gpio_config);

  return FLN_OK;
}

void bsp_rf_spi_deinit(void)
{
  if (spi_instance.Instance == RF_SPI) {
    RF_SPI_FORCE_RESET();
    RF_SPI_RELEASE_RESET();

    HAL_GPIO_DeInit(RF_SPI_SCK_GPIO_PORT, RF_SPI_SCK_PIN);
    HAL_GPIO_DeInit(RF_SPI_MISO_GPIO_PORT, RF_SPI_MISO_PIN);
    HAL_GPIO_DeInit(RF_SPI_MOSI_GPIO_PORT, RF_SPI_MOSI_PIN);
  }
}

void spi_init(void)
{
  if (hw_init() != FLN_OK) {
    error_handler();
  }
}

int spi_transfer(uint8_t *tx_buffer, uint16_t tx_size, uint8_t *rx_buffer, uint16_t rx_size)
{
  if (tx_size == 0 || tx_buffer == NULL) {
    hal_custom_receive(&spi_instance, rx_buffer, rx_size, 5000);
  }
  else if (rx_size == 0 || rx_buffer == NULL) {
    hal_custom_transmit(&spi_instance, tx_buffer, tx_size, 5000);
  }
  else {
    HAL_SPI_TransmitReceive(&spi_instance, tx_buffer, rx_buffer, max(rx_size, tx_size), 5000);
  }
  return FLN_OK;
}
