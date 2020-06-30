/******************************************************************************
 * @file     rf_bsp.c
 * @brief    Falcon Board Support Package for RF
 * @version  1.0
 * @date     2020-06-07
 * @author   Adam Bujak
 ******************************************************************************/

#include "rf_bsp.h"

#include "nrf_drv_gpiote.h"

void initialize_rftest_pins(void)
{    /* Initialize SPI SS Pin */
    nrf_drv_gpiote_out_config_t ss_pin = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);

    APP_ERROR_CHECK(nrf_drv_gpiote_out_init(RF_TX_SPI_SS_PIN, &ss_pin));

    nrf_drv_gpiote_out_config_t ce_pin = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);

    APP_ERROR_CHECK(nrf_drv_gpiote_out_init(RF_TX_CE_PIN, &ce_pin));
}

void initialize_rf_pins(void (*rf_irq_handler) (nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action))
{
    /* Initialize gpiote module */
    APP_ERROR_CHECK(nrf_drv_gpiote_init());


    /* Initialize SPI SS Pin */
    nrf_drv_gpiote_out_config_t ss1_pin = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);

    APP_ERROR_CHECK(nrf_drv_gpiote_out_init(RF_RX_SPI_SS_PIN, &ss1_pin));

    nrf_drv_gpiote_out_config_t ce1_pin = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);

    APP_ERROR_CHECK(nrf_drv_gpiote_out_init(RF_RX_CE_PIN, &ce1_pin));

    /* Intialize IRQ pin */
    nrf_drv_gpiote_in_config_t rx_in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
    rx_in_config.pull = NRF_GPIO_PIN_PULLUP;

    APP_ERROR_CHECK(nrf_drv_gpiote_in_init(RF_RX_IRQ_PIN, &rx_in_config, rf_irq_handler));

    nrf_drv_gpiote_in_event_enable(RF_RX_IRQ_PIN, true);
}

void set_rf_tx_cs_pin(uint8_t val)
{
    if (val > 0)
    {
        nrf_drv_gpiote_out_set(RF_TX_SPI_SS_PIN);
    }
    else
    {
        nrf_drv_gpiote_out_clear(RF_TX_SPI_SS_PIN);
    }
}

void set_rf_tx_ce_pin(uint8_t val)
{
    if (val > 0)
    {
        nrf_drv_gpiote_out_set(RF_TX_CE_PIN);
    }
    else
    {
        nrf_drv_gpiote_out_clear(RF_TX_CE_PIN);
    }
}

void set_rf_rx_cs_pin(uint8_t val)
{
    if (val > 0)
    {
        nrf_drv_gpiote_out_set(RF_RX_SPI_SS_PIN);
    }
    else
    {
        nrf_drv_gpiote_out_clear(RF_RX_SPI_SS_PIN);
    }
}

void set_rf_rx_ce_pin(uint8_t val)
{
    if (val > 0)
    {
        nrf_drv_gpiote_out_set(RF_RX_CE_PIN);
    }
    else
    {
        nrf_drv_gpiote_out_clear(RF_RX_CE_PIN);
    }
}
