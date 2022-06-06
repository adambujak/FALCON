/******************************************************************************
* @file     board.h
* @brief    Albus board file
* @version  1.0
* @date     2021-03-25
* @author   Adam Bujak
******************************************************************************/

#ifndef BOARD_H
#define BOARD_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_utils.h"

#define SYSCLK_FREQ             96000000

/* System Time */
#define SYSTEM_TIME_TIMER       TIM2
#define SYSTEM_TIME_CLK_EN()    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2)

/* UART */
#if defined(BOARD_DISCO)
#define UART                  USART2
#define UART_IRQn             USART2_IRQn
#define UART_IRQHandler       USART2_IRQHandler

#define UART_RX_PIN           LL_GPIO_PIN_3
#define UART_TX_PIN           LL_GPIO_PIN_2
#define UART_GPIO_PORT        GPIOA
#define UART_GPIO_AF          LL_GPIO_AF_7

#define UART_BAUDRATE         115200

#define UART_CLK_EN()         LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2)
#define GPIO_UART_CLK_EN()    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)

#elif defined(BOARD_NUCLEO)
#define UART                  USART3
#define UART_IRQn             USART3_IRQn
#define UART_IRQHandler       USART3_IRQHandler

#define UART_RX_PIN           LL_GPIO_PIN_8
#define UART_TX_PIN           LL_GPIO_PIN_9
#define UART_GPIO_PORT        GPIOD
#define UART_GPIO_AF          LL_GPIO_AF_7

#define UART_BAUDRATE         115200

#define UART_CLK_EN()         LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3)
#define GPIO_UART_CLK_EN()    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD)
#endif

/* RF */
#define RF_SPI                           SPI2
#define RF_SPI_CLK_ENABLE()              __HAL_RCC_SPI2_CLK_ENABLE()
#define RF_SPI_SCK_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_SPI_MISO_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_SPI_MOSI_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE(); \
  __HAL_RCC_GPIOC_CLK_ENABLE()
#define RF_IRQ_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()

#define RF_SPI_FORCE_RESET()             __HAL_RCC_SPI2_FORCE_RESET()
#define RF_SPI_RELEASE_RESET()           __HAL_RCC_SPI2_RELEASE_RESET()

#define RF_SPI_SCK_PIN                   GPIO_PIN_13
#define RF_SPI_SCK_GPIO_PORT             GPIOB
#define RF_SPI_SCK_AF                    GPIO_AF5_SPI2
#define RF_SPI_MISO_PIN                  GPIO_PIN_14
#define RF_SPI_MISO_GPIO_PORT            GPIOB
#define RF_SPI_MISO_AF                   GPIO_AF5_SPI2
#define RF_SPI_MOSI_PIN                  GPIO_PIN_15
#define RF_SPI_MOSI_GPIO_PORT            GPIOB
#define RF_SPI_MOSI_AF                   GPIO_AF5_SPI2

#define RF_IRQ_PIN                       LL_GPIO_PIN_10
#define RF_IRQ_GPIO_PORT                 GPIOB
#define RF_IRQ_EXTI_LINE                 LL_EXTI_LINE_10

#define RF_IRQn                          EXTI15_10_IRQn
#define RF_IRQHandler                    EXTI15_10_IRQHandler

#define RF_CE_PIN                        LL_GPIO_PIN_6
#define RF_CE_GPIO_PORT                  GPIOC

#define RF_GPIO_IRQ_CLK_EN()             LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
#define RF_GPIO_CE_CLK_EN()              LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
#define RF_GPIO_SS_CLK_EN()              LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

#define RF_SPI_SS_PIN                    LL_GPIO_PIN_12
#define RF_SS_GPIO_PORT                  GPIOB

#endif  // BOARD_H
