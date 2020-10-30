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

#define SPI_SCK_PIN 4
#define SPI_MISO_PIN 30
#define SPI_MOSI_PIN 29
#define SPI_SS_PIN 31
#define RF_CE_PIN 28
#define SPI_IRQ_PRIORITY 6

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

#define CE_LOW() set_rf_ce_pin(0)
#define CE_HIGH() set_rf_ce_pin(1)

frf_t radio;

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

static void spi_init(void)
{
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin = (uint8_t)NRF_SPI_PIN_NOT_CONNECTED;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin  = SPI_SCK_PIN;

    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    /* Initialize gpiote module */
    APP_ERROR_CHECK(nrf_drv_gpiote_init());

    /* Initialize SPI SS Pin */
    nrf_drv_gpiote_out_config_t ss_pin = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);
    APP_ERROR_CHECK(nrf_drv_gpiote_out_init(SPI_SS_PIN, &ss_pin));

    /* Initialize SPI CE Pin */
    nrf_drv_gpiote_out_config_t ce_pin = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);
    APP_ERROR_CHECK(nrf_drv_gpiote_out_init(RF_CE_PIN, &ce_pin));

    spi_xfer_done = false;
}


int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("SPI example started.\r\n");
    spi_init();

    set_rf_ce_pin(1);

    frf_config_t config = {
      .setCE = set_rf_ce_pin,
      .setCS = set_rf_cs_pin,
      .blockingTransfer = spi_transfer,
      .spiCtx = (void *)&spi
    };

    frf_init(&radio, config);
    uint8_t txData[FRF_MAX_SIZE_PACKET+1] = {1,2,3,4,5};
    uint8_t payload_len = FRF_MAX_SIZE_PACKET;

    frf_start(&radio, 2, payload_len, rxAddr, txAddr);
    frf_powerUpTx(&radio);

    while (1)
    {
        frf_send(&radio, txData, payload_len);

        txData[0]++;

        bsp_board_led_invert(BSP_BOARD_LED_0);
        nrf_delay_ms(200);
        NRF_LOG_FLUSH();
    }
}
