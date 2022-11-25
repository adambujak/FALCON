/******************************************************************************
* @file     rf_bsp.c
* @brief    Falcon Board Support Package for RF
* @version  1.0
* @date     2020-06-07
* @author   Adam Bujak
******************************************************************************/

#include "rf_bsp.h"

#include "nrf_drv_gpiote.h"

/**
 * @brief   Setup GPIO pins
 * @param   rf_irq_handler - irq pin handler function
 * @param   rf_ss_pin      - rf slave select pin (spi)
 * @param   rf_ce_pin      - rf chip enable pin
 * @param   rf_irq_pin     - rf irq status pin
 * @retval  None
 */
void initialize_rf_pins(void (*rf_irq_handler) (nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action),
                        nrfx_gpiote_pin_t rf_ss_pin,
                        nrfx_gpiote_pin_t rf_ce_pin,
                        nrfx_gpiote_pin_t rf_irq_pin)
{
  /* Initialize gpiote module */
  APP_ERROR_CHECK(nrf_drv_gpiote_init());

  /* Initialize SPI SS Pin */
  nrf_drv_gpiote_out_config_t ss_pin = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);

  APP_ERROR_CHECK(nrf_drv_gpiote_out_init(rf_ss_pin, &ss_pin));

  /* Initialize SPI CE Pin */
  nrf_drv_gpiote_out_config_t ce_pin = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);

  APP_ERROR_CHECK(nrf_drv_gpiote_out_init(rf_ce_pin, &ce_pin));

  /* Intialize IRQ pin */
  nrf_drv_gpiote_in_config_t rx_in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);

  rx_in_config.pull = NRF_GPIO_PIN_PULLUP;

  APP_ERROR_CHECK(nrf_drv_gpiote_in_init(rf_irq_pin, &rx_in_config, rf_irq_handler));

  nrf_drv_gpiote_in_event_enable(rf_irq_pin, true);
}
