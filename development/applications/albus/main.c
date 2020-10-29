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
#include "nrf24l01.h"

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

nRF24L01_t radio;

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
    NRF_LOG_INFO("width %d\r\n", width);


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

    //CE_HIGH();

    nRF24L01_set_operation_mode(&radio, NRF24L01_PTX);
}

static void radio_send(uint8_t *data, uint8_t len)
{
  CE_LOW();
  nRF24L01_flush_tx(&radio);
  nRF24L01_write_tx_payload(&radio, data, len);
  CE_HIGH();
}

int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    spi_init();

    set_rf_ce_pin(1);

    nRF24L01_initialize(&radio, spi_transfer, (void *)&spi, set_rf_cs_pin);
    uint8_t txData[4] = {1,2,3,4};
    uint8_t payload_len = 4;
    radio_init(2, payload_len);

    NRF_LOG_INFO("SPI example started.");

    while (1)
    {
        radio_send(txData, payload_len);
        nrf_delay_ms(1000);

        txData[0]++;

        NRF_LOG_FLUSH();

        bsp_board_led_invert(BSP_BOARD_LED_0);
        nrf_delay_ms(200);
    }
}
