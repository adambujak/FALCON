/******************************************************************************
 * @file     device_com.c
 * @brief    Device Com Layer Source File
 * @version  1.0
 * @date     2020-06-16
 * @author   Adam Bujak
 ******************************************************************************/

#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include <stdint.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "rf_bsp.h"
#include "app_config.h"
#include "device_com.h"

#include "frf.h"

static nrf_drv_spi_t rf_spi = NRF_DRV_SPI_INSTANCE(RF_SPI_INSTANCE);  /**< SPI instance. */
static volatile bool rf_spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

static uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
static uint8_t rx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};

static uint8_t rfIRQFiredFlag = 0;
static frf_t   rfModule;

static char txData[MAX_STRING_LENGTH] = TEST_STRING;
static char rxData[MAX_STRING_LENGTH];

/**
 * @brief  Function to set rf ce pin
 * @param  val - value to which to set pin
 * @retval None
 */
static void set_rf_ce_pin(uint8_t val)
{
    if (val > 0)
    {
        nrf_drv_gpiote_out_set(RF_CE_PIN);
    }
    else
    {
        nrf_drv_gpiote_out_clear(RF_CE_PIN);
    }
}

/**
 * @brief  Function to set rf cs pin
 * @param  val - value to which to set pin
 * @retval None
 */
static void set_rf_cs_pin(uint8_t val)
{
    if (val > 0)
    {
        nrf_drv_gpiote_out_set(RF_SPI_SS_PIN);
    }
    else
    {
        nrf_drv_gpiote_out_clear(RF_SPI_SS_PIN);
    }
}

/**
 * @brief  Function to be called when interrupt is detected on rf chip
 * @param  None
 * @retval None
 */
static void rf_irq_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    rfIRQFiredFlag = 1;
    NRF_LOG_INFO("IRQ pin fired interrupt");
}

/**
 * @brief  SPI user event handler.
 * @param  event
 * @retval None
 */
static void rf_spi_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context)
{
    rf_spi_xfer_done = true;
}

/**
 * @brief  SPI transfer function for rf module
 * @param  context - spi handle
 * @param  tx_buf  - pointer to data to be sent
 * @param  tx_len  - number of bytes to be sent
 * @param  rx_buf  - pointer to data to be read
 * @param  rx_len  - number of bytes to be read
 * @retval None
 */
static void rf_spi_transfer (void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
    rf_spi_xfer_done = false;
    nrf_drv_spi_transfer(context, tx_buf, tx_len, rx_buf, rx_len);
    while (!rf_spi_xfer_done)
    {
        __WFE();
    }
}

/**
 * @brief  Initialization function for device com
 * @param  None
 * @retval None
 */
void device_com_init(void)
{
    nrf_drv_spi_config_t rf_spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    rf_spi_config.ss_pin   = (uint8_t)NRF_SPI_PIN_NOT_CONNECTED;
    rf_spi_config.miso_pin = RF_SPI_MISO_PIN;
    rf_spi_config.mosi_pin = RF_SPI_MOSI_PIN;
    rf_spi_config.sck_pin  = RF_SPI_SCK_PIN;
    NRF_LOG_INFO("started device com init");
    NRF_LOG_FLUSH();
    APP_ERROR_CHECK(nrf_drv_spi_init(&rf_spi, &rf_spi_config, rf_spi_event_handler, NULL));

    NRF_LOG_INFO("init spi");
    NRF_LOG_FLUSH();
    initialize_rf_pins(rf_irq_pin_handler, RF_SPI_SS_PIN, RF_CE_PIN, RF_IRQ_PIN);

    frf_config_t rfConfig = {
        .direction = FRF_DIR_RX,
        .setCE = set_rf_ce_pin,
        .setCS = set_rf_cs_pin,
        .blockingTransfer = rf_spi_transfer,
        .spiCtx = &rf_spi
    };

    rxData[MAX_STRING_LENGTH-1]='\0';

    NRF_LOG_INFO("init pins");
    NRF_LOG_FLUSH();

    frf_init(&rfModule, rfConfig);
    frf_start(&rfModule, 2, MAX_STRING_LENGTH); //TODO channel, payloadlength
    frf_tx_address(&rfModule, tx_address);
    frf_rx_address(&rfModule, rx_address);
    NRF_LOG_INFO("frf started");
    NRF_LOG_FLUSH();
}

/**
 * @brief  Returns status of device com layer
 * @param  None
 * @retval status
 */
uint32_t device_com_getStatus(void)
{
    return 0;
}

static uint32_t rf_read(uint8_t *rxBuf)
{
    if(frf_dataReady(&rfModule))
    {
        uint8_t rxByteCnt = frf_payloadLength(&rfModule);
        frf_getData(&rfModule, rxBuf);
        NRF_LOG_INFO("%d bytes read from rf module", rxByteCnt);
        return rxByteCnt;
    }
    return 0;
}

static uint32_t rf_write(uint8_t *data, uint16_t length)
{
    frf_send(&rfModule, data, length);
    /* Wait for transmission to end */
    while(frf_isSending(&rfModule));

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

    if (rfIRQFiredFlag == 1)
    {
        NRF_LOG_INFO("Clear Interrupts");
        frf_clearInterrupts(&rfModule);
        rfIRQFiredFlag = 0;
    }
    frf_powerUpRx(&rfModule);
    NRF_LOG_FLUSH();
    return 0;
}

void device_com_process()
{
    int status = rf_read((uint8_t *) rxData);
    NRF_LOG_INFO("Rx address: %x || Read data: '%s' from %x", rx_address[0], rxData, tx_address[0]);
    NRF_LOG_FLUSH();
}
