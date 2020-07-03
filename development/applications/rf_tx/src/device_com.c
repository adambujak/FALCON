/******************************************************************************
 * @file     device_com.c
 * @brief    Device Com Layer Source File
 * @version  1.0
 * @date     2020-06-16
 * @author   Adam Bujak
 ******************************************************************************/

#include "device_com.h"

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

//SPI buffers
static uint8_t       m_tx_buf[2];           /**< TX buffer. */
static uint8_t       m_rx_buf[sizeof(m_tx_buf) + 1];  /**< RX buffer. */

static uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
static uint8_t rx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};

static uint8_t rxIRQFiredFlag = 0;
static frf_t rfModule;
static frf_t rftestModule;


/**
 * @brief SPI user event handler.
 * @param event
 */
static void spi_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context)
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


static void rf_irq_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    rxIRQFiredFlag = 1;
    NRF_LOG_INFO("IRQ rx pin fired interrupt");
}

//TODO delete
static void device_com_test_init(void)
{
    frf_config_t rftestConfig = {
        .direction = FRF_DIR_RX,
        .setCE = set_rf_rx_ce_pin,
        .setCS = set_rf_rx_cs_pin,
        .blockingTransfer = transfer,
        .spiCtx = &spi
    };


    frf_init(&rftestModule, rftestConfig);
    frf_start(&rftestModule, 2, 4); //TODO channel, payloadlength
    frf_tx_address(&rftestModule, rx_address);
    frf_rx_address(&rftestModule, tx_address);
}

void device_com_init(void)
{
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = (uint8_t)NRF_SPI_PIN_NOT_CONNECTED;
    spi_config.miso_pin = RF_SPI_MISO_PIN;
    spi_config.mosi_pin = RF_SPI_MOSI_PIN;
    spi_config.sck_pin  = RF_SPI_SCK_PIN;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    initialize_rf_pins(rf_irq_pin_handler);

    //TODO: for testing - remove later
    initialize_rftest_pins();

    frf_config_t rfConfig = {
        .direction = FRF_DIR_TX,
        .setCE = set_rf_tx_ce_pin,
        .setCS = set_rf_tx_cs_pin,
        .blockingTransfer = transfer,
        .spiCtx = &spi
    };

    frf_init(&rfModule, rfConfig);
    frf_start(&rfModule, 2, 4); //TODO channel, payloadlength
    frf_tx_address(&rfModule, tx_address);
    frf_rx_address(&rfModule, rx_address);

    //test TODO: delete
    device_com_test_init();

}

uint32_t device_com_getStatus(void)
{
    return 0;
}

uint32_t device_com_read(uint8_t *read_buf)
{
    // frf_powerUpRx(&rfModule);
    if(frf_dataReady(&rfModule))
        {
        frf_getData(&rfModule, read_buf);
        NRF_LOG_INFO("> ");
        NRF_LOG_INFO("%d ",read_buf[0]);
        NRF_LOG_INFO("%d ",read_buf[1]);
        NRF_LOG_INFO("%d ",read_buf[2]);
        NRF_LOG_INFO("%d\r\n",read_buf[3]);
        }
    return 0;
}

uint32_t device_com_write(uint8_t *data, uint16_t length)
{
    frf_send(&rfModule, data, length);
    /* Wait for transmission to end */
    while(frf_isSending(&rfModule));
    // frf_powerDown(&rfModule);

    uint8_t temp = frf_lastMessageStatus(&rfModule);

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
        frf_clearInterrupts(&rfModule);
        rxIRQFiredFlag = 0;
    } 
    frf_powerUpRx(&rfModule);
    NRF_LOG_FLUSH();
    return 0;
}

uint32_t device_com_test_read(uint8_t *read_buf)
{   
    if(frf_dataReady(&rftestModule))
        {
        frf_getData(&rftestModule, read_buf);
        NRF_LOG_INFO("> ");
        NRF_LOG_INFO("%d ",read_buf[0]);
        NRF_LOG_INFO("%d ",read_buf[1]);
        NRF_LOG_INFO("%d ",read_buf[2]);
        NRF_LOG_INFO("%d\r\n",read_buf[3]);
        }
    return 0;
}

uint32_t device_com_test_write(uint8_t *data, uint16_t length)
{
    frf_send(&rftestModule, data, length);
    /* Wait for transmission to end */
    while(frf_isSending(&rftestModule));
    // frf_powerDown(&rftestModule);

    uint8_t temp = frf_lastMessageStatus(&rftestModule);

    if(temp == FRF_TRANSMISSON_OK)
    {
        NRF_LOG_INFO("Transmission from test success!\r\n");
        NRF_LOG_FLUSH();
    }
    else if(temp == FRF_MESSAGE_LOST)
    {
        NRF_LOG_INFO("Transmission from test failed!\r\n");
        NRF_LOG_FLUSH();
    }

        if (rxIRQFiredFlag == 1)
    {
        NRF_LOG_INFO("Clear from test Interrupts");
        frf_clearInterrupts(&rftestModule);
        rxIRQFiredFlag = 0;
    } 

    frf_powerUpRx(&rftestModule);
    NRF_LOG_FLUSH();
    return 0;
}

uint32_t device_com_test(void)
{
    return 0;
}