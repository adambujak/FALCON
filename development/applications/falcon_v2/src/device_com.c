#include "device_com.h"

#include "falcon_common.h"
#include "frf.h"
#include "bsp.h"

static uint8_t rxAddr[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
static uint8_t txAddr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};

static frf_t radio;

static void rf_spi_transfer (void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
  bsp_rf_transceive(tx_buf, tx_len, rx_buf, rx_len);
}

static void rfISR(void)
{
  DEBUG_LOG("RF Interrupt fired\r\n");
}

void device_com_setup(void)
{
  FLN_ERR_CHECK(bsp_rf_init(rfISR));

  frf_init(&radio, rf_spi_transfer, 0, bsp_rf_cs_set, bsp_rf_ce_set);
  uint8_t payload_len = FRF_MAX_SIZE_PACKET;

  frf_start(&radio, 2, payload_len, rxAddr, txAddr);
}

void device_com_task(void *pvParameters)
{
  char rxData[FRF_MAX_SIZE_PACKET];
  while(1) {

    if (frf_dataReady(&radio)) {
      frf_getData(&radio, (uint8_t *) rxData);
      DEBUG_LOG("READ Data: %s\r\n", rxData);
      bsp_leds_toggle(LED3_PIN);
    }

    vTaskDelay(50);
  }
}

