#ifndef BOARD_H
#define BOARD_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_utils.h"

#define SYSCLK_FREQ                           96000000

/* System Time */
#define SYSTEM_TIME_TIMER                     TIM3
#define SYSTEM_TIME_CLK_EN()                  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3)

/* LEDS */
#ifdef BOARD_V1
#define FLN_LED_CLK_ENABLE()                  __HAL_RCC_GPIOA_CLK_ENABLE();
#define FLN_LED_PIN                           GPIO_PIN_5
#define FLN_LED_PORT                          GPIOA
#endif

#define FLN_LED_AF                            GPIO_AF2_TIM2
#define FLN_LED_TIMER                         TIM2
#define FLN_LED_TIMER_IRQ                     TIM2_IRQn
#define FLN_LED_TIMER_IRQ_Handler             TIM2_IRQHandler
#define FLN_LED_TIMER_CLK_ENABLE()            __HAL_RCC_TIM2_CLK_ENABLE()

/* UART */
#define UART                                  USART2
#define UART_CLK_ENABLE()                     __HAL_RCC_USART2_CLK_ENABLE();
#define UART_RX_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART_TX_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()

#define UART_FORCE_RESET()                    __HAL_RCC_USART2_FORCE_RESET()
#define UART_RELEASE_RESET()                  __HAL_RCC_USART2_RELEASE_RESET()

#define UART_TX_PIN                           GPIO_PIN_2
#define UART_TX_GPIO_PORT                     GPIOA
#define UART_TX_AF                            GPIO_AF7_USART2
#define UART_RX_PIN                           GPIO_PIN_3
#define UART_RX_GPIO_PORT                     GPIOA
#define UART_RX_AF                            GPIO_AF7_USART2

#define UART_BAUDRATE                         115200

/* Sensors */
#define FLN_SENSORS_I2C                       I2C1
#define FLN_SENSORS_I2C_CLK_ENABLE()          __HAL_RCC_I2C1_CLK_ENABLE()
#define FLN_SENSORS_I2C_SDA_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define FLN_SENSORS_I2C_SCL_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define FLN_SENSORS_I2C_FORCE_RESET()         __HAL_RCC_I2C1_FORCE_RESET()
#define FLN_SENSORS_I2C_RELEASE_RESET()       __HAL_RCC_I2C1_RELEASE_RESET()

#define FLN_SENSORS_I2C_SCL_PIN               GPIO_PIN_8
#define FLN_SENSORS_I2C_SCL_GPIO_PORT         GPIOB
#define FLN_SENSORS_I2C_SDA_PIN               GPIO_PIN_9
#define FLN_SENSORS_I2C_SDA_GPIO_PORT         GPIOB
#define FLN_SENSORS_I2C_SCL_SDA_AF            GPIO_AF4_I2C1

#define FLN_SENSORS_I2C_SPEEDCLOCK            400000
#define FLN_SENSORS_I2C_DUTYCYCLE             I2C_DUTYCYCLE_2

/* IMU */
#define IMU_IRQ_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define IMU_IRQ_PIN                           GPIO_PIN_8
#define IMU_IRQ_GPIO_PORT                     GPIOA

/* MOTORS */
#define FLN_MOTOR_TIMER                       TIM3
#define FLN_MOTOR_TIMER_CLK_ENABLE()          __HAL_RCC_TIM3_CLK_ENABLE()
#define FLN_MOTOR_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE();     \
                                              __HAL_RCC_GPIOB_CLK_ENABLE();

#define FLN_MOTOR_GPIO_PORT_CHANNEL1          GPIOA
#define FLN_MOTOR_GPIO_PORT_CHANNEL2          GPIOA
#define FLN_MOTOR_GPIO_PORT_CHANNEL3          GPIOB
#define FLN_MOTOR_GPIO_PORT_CHANNEL4          GPIOB
#define FLN_MOTOR_GPIO_PIN_CHANNEL1           GPIO_PIN_6
#define FLN_MOTOR_GPIO_PIN_CHANNEL2           GPIO_PIN_7
#define FLN_MOTOR_GPIO_PIN_CHANNEL3           GPIO_PIN_0
#define FLN_MOTOR_GPIO_PIN_CHANNEL4           GPIO_PIN_1
#define FLN_MOTOR_GPIO_AF_CHANNEL1            GPIO_AF2_TIM3
#define FLN_MOTOR_GPIO_AF_CHANNEL2            GPIO_AF2_TIM3
#define FLN_MOTOR_GPIO_AF_CHANNEL3            GPIO_AF2_TIM3
#define FLN_MOTOR_GPIO_AF_CHANNEL4            GPIO_AF2_TIM3

/* RF */
#define RF_SPI                                SPI2
#define RF_SPI_CLK_ENABLE()                   __HAL_RCC_SPI2_CLK_ENABLE()
#define RF_SPI_SCK_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_SPI_MISO_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_SPI_MOSI_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE(); \
                                              __HAL_RCC_GPIOC_CLK_ENABLE()
#define RF_IRQ_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()

#define RF_SPI_FORCE_RESET()                  __HAL_RCC_SPI2_FORCE_RESET()
#define RF_SPI_RELEASE_RESET()                __HAL_RCC_SPI2_RELEASE_RESET()

#define RF_IRQ_PIN                            GPIO_PIN_10
#define RF_IRQ_GPIO_PORT                      GPIOB
#define RF_CE_PIN                             GPIO_PIN_6
#define RF_CE_GPIO_PORT                       GPIOC
#define RF_SPI_SS_PIN                         GPIO_PIN_12
#define RF_SPI_SS_GPIO_PORT                   GPIOB
#define RF_SPI_SCK_PIN                        GPIO_PIN_13
#define RF_SPI_SCK_GPIO_PORT                  GPIOB
#define RF_SPI_SCK_AF                         GPIO_AF5_SPI2
#define RF_SPI_MISO_PIN                       GPIO_PIN_14
#define RF_SPI_MISO_GPIO_PORT                 GPIOB
#define RF_SPI_MISO_AF                        GPIO_AF5_SPI2
#define RF_SPI_MOSI_PIN                       GPIO_PIN_15
#define RF_SPI_MOSI_GPIO_PORT                 GPIOB
#define RF_SPI_MOSI_AF                        GPIO_AF5_SPI2

#endif  // BOARD_H
