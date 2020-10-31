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
  frf_isr(&radio);
}

void device_com_setup(void)
{
  FLN_ERR_CHECK(bsp_rf_init(rfISR));

  frf_init(&radio, rf_spi_transfer, 0, bsp_rf_cs_set, bsp_rf_ce_set);
  uint8_t payload_len = FRF_PACKET_SIZE;

  frf_start(&radio, 2, payload_len, rxAddr, txAddr);
}

void device_com_task(void *pvParameters)
{
  /* Implement packet queue - head only popped upon tx event */
  /* RX event - set rx flag and handle in main loop*/
  while(1) {
//	char rxData[32];
//    if (frf_dataReady(&radio)) {
//      frf_getData(&radio, (uint8_t *) rxData);
//      bsp_leds_toggle(LED3_PIN);
//    }
    frf_packet_t packet;
    if (frf_getPacket(&radio, packet) == 0) {
      DEBUG_LOG("REad: %s\r\n", (char*)packet);
      bsp_leds_toggle(LED3_PIN);
    }

    vTaskDelay(50);
  }
}

