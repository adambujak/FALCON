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
#define FLN_LED_1    LED1_PIN
#define FLN_LED_2    LED2_PIN
#define FLN_LED_3    LED3_PIN
#define FLN_LED_4    LED4_PIN

#define FLN_LED_PORT GPIOE

int bsp_leds_init(uint32_t ledsMask);
void bsp_leds_toggle(uint32_t ledsMask);


 /* UART */
typedef UART_HandleTypeDef              fln_uart_handle_t;

#define FLN_UART                         USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define FLN_USARTx_TX_PIN                GPIO_PIN_2
#define FLN_USARTx_TX_GPIO_PORT          GPIOA
#define FLN_USARTx_TX_AF                 GPIO_AF7_USART2
#define FLN_USARTx_RX_PIN                GPIO_PIN_3
#define FLN_USARTx_RX_GPIO_PORT          GPIOA
#define FLN_USARTx_RX_AF                 GPIO_AF7_USART2

int bsp_uart_init(fln_uart_handle_t *handle);
void bsp_uart_put_char(fln_uart_handle_t *handle, uint8_t c);

#endif // FALCON_BSP_H
