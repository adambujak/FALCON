/******************************************************************************
 * @file     bsp.h
 * @brief    Falcon BSP for STM32 Dev Kit
 * @version  1.0
 * @date     2020-10-04
 * @author   Adam Bujak
 ******************************************************************************/

#ifndef FALCON_BSP_H
#define FALCON_BSP_H

#include <stdint.h>

#include "stm32f4xx_hal.h"
#include "board.h"
#include "falcon_common.h"

void bsp_board_bringup(void);

int bsp_rf_spi_init(void);
int bsp_leds_init(void (*callback) (void));
void bsp_leds_set(uint8_t val);
void bsp_leds_timer_start(void);
void bsp_leds_timer_stop(void);

int bsp_imu_int_init(void (*isrCallback) (void));

int bsp_motors_init(void);
void bsp_motors_pwm_set_us(uint8_t motor, uint16_t us);

int bsp_rf_spi_init(void);
void bsp_rf_spi_deinit(void);
int bsp_rf_transceive(uint8_t *txBuffer, uint16_t txSize, uint8_t *rxBuffer, uint16_t rxSize);

int bsp_rf_gpio_init(void);
void bsp_rf_cs_set(uint8_t value);
void bsp_rf_ce_set(uint8_t value);

int bsp_rf_init(void (*isrCallback) (void));
#endif  // FALCON_BSP_H
