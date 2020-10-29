#include "device_com.h"

#include "falcon_common.h"
#include "frf.h"
#include "bsp.h"

static uint8_t rxAddr[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
static uint8_t txAddr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};

#define CE_LOW() bsp_rf_ce_set(0)
#define CE_HIGH() bsp_rf_ce_set(1)


nRF24L01_t radio;

static void rf_spi_transfer (void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
  bsp_rf_transceive(tx_buf, tx_len, rx_buf, rx_len);
}

static void radio_init(uint8_t channel, uint8_t payload_len)
{
    CE_LOW();
    // Set RF channel
    nRF24L01_set_rf_channel(&radio, channel);

    // Set length of incoming payload
    nRF24L01_set_rx_payload_width(&radio, 0, 0);
    nRF24L01_set_rx_payload_width(&radio, 1, payload_len);
    nRF24L01_set_rx_payload_width(&radio, 2, 0);
    nRF24L01_set_rx_payload_width(&radio, 3, 0);
    nRF24L01_set_rx_payload_width(&radio, 4, 0);
    nRF24L01_set_rx_payload_width(&radio, 5, 0);

    uint8_t width = nRF24L01_get_rx_payload_width(&radio, 1);
    DEBUG_LOG("width %d\r\n", width);


    nRF24L01_set_output_power(&radio, NRF24L01_0DBM);

    nRF24L01_set_datarate(&radio, NRF24L01_1MBPS);

    nRF24L01_set_crc_mode(&radio, NRF24L01_CRC_8BIT);


    nRF24L01_open_pipe(&radio, NRF24L01_ALL, true);

    nRF24L01_set_auto_retr(&radio, 0xF, 1000);
    nRF24L01_set_address_width(&radio, 5);

    // Dynamic length configurations: No dynamic length

    // Start listening
    nRF24L01_clear_irq_flags_get_status(&radio);
    nRF24L01_set_operation_mode(&radio, NRF24L01_PRX);
    nRF24L01_set_power_mode(&radio, NRF24L01_PWR_UP);

    nRF24L01_set_address(&radio, NRF24L01_PIPE1, rxAddr);
    nRF24L01_set_address(&radio, NRF24L01_PIPE0, txAddr);
    nRF24L01_set_address(&radio, NRF24L01_TX, txAddr);

    CE_HIGH();
}


uint8_t radio_data_ready(void)
{
    uint8_t status = nRF24L01_nop(&radio);

    if ( status & (1 << NRF24L01_RX_DR) )
    {
        return 1;
    }

  return !(nRF24L01_rx_fifo_empty(&radio));
}


void device_com_setup(void)
{
  FLN_ERR_CHECK(bsp_rf_init());

  nRF24L01_initialize(&radio, rf_spi_transfer, 0, bsp_rf_cs_set);
  uint8_t payload_len = 4;
  radio_init(2, payload_len);
  nRF24L01_flush_rx(&radio);
}

void device_com_task(void *pvParameters)
{
  uint8_t rxData[4];
  while(1) {

    if (radio_data_ready()) {
      nRF24L01_read_rx_payload(&radio, rxData);
      nRF24L01_clear_irq_flags_get_status(&radio);
      DEBUG_LOG("READ Data: %d\r\n", (int)rxData[0]);
      bsp_leds_toggle(LED3_PIN);
    }

    vTaskDelay(50);
  }
}
