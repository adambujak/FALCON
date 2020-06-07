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

#include "nRF24L01.h"

#define SPI_INSTANCE  0 /**< SPI instance index. */
static nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

static uint8_t       m_tx_buf[2];           /**< TX buffer. */
static uint8_t       m_rx_buf[sizeof(m_tx_buf) + 1];  /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */

#define PIN_IN  0

static inline void setCS(uint8_t val);
/**
 * @brief SPI user event handler.
 * @param event
 */
void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
    if (m_rx_buf[0] != 0)
    {

    }
}

uint8_t read(uint8_t addr)
{
    memset(m_rx_buf, 0, m_length);
    spi_xfer_done = false;

    m_tx_buf[0] =  addr & 0x1F;

    setCS(0);
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 1, m_rx_buf, m_length));

    while (!spi_xfer_done)
    {
        __WFE();
    }
    setCS(1);
    return m_rx_buf[1];
}

void write(uint8_t addr, uint8_t val)
{
    memset(m_rx_buf, 0, m_length);
    spi_xfer_done = false;

    m_tx_buf[0] = 0x20 | (addr & 0x1F);
    m_tx_buf[1] = val;

    NRF_LOG_INFO("Writing data 0: %x", m_tx_buf[0]);
    NRF_LOG_INFO("Writing data 1: %x", m_tx_buf[1]);

    setCS(1);
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 2, m_rx_buf, m_length));
    while (!spi_xfer_done)
    {
        __WFE();
    }
    setCS(0);
}

static inline void transfer ( void * context, uint8_t * tx_buf, uint16_t tx_len,
    uint8_t * rx_buf, uint16_t rx_len )
{
    spi_xfer_done = false;
    nrf_drv_spi_transfer(context, tx_buf, tx_len, rx_buf, rx_len);
    while (!spi_xfer_done)
    {
        __WFE();
    }
}

void irq_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    NRF_LOG_INFO("IRQ pin fired interrup");
}

/**
 * @brief Function for configuring: PIN_IN pin for input, PIN_OUT pin for output,
 * and configures GPIOTE to give an interrupt on pin change.
 */
static void gpio_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_config_t out_config = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);

    err_code = nrf_drv_gpiote_out_init(SPI_SS_PIN, &out_config);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(PIN_IN, &in_config, irq_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(PIN_IN, true);
}

static inline void setCS(uint8_t val)
{
    if (val > 0)
    {
        nrf_drv_gpiote_out_set(SPI_SS_PIN);
    }
    else
    {
        nrf_drv_gpiote_out_clear(SPI_SS_PIN);
    }
}

int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = (uint8_t)NRF_SPI_PIN_NOT_CONNECTED;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin  = SPI_SCK_PIN;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    gpio_init();

    nRF24L01_t rfModule;
    nRF24L01_initialize(&rfModule, transfer, &spi, setCS);


    NRF_LOG_INFO("SPI example started.");


    uint8_t reg = read(0x00);

    NRF_LOG_INFO("CONfig %d: \n", reg);

    nRF24L01_set_power_mode(&rfModule, NRF24L01_PWR_UP);

    reg = read(0x00);

    NRF_LOG_INFO("CONfig %d: \n", reg);
    while (1)
    {

        NRF_LOG_FLUSH();

        bsp_board_led_invert(BSP_BOARD_LED_0);
        nrf_delay_ms(200);
    }
}
