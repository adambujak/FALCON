#include "radio.h"

#include "falcon_common.h"
#include "radio_common.h"
#include "frf.h"
#include "bsp.h"

static uint8_t hedwigAddress[RADIO_ADDRESS_LENGTH];
static uint8_t albusAddress[RADIO_ADDRESS_LENGTH];

uint8_t rx_buffer[32];
uint8_t tx_buffer[32];
static volatile bool rfRxReady = false;

static frf_t radio;

static void rf_spi_transfer(void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
  bsp_rf_transceive(tx_buf, tx_len, rx_buf, rx_len);
}

static inline void rf_isr(void)
{
  frf_isr(&radio);
}

static void rf_event_callback(frf_event_t event)
{
  switch(event) {
    case FRF_EVENT_TX_FAILED:
      LOG_DEBUG("RF TX FAILED\r\n");
      break;
    case FRF_EVENT_TX_SUCCESS:
      LOG_DEBUG("RF TX SUCCESS\r\n");
      break;
    case FRF_EVENT_RX:
      rfRxReady = true;
      frf_getPacket(&radio, rx_buffer);
      LOG_DEBUG("RF RX Event\r\n");
      break;
  }
}

uint32_t radio_send_data(uint8_t *source, uint32_t length)
{
  frf_sendPacket(&radio, source);
  return 32;
}

uint32_t radio_get_data(uint8_t *dest, uint32_t length)
{
  if (rfRxReady) {
    memcpy(dest, rx_buffer, 32);
    rfRxReady = false;
    return 32;
  }
  return 0;
}

void radio_init(void)
{
  radio_get_hedwig_address(hedwigAddress);
  radio_get_albus_address(albusAddress);

  FLN_ERR_CHECK(bsp_rf_init(rf_isr));

  /* RF Module Init */
  frf_config_t config = {
    .transferFunc = rf_spi_transfer,
    .role = FRF_DEVICE_ROLE_RX,
    .spiCtx = NULL,
    .setCS = bsp_rf_cs_set,
    .setCE = bsp_rf_ce_set,
    .delay = rtos_delay_ms,
    .eventCallback = rf_event_callback
  };

  frf_init(&radio, &config);
  frf_start(&radio, 2, FRF_PACKET_SIZE, hedwigAddress, albusAddress);
}

void radio_process(void)
{
  frf_process(&radio);
}