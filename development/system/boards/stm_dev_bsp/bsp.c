#include "bsp.h"

#define max(a,b) (a)>(b)?a:b
static int SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef  ret = HAL_OK;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM       = 8;
  RCC_OscInitStruct.PLL.PLLN       = 200;
  RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ       = 7;
  RCC_OscInitStruct.PLL.PLLR       = 2;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if (ret != HAL_OK) {
    return FLN_ERR;
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
  if (ret != HAL_OK) {
    return FLN_ERR;
  }

  return FLN_OK;
}

void bsp_board_bringup(void)
{
  HAL_Init();

  FLN_ERR_CHECK(SystemClock_Config());
}

int bsp_leds_init(void)
{
  FLN_LED_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  GPIO_InitStruct.Pin = FLN_LED_PIN;

  HAL_GPIO_Init(FLN_LED_PORT, &GPIO_InitStruct);

  return FLN_OK;
}

void bsp_leds_toggle()
{
  HAL_GPIO_TogglePin(FLN_LED_PORT, FLN_LED_PIN);
}

int bsp_uart_init(fln_uart_handle_t *handle)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  FLN_UART_TX_GPIO_CLK_ENABLE();
  FLN_UART_RX_GPIO_CLK_ENABLE();

  FLN_UART_CLK_ENABLE();

  GPIO_InitStruct.Pin       = FLN_UART_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = FLN_UART_TX_AF;

  HAL_GPIO_Init(FLN_UART_TX_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin       = FLN_UART_RX_PIN;
  GPIO_InitStruct.Alternate = FLN_UART_RX_AF;

  HAL_GPIO_Init(FLN_UART_RX_GPIO_PORT, &GPIO_InitStruct);

  handle->Instance = FLN_UART;

  handle->Init.BaudRate     = FLN_UART_BAUDRATE;
  handle->Init.WordLength   = UART_WORDLENGTH_9B;
  handle->Init.StopBits     = UART_STOPBITS_1;
  handle->Init.Parity       = UART_PARITY_ODD;
  handle->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  handle->Init.Mode         = UART_MODE_TX_RX;
  handle->Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(handle) != HAL_OK) {
    return FLN_ERR;
  }
  return FLN_OK;
}

void bsp_uart_put_char(fln_uart_handle_t *handle, uint8_t *ch)
{
  HAL_UART_Transmit(handle, ch, 1, 0xFFFF);
}

void bsp_uart_write(fln_uart_handle_t *handle, uint8_t *data, uint16_t length)
{
  HAL_UART_Transmit(handle, data, length, 0xFFFF);
}

int bsp_i2c_init(fln_i2c_handle_t *handle)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  FLN_SENSORS_I2C_SCL_GPIO_CLK_ENABLE();
  FLN_SENSORS_I2C_SDA_GPIO_CLK_ENABLE();
  FLN_SENSORS_I2C_CLK_ENABLE();

  FLN_SENSORS_I2C_FORCE_RESET();
  FLN_SENSORS_I2C_RELEASE_RESET();

  GPIO_InitStruct.Pin       = FLN_SENSORS_I2C_SCL_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = FLN_SENSORS_I2C_SCL_SDA_AF;
  HAL_GPIO_Init(FLN_SENSORS_I2C_SCL_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin       = FLN_SENSORS_I2C_SDA_PIN;
  GPIO_InitStruct.Alternate = FLN_SENSORS_I2C_SCL_SDA_AF;
  HAL_GPIO_Init(FLN_SENSORS_I2C_SDA_GPIO_PORT, &GPIO_InitStruct);

  handle->Instance             = FLN_SENSORS_I2C;
  handle->Init.ClockSpeed      = FLN_SENSORS_I2C_SPEEDCLOCK;
  handle->Init.DutyCycle       = FLN_SENSORS_I2C_DUTYCYCLE;
  handle->Init.OwnAddress1     = 0xFF;
  handle->Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  handle->Init.OwnAddress2     = 0xFF;
  handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  handle->Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

  if(HAL_I2C_Init(handle) != HAL_OK) {
    return FLN_ERR;
  }
  return FLN_OK;
}

