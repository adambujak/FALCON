#include "device_com.h"

#include "falcon_common.h"
#include "frf.h"
#include "bsp.h"

static uint8_t rxAddr[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
static uint8_t txAddr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};



frf_t radio;

static void rf_spi_transfer (void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
  bsp_rf_transceive(tx_buf, tx_len, rx_buf, rx_len);
}


void device_com_setup(void)
{
  FLN_ERR_CHECK(bsp_rf_init());


  frf_config_t config = {
    .setCE = bsp_rf_ce_set,
    .setCS = bsp_rf_cs_set,
    .blockingTransfer = rf_spi_transfer,
    .spiCtx = (void *)0
  };

  frf_init(&radio, config);
  uint8_t payload_len = FRF_MAX_SIZE_PACKET;

  frf_start(&radio, 2, payload_len, rxAddr, txAddr);
}

void device_com_task(void *pvParameters)
{
  uint8_t rxData[FRF_MAX_SIZE_PACKET];
  while(1) {

    if (frf_dataReady(&radio)) {
      frf_getData(&radio, rxData);
      DEBUG_LOG("READ Data: %d\r\n", (int)rxData[0]);
      bsp_leds_toggle(LED3_PIN);
    }

    vTaskDelay(50);
  }
}
