#include "device_com.h"

#include "falcon_common.h"
#include "frf.h"
#include "bsp.h"

static uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
static uint8_t rx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};

static frf_t rfModule;

static void rf_spi_transfer (void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
  bsp_rf_transceive(tx_buf, tx_len, rx_buf, rx_len);
}

void device_com_setup(void)
{
  FLN_ERR_CHECK(bsp_rf_init());

  frf_config_t rfConfig = {
      .setCE = bsp_rf_ce_set,
      .setCS = bsp_rf_cs_set,
      .blockingTransfer = rf_spi_transfer,
      .spiCtx = 0
  };

  frf_init(&rfModule, rfConfig);
  frf_start(&rfModule, 2, 8, rx_address, tx_address); //TODO channel, payloadlength
}

void device_com_task(void *pvParameters)
{
  while(1) {
    vTaskDelay(1000);
  }
}
