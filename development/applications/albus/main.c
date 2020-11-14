#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "frf.h"
#include "falcon_common.h"

#define SPI_SCK_PIN 4
#define SPI_MISO_PIN 30
#define SPI_MOSI_PIN 29
#define SPI_SS_PIN 31
#define RF_CE_PIN 28
#define RF_IRQ_PIN 3
#define SPI_IRQ_PRIORITY 6

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

#define CE_LOW() set_rf_ce_pin(0)
#define CE_HIGH() set_rf_ce_pin(1)

frf_t radio;
volatile bool irq_fired = false;

static uint8_t txAddr[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
static uint8_t rxAddr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};

static void set_rf_ce_pin(uint8_t val)
{
  if (val > 0) {
    nrf_drv_gpiote_out_set(RF_CE_PIN);
  }
  else {
    nrf_drv_gpiote_out_clear(RF_CE_PIN);
  }
}

static void set_rf_cs_pin(uint8_t val)
{
  if (val > 0) {
    nrf_drv_gpiote_out_set(SPI_SS_PIN);
  }
  else {
    nrf_drv_gpiote_out_clear(SPI_SS_PIN);
  }
}

static void spi_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context)
{
  spi_xfer_done = true;
}

static void spi_transfer (void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
  spi_xfer_done = false;
  nrf_drv_spi_transfer(context, tx_buf, tx_len, rx_buf, rx_len);
  while (!spi_xfer_done) {
      __WFE();
  }
}

static inline void rf_irq_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  frf_isr(&radio);
  irq_fired = true;
}

static void spi_init(void)
{
  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  spi_config.ss_pin = (uint8_t)NRF_SPI_PIN_NOT_CONNECTED;
  spi_config.miso_pin = SPI_MISO_PIN;
  spi_config.mosi_pin = SPI_MOSI_PIN;
  spi_config.sck_pin  = SPI_SCK_PIN;
  spi_xfer_done = false;

  APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

  /* Initialize gpiote module */
  APP_ERROR_CHECK(nrf_drv_gpiote_init());

  /* Initialize SPI SS Pin */
  nrf_drv_gpiote_out_config_t ss_pin = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);
  APP_ERROR_CHECK(nrf_drv_gpiote_out_init(SPI_SS_PIN, &ss_pin));

  /* Initialize SPI CE Pin */
  nrf_drv_gpiote_out_config_t ce_pin = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);
  APP_ERROR_CHECK(nrf_drv_gpiote_out_init(RF_CE_PIN, &ce_pin));

  /* Initialize RF IRQ pin */
  nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
  in_config.pull = NRF_GPIO_PIN_NOPULL;

  ret_code_t err_code = nrf_drv_gpiote_in_init(RF_IRQ_PIN, &in_config, rf_irq_pin_handler);
  APP_ERROR_CHECK(err_code);

  nrf_drv_gpiote_in_event_enable(RF_IRQ_PIN, true);
}


int main(void)
{
  bsp_board_init(BSP_INIT_LEDS);

  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  NRF_LOG_INFO("Albus started.\r\n");

  spi_init();

  frf_init(&radio, spi_transfer, (void *)&spi, set_rf_cs_pin,
           set_rf_ce_pin, nrf_delay_ms);

  char txData[FRF_PACKET_SIZE] = "ALBUS1";
  uint8_t payload_len = FRF_PACKET_SIZE;

  frf_start(&radio, 2, payload_len, rxAddr, txAddr);

  while (1)
  {
    //frf_sendPacket(&radio, (uint8_t*)txData);
    //frf_finishSending(&radio);

    txData[5] = 48+((txData[5]+1)%10);

    frf_packet_t packet;
    if (frf_getPacket(&radio, packet) == 0) {
      DEBUG_LOG("ALBUS: %s\r\n", (char*)packet);
    }

    frf_process(&radio);

    bsp_board_led_invert(BSP_BOARD_LED_0);
    NRF_LOG_FLUSH();
    while(!irq_fired) {
      nrf_delay_ms(500);
    }
    irq_fired = false;
  }
}