int bsp_i2c_write(fln_i2c_handle_t *handle,
             uint8_t slave_addr,
             uint8_t reg_addr,
             uint16_t length,
             uint8_t *data)
{
  while(HAL_I2C_Mem_Write(handle, ((uint16_t)slave_addr<<1), reg_addr, 1U, data, length, 25U)) {
    if (HAL_I2C_GetError(handle) != HAL_I2C_ERROR_AF) {
      return FLN_ERR;
    }
  }
  return FLN_OK;
}

int bsp_i2c_read(fln_i2c_handle_t *handle,
             uint8_t slave_addr,
             uint8_t reg_addr,
             uint16_t length,
             uint8_t *data)
{
  while(HAL_I2C_Mem_Read(handle, ((uint16_t)slave_addr<<1), reg_addr, 1U, data, length, 25U)) {
    if (HAL_I2C_GetError(handle) != HAL_I2C_ERROR_AF) {
      return FLN_ERR;
    }
  }
  return FLN_OK;
}

/************************************************************
 ************************** Motors **************************
 ***********************************************************/

static TIM_HandleTypeDef motorTimerHandle;

int bsp_motors_init(void)
{
  GPIO_InitTypeDef   GPIO_InitStruct;

  FLN_MOTOR_TIMER_CLK_ENABLE();
  FLN_MOTOR_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  GPIO_InitStruct.Alternate = FLN_MOTOR_GPIO_AF_CHANNEL1;
  GPIO_InitStruct.Pin = FLN_MOTOR_GPIO_PIN_CHANNEL1;
  HAL_GPIO_Init(FLN_MOTOR_GPIO_PORT_CHANNEL1, &GPIO_InitStruct);

  GPIO_InitStruct.Alternate = FLN_MOTOR_GPIO_AF_CHANNEL2;
  GPIO_InitStruct.Pin = FLN_MOTOR_GPIO_PIN_CHANNEL2;
  HAL_GPIO_Init(FLN_MOTOR_GPIO_PORT_CHANNEL2, &GPIO_InitStruct);

  GPIO_InitStruct.Alternate = FLN_MOTOR_GPIO_AF_CHANNEL3;
  GPIO_InitStruct.Pin = FLN_MOTOR_GPIO_PIN_CHANNEL3;
  HAL_GPIO_Init(FLN_MOTOR_GPIO_PORT_CHANNEL3, &GPIO_InitStruct);

  GPIO_InitStruct.Alternate = FLN_MOTOR_GPIO_AF_CHANNEL4;
  GPIO_InitStruct.Pin = FLN_MOTOR_GPIO_PIN_CHANNEL4;
  HAL_GPIO_Init(FLN_MOTOR_GPIO_PORT_CHANNEL4, &GPIO_InitStruct);


  uint32_t motorTimerPrescalerValue = (uint32_t)(SystemCoreClock / 1000000) - 1;

  motorTimerHandle.Instance = FLN_MOTOR_TIMER;

  motorTimerHandle.Init.Prescaler         = motorTimerPrescalerValue;
  motorTimerHandle.Init.Period            = 20000;
  motorTimerHandle.Init.ClockDivision     = 0;
  motorTimerHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  motorTimerHandle.Init.RepetitionCounter = 0;
  motorTimerHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_PWM_Init(&motorTimerHandle) != HAL_OK) {
    return FLN_ERR;
  }
	return FLN_OK;
}


void bsp_motors_pwm_set_us(uint8_t motor, uint16_t us)
{
  ASSERT(us >= 1000 && us <= 2000);

  static uint32_t motorChannels[] = {TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4};
  static TIM_OC_InitTypeDef pwmConfig = {
    .OCMode       = TIM_OCMODE_PWM1,
    .OCPolarity   = TIM_OCPOLARITY_HIGH,
    .OCFastMode   = TIM_OCFAST_DISABLE,
    .OCNPolarity  = TIM_OCNPOLARITY_HIGH,
    .OCNIdleState = TIM_OCNIDLESTATE_RESET,
    .OCIdleState  = TIM_OCIDLESTATE_RESET
  };

  pwmConfig.Pulse = us;
  uint32_t channel = motorChannels[motor-1];

  if (HAL_TIM_PWM_ConfigChannel(&motorTimerHandle, &pwmConfig, channel) != HAL_OK) {
    error_handler();
  }

  if (HAL_TIM_PWM_Start(&motorTimerHandle, channel) != HAL_OK) {
    error_handler();
  }
}

/* RF */
static SPI_HandleTypeDef rfSpiHandle;

