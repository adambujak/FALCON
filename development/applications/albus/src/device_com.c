#include "device_com.h"

#include "falcon_common.h"
#include "radio_common.h"
#include "frf.h"
#include "bsp.h"

static uint8_t hedwig_address[RADIO_ADDRESS_LENGTH];
static uint8_t albus_address[RADIO_ADDRESS_LENGTH];

static frf_t radio;

static void rf_spi_transfer (void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
  bsp_rf_transceive(tx_buf, tx_len, rx_buf, rx_len);
}

static inline void rf_isr(void)
{
  frf_isr(&radio);
}

void device_com_setup(void)
{
  FLN_ERR_CHECK(bsp_rf_init(rf_isr));

  radio_get_hedwig_address(hedwig_address);
  radio_get_albus_address(albus_address);

  frf_init(&radio, rf_spi_transfer, 0, bsp_rf_cs_set, bsp_rf_ce_set, vTaskDelay);
}

void device_com_task(void *pvParameters)
{
  uint8_t payload_len = FRF_PACKET_SIZE;
  frf_start(&radio, 2, payload_len, albus_address, hedwig_address);

  while(1) {
    frf_packet_t packet;
    if (frf_getPacket(&radio, packet) == 0) {
      DEBUG_LOG("HEDWIG: %s\r\n", (char*)packet);
      BSP_LED_Toggle(LED1);

      vTaskDelay(50);
    }
    frf_process(&radio);

    vTaskDelay(10);
  }
}

