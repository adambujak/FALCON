#include "uart.h"

#include "stm32f4xx_hal.h"
#include <string.h>

#define FLN_UART                         USART3
#define FLN_UART_CLK_ENABLE()            __HAL_RCC_USART3_CLK_ENABLE();
#define FLN_UART_RX_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()
#define FLN_UART_TX_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOD_CLK_ENABLE()

#define FLN_UART_FORCE_RESET()           __HAL_RCC_USART3_FORCE_RESET()
#define FLN_UART_RELEASE_RESET()         __HAL_RCC_USART3_RELEASE_RESET()

#define FLN_UART_TX_PIN                  GPIO_PIN_9
#define FLN_UART_TX_GPIO_PORT            GPIOD
#define FLN_UART_TX_AF                   GPIO_AF7_USART3
#define FLN_UART_RX_PIN                  GPIO_PIN_8
#define FLN_UART_RX_GPIO_PORT            GPIOD
#define FLN_UART_RX_AF                   GPIO_AF7_USART3

#define FLN_UART_IRQn                    USART2_IRQn
#define FLN_UART_IRQHandler              USART2_IRQHandler

#define FLN_UART_BAUDRATE                115200


static UART_HandleTypeDef uart_handle;

static int uart_module_init(void)
{
  /* (1) Enable GPIO clock and configures the USART pins *********************/

  /* Enable the peripheral clock of GPIO Port */
  USARTx_GPIO_CLK_ENABLE();

  /* Configure Tx Pin as : Alternate function, High Speed, Push pull, Pull up */
  LL_GPIO_SetPinMode(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, LL_GPIO_MODE_ALTERNATE);
  USARTx_SET_TX_GPIO_AF();
  LL_GPIO_SetPinSpeed(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, LL_GPIO_PULL_UP);

  /* Configure Rx Pin as : Alternate function, High Speed, Push pull, Pull up */
  LL_GPIO_SetPinMode(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, LL_GPIO_MODE_ALTERNATE);
  USARTx_SET_RX_GPIO_AF();
  LL_GPIO_SetPinSpeed(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, LL_GPIO_PULL_UP);

  /* (2) NVIC Configuration for USART interrupts */
  /*  - Set priority for USARTx_IRQn */
  /*  - Enable USARTx_IRQn */
  NVIC_SetPriority(USARTx_IRQn, 0);
  NVIC_EnableIRQ(USARTx_IRQn);

  /* (3) Enable USART peripheral clock and clock source ***********************/
  USARTx_CLK_ENABLE();

  /* (4) Configure USART functional parameters ********************************/

  /* Disable USART prior modifying configuration registers */
  /* Note: Commented as corresponding to Reset value */
  // LL_USART_Disable(USARTx_INSTANCE);

  /* TX/RX direction */
  LL_USART_SetTransferDirection(USARTx_INSTANCE, LL_USART_DIRECTION_TX_RX);

  /* 8 data bit, 1 start bit, 1 stop bit, no parity */
  LL_USART_ConfigCharacter(USARTx_INSTANCE, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

  /* No Hardware Flow control */
  /* Reset value is LL_USART_HWCONTROL_NONE */
   LL_USART_SetHWFlowCtrl(USARTx_INSTANCE, LL_USART_HWCONTROL_NONE);

  /* Oversampling by 16 */
  /* Reset value is LL_USART_OVERSAMPLING_16 */
   LL_USART_SetOverSampling(USARTx_INSTANCE, LL_USART_OVERSAMPLING_16);

  /* Set Baudrate to 115200 using APB frequency set to 100000000/APB_Div Hz */
  /* Frequency available for USART peripheral can also be calculated through LL RCC macro */
  /* Ex :
      Periphclk = LL_RCC_GetUSARTClockFreq(Instance); or LL_RCC_GetUARTClockFreq(Instance); depending on USART/UART instance

      In this example, Peripheral Clock is expected to be equal to 100000000/APB_Div Hz => equal to SystemCoreClock/APB_Div
  */
  LL_USART_SetBaudRate(USARTx_INSTANCE, SystemCoreClock/APB_Div, LL_USART_OVERSAMPLING_16, 115200);

  /* (5) Enable USART *********************************************************/
  LL_USART_Enable(USARTx_INSTANCE);
}

int uart_write(uint8_t *data, uint32_t length)
{
  return HAL_UART_Transmit(&uart_handle, data, length, 0xFFFF);
}

int uart_read(uint8_t *dest, uint32_t length)
{
  return HAL_UART_Receive(&uart_handle, (uint8_t *) dest, length, 0xFFFF);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *handle)
{
}

void FLN_UART_IRQHandler(void)
{
  HAL_UART_IRQHandler(&uart_handle);
}

void uart_init(void)
{
  uart_module_init();
}

void uart_process(void)
{
  unsigned char read_char;
  if (uart_read(&read_char, 1) == HAL_OK) {
    uart_write(&read_char, 1);
  }
}