int bsp_rf_spi_init(void)
{
  rfSpiHandle.Instance               = RF_SPI;
  rfSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  rfSpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  rfSpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
  rfSpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
  rfSpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  rfSpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  rfSpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  rfSpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  rfSpiHandle.Init.CRCPolynomial     = 7;
  rfSpiHandle.Init.NSS               = SPI_NSS_SOFT;
  rfSpiHandle.Init.Mode              = SPI_MODE_MASTER;

  GPIO_InitTypeDef  GPIO_InitStruct;

  if (rfSpiHandle.Instance == RF_SPI) {
    RF_SPI_SCK_GPIO_CLK_ENABLE();
    RF_SPI_MISO_GPIO_CLK_ENABLE();
    RF_SPI_MOSI_GPIO_CLK_ENABLE();
    RF_SPI_CLK_ENABLE();

    if(HAL_SPI_Init(&rfSpiHandle) != HAL_OK)
    {
      /* Initialization Error */
      return FLN_ERR;
    }

    GPIO_InitStruct.Pin       = RF_SPI_SCK_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = RF_SPI_SCK_AF;
    HAL_GPIO_Init(RF_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RF_SPI_MISO_PIN;
    GPIO_InitStruct.Alternate = RF_SPI_MISO_AF;
    HAL_GPIO_Init(RF_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RF_SPI_MOSI_PIN;
    GPIO_InitStruct.Alternate = RF_SPI_MOSI_AF;
    HAL_GPIO_Init(RF_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);
  }
  return FLN_OK;
}

void bsp_rf_spi_deinit(void)
{
  if(rfSpiHandle.Instance == RF_SPI) {
    RF_SPI_FORCE_RESET();
    RF_SPI_RELEASE_RESET();

    HAL_GPIO_DeInit(RF_SPI_SCK_GPIO_PORT, RF_SPI_SCK_PIN);
    HAL_GPIO_DeInit(RF_SPI_MISO_GPIO_PORT, RF_SPI_MISO_PIN);
    HAL_GPIO_DeInit(RF_SPI_MOSI_GPIO_PORT, RF_SPI_MOSI_PIN);
  }
}

int bsp_rf_transceive(uint8_t *txBuffer, uint16_t txSize, uint8_t *rxBuffer, uint16_t rxSize)
{
  if (txSize == 0 || txBuffer == NULL) {
    hal_custom_receive(&rfSpiHandle, rxBuffer, rxSize, 5000);
  }
  else if (rxSize == 0 || rxBuffer == NULL) {
    hal_custom_transmit(&rfSpiHandle, txBuffer, txSize, 5000);
  }
  else {
	  HAL_SPI_TransmitReceive(&rfSpiHandle, txBuffer, rxBuffer, max(rxSize,txSize), 5000);
  }
  return FLN_OK;
}

int bsp_rf_gpio_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  RF_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  GPIO_InitStruct.Pin = RF_CE_PIN;
  HAL_GPIO_Init(RF_CE_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RF_SPI_SS_PIN;
  HAL_GPIO_Init(RF_SPI_SS_GPIO_PORT, &GPIO_InitStruct);

  bsp_rf_cs_set(1);
  bsp_rf_ce_set(0);

  RF_IRQ_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  GPIO_InitStruct.Pin = RF_IRQ_PIN;
  HAL_GPIO_Init(RF_IRQ_GPIO_PORT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  return FLN_OK;
}

void bsp_rf_cs_set(uint8_t value)
{
  HAL_GPIO_WritePin(RF_SPI_SS_GPIO_PORT, RF_SPI_SS_PIN, value);
}

void bsp_rf_ce_set(uint8_t value)
{
  HAL_GPIO_WritePin(RF_CE_GPIO_PORT, RF_CE_PIN, value);
}

static void (*rfISRCallback) (void);
int bsp_rf_init(void (*isrCallback) (void))
{
  rfISRCallback = isrCallback;
  if (bsp_rf_spi_init() != FLN_OK) {
    return FLN_ERR;
  }
  if (bsp_rf_gpio_init() != FLN_OK) {
    return FLN_ERR;
  }
  return FLN_OK;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == RF_IRQ_PIN)
  {
    rfISRCallback();
  }
}

void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(RF_IRQ_PIN);
}

void error_handler(void)
{
  while (1);
}
