/******************************************************************************
 * @file     board.h
 * @brief    Albus board file
 * @version  1.0
 * @date     2021-03-25
 * @author   Adam Bujak
 ******************************************************************************/

#ifndef BOARD_H
#define BOARD_H

#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_gpio.h"

#define SYSCLK_FREQ            96000000

/* UART */
#define UART                   USART2
#define UART_IRQn              USART2_IRQn
#define UART_IRQHandler        USART2_IRQHandler

#define UART_RX_PIN            LL_GPIO_PIN_3
#define UART_TX_PIN            LL_GPIO_PIN_2
#define UART_GPIO_PORT         GPIOA
#define UART_GPIO_AF           LL_GPIO_AF_7

#define UART_BAUDRATE          115200

#define UART_CLK_EN()          LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2)
#define GPIO_UART_CLK_EN()     LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)

/* RF */
#define RF_SPI                     SPI2

#define RF_IRQ_PIN                 LL_GPIO_PIN_10
#define RF_IRQ_GPIO_PORT           GPIOB
#define RF_IRQ_EXTI_LINE           LL_EXTI_LINE_10

#define RF_IRQn                    EXTI15_10_IRQn
#define RF_IRQHandler              EXTI15_10_IRQHandler

#define RF_CE_PIN                  LL_GPIO_PIN_6
#define RF_CE_GPIO_PORT            GPIOC

#define RF_GPIO_IRQ_CLK_EN()       LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
#define RF_GPIO_CE_CLK_EN()        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
#define RF_GPIO_SS_CLK_EN()        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

#define RF_SPI_SS_PIN              LL_GPIO_PIN_12
#define RF_SPI_SCK_PIN             LL_GPIO_PIN_13
#define RF_SPI_MISO_PIN            LL_GPIO_PIN_14
#define RF_SPI_MOSI_PIN            LL_GPIO_PIN_15

#define RF_SPI_GPIO_AF             LL_GPIO_AF_5
#define RF_SPI_GPIO_PORT           GPIOB

#define RF_SPI_GPIO_CLK_EN()       LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2); \
                                   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

#define SYSTEM_TIME_TIMER          TIM2

#endif  // BOARD_H
