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
#include "stm32412g_discovery.h"

#include "falcon_common.h"

void bsp_board_bringup(void);

int bsp_rf_spi_init(void);

/* LEDS */
#define FLN_LED_1 LED1_PIN
#define FLN_LED_2 LED2_PIN
#define FLN_LED_3 LED3_PIN
#define FLN_LED_4 LED4_PIN

#define FLN_LED_PORT GPIOE

int  bsp_leds_init(uint32_t ledsMask);
void bsp_leds_toggle(uint32_t ledsMask);

/* UART */
typedef UART_HandleTypeDef fln_uart_handle_t;

#define FLN_UART                    USART2
#define USARTx_CLK_ENABLE()         __HAL_RCC_USART2_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()   __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET() __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define FLN_USARTx_TX_PIN       GPIO_PIN_2
#define FLN_USARTx_TX_GPIO_PORT GPIOA
#define FLN_USARTx_TX_AF        GPIO_AF7_USART2
#define FLN_USARTx_RX_PIN       GPIO_PIN_3
#define FLN_USARTx_RX_GPIO_PORT GPIOA
#define FLN_USARTx_RX_AF        GPIO_AF7_USART2

int  bsp_uart_init(fln_uart_handle_t *handle);
void bsp_uart_put_char(fln_uart_handle_t *handle, uint8_t *ch);
void bsp_uart_write(fln_uart_handle_t *handle, uint8_t *data, uint16_t length);

/*I2C*/
typedef I2C_HandleTypeDef fln_i2c_handle_t;

#define FLN_SENSORS_I2C                            I2C1
#define FLN_SENSORS_I2C_CLK_ENABLE()               __HAL_RCC_I2C1_CLK_ENABLE()
#define FLN_SENSORS_I2C_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define FLN_SENSORS_I2C_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define FLN_SENSORS_I2C_FORCE_RESET()              __HAL_RCC_I2C1_FORCE_RESET()
#define FLN_SENSORS_I2C_RELEASE_RESET()            __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define FLN_SENSORS_I2C_SCL_PIN                    GPIO_PIN_8 /*D9*/
#define FLN_SENSORS_I2C_SCL_GPIO_PORT              GPIOB
#define FLN_SENSORS_I2C_SDA_PIN                    GPIO_PIN_9 /*D14*/
#define FLN_SENSORS_I2C_SDA_GPIO_PORT              GPIOB
#define FLN_SENSORS_I2C_SCL_SDA_AF                 GPIO_AF4_I2C1

/* I2C SPEEDCLOCK define: max value 400 KHz on STM32F4xx*/
#define FLN_SENSORS_I2C_SPEEDCLOCK   400000
#define FLN_SENSORS_I2C_DUTYCYCLE    I2C_DUTYCYCLE_2

int bsp_i2c_init(fln_i2c_handle_t *handle);

int bsp_i2c_write(fln_i2c_handle_t *handle,
                  uint8_t slave_addr,
                  uint8_t reg_addr,
                  uint16_t length,
                  uint8_t *data);

int bsp_i2c_read(fln_i2c_handle_t *handle,
                 uint8_t slave_addr,
                 uint8_t reg_addr,
                 uint16_t length,
                 uint8_t *data);

#endif  // FALCON_BSP_H
