/******************************************************************************
* @file     rf_bsp.h
* @brief    Falcon Board Support Package for RF
* @version  1.0
* @date     2020-04-02
* @author   Adam Bujak
******************************************************************************/

#ifndef FALCON_RF_BSP_H
#define FALCON_RF_BSP_H

#include "nrf_drv_gpiote.h"

void initialize_rf_pins(void (*rf_irq_handler) (nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action),
                        nrfx_gpiote_pin_t rf_ss_pin,
                        nrfx_gpiote_pin_t rf_ce_pin,
                        nrfx_gpiote_pin_t rf_irq_pin);

#endif // FALCON_RF_BSP_H
