#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "boards.h"
#include "rf_bsp.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "frf.h"

#define SPI_INSTANCE  0 /**< SPI instance index. */
static nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

static uint8_t       m_tx_buf[2];           /**< TX buffer. */
static uint8_t       m_rx_buf[sizeof(m_tx_buf) + 1];  /**< RX buffer. */
uint8_t data_array[4];

uint8_t tx_buffer[4] = {22,23,24,0};

uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};

uint8_t rxIRQFiredFlag = 0;

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

void rx_irq_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    rxIRQFiredFlag = 1;
    NRF_LOG_INFO("IRQ rx pin fired interrupt");
}

void tx_irq_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    NRF_LOG_INFO("IRQ tx pin fired interrupt");
}

int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    nrf_delay_ms(100);

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = (uint8_t)NRF_SPI_PIN_NOT_CONNECTED;
    spi_config.miso_pin = RF_SPI_MISO_PIN;
    spi_config.mosi_pin = RF_SPI_MOSI_PIN;
    spi_config.sck_pin  = RF_SPI_SCK_PIN;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    initialize_rf_pins(rx_irq_pin_handler, tx_irq_pin_handler);

    frf_t rfTxModule;
    frf_config_t rfTxConfig = {
        .direction = FRF_DIR_TX,
        .setCE = set_rf_tx_ce_pin,
        .setCS = set_rf_tx_cs_pin,
        .blockingTransfer = transfer,
        .spiCtx = &spi
    };

    frf_init(&rfTxModule, rfTxConfig);

    frf_start(&rfTxModule, 2, 4);

    frf_tx_address(&rfTxModule, tx_address);
    frf_rx_address(&rfTxModule, rx_address);

    frf_t rfRxModule;
    frf_config_t rfRxConfig = {
        .direction = FRF_DIR_RX,
        .setCE = set_rf_rx_ce_pin,
        .setCS = set_rf_rx_cs_pin,
        .blockingTransfer = transfer,
        .spiCtx = &spi
    };


    frf_init(&rfRxModule, rfRxConfig);

    frf_start(&rfRxModule, 2, 4);

    frf_tx_address(&rfRxModule, rx_address);
    frf_rx_address(&rfRxModule, tx_address);

    while(1)
    {
        if(frf_dataReady(&rfRxModule))
        {
            frf_getData(&rfRxModule, data_array);
            NRF_LOG_INFO("> ");
            NRF_LOG_INFO("%d ",data_array[0]);
            NRF_LOG_INFO("%d ",data_array[1]);
            NRF_LOG_INFO("%d ",data_array[2]);
            NRF_LOG_INFO("%d\r\n",data_array[3]);
        }
        tx_buffer[3]++;
        nrf_delay_ms(1500);
        frf_send(&rfTxModule, tx_buffer, 4);

        /* Wait for transmission to end */
        while(frf_isSending(&rfTxModule));

        uint8_t temp = frf_lastMessageStatus(&rfTxModule);

        if(temp == FRF_TRANSMISSON_OK)
        {
            NRF_LOG_INFO("Transmission success!\r\n");
            NRF_LOG_FLUSH();
        }
        else if(temp == FRF_MESSAGE_LOST)
        {
            NRF_LOG_INFO("Transmission failed!\r\n");
            NRF_LOG_FLUSH();
        }

        if (rxIRQFiredFlag == 1)
        {
            NRF_LOG_INFO("Clear Interrupts");
            frf_clearInterrupts(&rfRxModule);
            rxIRQFiredFlag = 0;
        }

        NRF_LOG_FLUSH();
        bsp_board_led_invert(BSP_BOARD_LED_0);
        nrf_delay_ms(500);
    }
}
