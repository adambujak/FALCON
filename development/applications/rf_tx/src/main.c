#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

static uint8_t       m_tx_buf[2];           /**< TX buffer. */
static uint8_t       m_rx_buf[sizeof(m_tx_buf) + 1];  /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */

/**
 * @brief SPI user event handler.
 * @param event
 */
void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    spi_xfer_done = true;
    NRF_LOG_INFO("Transfer completed.");
    if (m_rx_buf[0] != 0)
    {
   //     NRF_LOG_INFO(" Received:");
   //     NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
    }
}

uint8_t read(uint8_t addr) 
{
    memset(m_rx_buf, 0, m_length);
    spi_xfer_done = false;
   
    m_tx_buf[0] =  addr & 0x1F;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 1, m_rx_buf, m_length));

    while (!spi_xfer_done)
    {
        __WFE();
    }
    NRF_LOG_INFO("Received read data 0: %x", m_rx_buf[0]);
    NRF_LOG_INFO("Received read data 1: %x", m_rx_buf[1]);
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
    
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 2, m_rx_buf, m_length));

    while (!spi_xfer_done)
    {
        __WFE();
    }
}

int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPI_SS_PIN;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin  = SPI_SCK_PIN;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    NRF_LOG_INFO("SPI example started.");
    
    uint8_t val = 0x02;

    uint8_t reg = read(0x00); 

    if ((reg & 0x01) != 0) 
    {
        val = 0x0; 
    }
    
    nrf_delay_ms(2000);
    NRF_LOG_INFO("Sending %x to write ", (reg & 0xFD) |val);

    write(0x00, (reg & 0xFD) |val);

    nrf_delay_ms(2000);
    reg = read(0x00);

    while (1)
    {
        
        NRF_LOG_FLUSH();

        bsp_board_led_invert(BSP_BOARD_LED_0);
        nrf_delay_ms(200);
    }
}
