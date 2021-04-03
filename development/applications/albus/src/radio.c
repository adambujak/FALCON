#include "radio.h"

#include "falcon_common.h"
#include "radio_common.h"
#include "system_time.h"

#include "falcon_packet.h"
#include "fp_encode.h"
#include "ff_encoder.h"

#include "spi.h"
#include "gpio.h"
#include "frf.h"

static uint8_t hedwig_address[RADIO_ADDRESS_LENGTH];
static uint8_t albus_address[RADIO_ADDRESS_LENGTH];

static frf_t radio;
uint32_t last_tx_time;
uint8_t frame_buffer[MAX_FRAME_SIZE];

static inline void rf_spi_transfer(void *context,
                                   uint8_t *tx_buf, uint16_t tx_len,
                                   uint8_t *rx_buf, uint16_t rx_len)
{
  (void) context;
  spi_transfer(tx_buf, tx_len, rx_buf, rx_len);
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
      // xTimerChangePeriod(rfTxTimer, 1000, 25);
      break;
    case FRF_EVENT_TX_SUCCESS:
      LOG_DEBUG("RF TX SUCCESS\r\n");
      break;
    case FRF_EVENT_RX:
      // rfRxReady = true;
      LOG_DEBUG("RF RX Event\r\n");
      break;
  }
}

static void rf_tx(void)
{
  uint32_t now = system_time_get();
  if (system_time_cmp_ms(last_tx_time, now) < 1000) {
    return;
  }
  frf_pushPacket(&radio, frame_buffer);
  frf_tx(&radio);
  last_tx_time = now;
}

void radio_process(void)
{
  rf_tx();
  frf_process(&radio);
}

void temp_func(void)
{
  ff_encoder_t encoder;
  ff_encoder_init(&encoder);
  ff_encoder_set_buffer(&encoder, frame_buffer);

  fpc_flight_control_t control = {
    {
      1.2,1.4,1.6,1.8
    }
  };

  if (ff_encoder_append_packet(&encoder, &control, FPT_FLIGHT_CONTROL_COMMAND) == FLN_ERR) {
    error_handler();
  }

  ff_encoder_append_footer(&encoder);
}

void radio_init(void)
{
  gpio_rf_irq_register(rf_isr);
  radio_get_hedwig_address(hedwig_address);
  radio_get_albus_address(albus_address);

  temp_func();

  frf_config_t config = {
    .transferFunc = rf_spi_transfer,
    .spiCtx = NULL,
    .setCS = gpio_rf_cs_write,
    .setCE = gpio_rf_ce_write,
    .delay_us = delay_us,
    .eventCallback = rf_event_callback
  };

  frf_init(&radio, &config);
  frf_start(&radio, 2, FRF_PACKET_SIZE, albus_address, hedwig_address);
}
