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

#define RF_SPI_MOSI_PIN    29
#define RF_SPI_MISO_PIN    30
#define RF_SPI_SCK_PIN     26
#define RF_TX_SPI_SS_PIN   31
#define RF_TX_IRQ_PIN      8
#define RF_TX_CE_PIN       7
#define RF_RX_SPI_SS_PIN   6
#define RF_RX_CE_PIN       5
#define RF_RX_IRQ_PIN      12

/**
  * @brief   Setup GPIO pins
  * @retval  None
  */
void initialize_rf_pins(void (*rx_irq_handler) (nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action),
                        void (*tx_irq_handler) (nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action));

void set_rf_tx_cs_pin(uint8_t val);

void set_rf_tx_ce_pin(uint8_t val);

void set_rf_rx_cs_pin(uint8_t val);

void set_rf_rx_ce_pin(uint8_t val);

#endif // FALCON_BSP_H
