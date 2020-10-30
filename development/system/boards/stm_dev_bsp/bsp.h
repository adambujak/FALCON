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

#define FLN_UART                        USART2
#define USARTx_CLK_ENABLE()             __HAL_RCC_USART2_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()            __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()          __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define FLN_USARTx_TX_PIN               GPIO_PIN_2
#define FLN_USARTx_TX_GPIO_PORT         GPIOA
#define FLN_USARTx_TX_AF                GPIO_AF7_USART2
#define FLN_USARTx_RX_PIN               GPIO_PIN_3
#define FLN_USARTx_RX_GPIO_PORT         GPIOA
#define FLN_USARTx_RX_AF                GPIO_AF7_USART2

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

/* MOTORS */
#define FLN_MOTOR_TIMER                 TIM3
#define FLN_MOTOR_TIMER_CLK_ENABLE()    __HAL_RCC_TIM3_CLK_ENABLE()
#define FLN_MOTOR_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE(); __HAL_RCC_GPIOB_CLK_ENABLE();

#define FLN_MOTOR_GPIO_PORT_CHANNEL1    GPIOA
#define FLN_MOTOR_GPIO_PORT_CHANNEL2    GPIOA
#define FLN_MOTOR_GPIO_PORT_CHANNEL3    GPIOB
#define FLN_MOTOR_GPIO_PORT_CHANNEL4    GPIOB
#define FLN_MOTOR_GPIO_PIN_CHANNEL1     GPIO_PIN_6
#define FLN_MOTOR_GPIO_PIN_CHANNEL2     GPIO_PIN_7
#define FLN_MOTOR_GPIO_PIN_CHANNEL3     GPIO_PIN_0
#define FLN_MOTOR_GPIO_PIN_CHANNEL4     GPIO_PIN_1
#define FLN_MOTOR_GPIO_AF_CHANNEL1      GPIO_AF2_TIM3
#define FLN_MOTOR_GPIO_AF_CHANNEL2      GPIO_AF2_TIM3
#define FLN_MOTOR_GPIO_AF_CHANNEL3      GPIO_AF2_TIM3
#define FLN_MOTOR_GPIO_AF_CHANNEL4      GPIO_AF2_TIM3

int bsp_motors_init(void);
void bsp_motors_pwm_set_us(uint8_t motor, uint16_t us);

/* RF */
#define RF_SPI                           SPI2
#define RF_SPI_CLK_ENABLE()              __HAL_RCC_SPI2_CLK_ENABLE()
#define RF_SPI_SCK_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_SPI_MISO_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_SPI_MOSI_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE(); __HAL_RCC_GPIOC_CLK_ENABLE()

#define RF_SPI_FORCE_RESET()             __HAL_RCC_SPI2_FORCE_RESET()
#define RF_SPI_RELEASE_RESET()           __HAL_RCC_SPI2_RELEASE_RESET()

#define RF_IRQ_PIN                       GPIO_PIN_10
#define RF_IRQ_GPIO_PORT                 GPIOB
#define RF_CE_PIN                        GPIO_PIN_0
#define RF_CE_GPIO_PORT                  GPIOC
#define RF_SPI_SS_PIN                    GPIO_PIN_12
#define RF_SPI_SS_GPIO_PORT              GPIOB
#define RF_SPI_SCK_PIN                   GPIO_PIN_13
#define RF_SPI_SCK_GPIO_PORT             GPIOB
#define RF_SPI_SCK_AF                    GPIO_AF5_SPI2
#define RF_SPI_MISO_PIN                  GPIO_PIN_14
#define RF_SPI_MISO_GPIO_PORT            GPIOB
#define RF_SPI_MISO_AF                   GPIO_AF5_SPI2
#define RF_SPI_MOSI_PIN                  GPIO_PIN_15
#define RF_SPI_MOSI_GPIO_PORT            GPIOB
#define RF_SPI_MOSI_AF                   GPIO_AF5_SPI2

int bsp_rf_spi_init(void);
void bsp_rf_spi_deinit(void);
int bsp_rf_transceive(uint8_t *txBuffer, uint16_t txSize, uint8_t *rxBuffer, uint16_t rxSize);

int bsp_rf_gpio_init(void);
void bsp_rf_cs_set(uint8_t value);
void bsp_rf_ce_set(uint8_t value);

int bsp_rf_init(void (*isrCallback) (void));
#endif  // FALCON_BSP_H
