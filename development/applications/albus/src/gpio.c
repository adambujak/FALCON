#include "gpio.h"

#include "falcon_common.h"
#include "board.h"

#include <stdlib.h>
#include <stdint.h>

static exti_cb_t rf_irq_cb = NULL;

#define PIN_WRITE(pin, port, value)                       \
  if ((value) == 0) {                                     \
    LL_GPIO_ResetOutputPin((port), (pin));                \
  }                                                       \
  else {                                                  \
    LL_GPIO_SetOutputPin((port), (pin));                  \
  }                                                       \

static void rf_ce_pin_init(void)
{
  RF_GPIO_CE_CLK_EN();
  LL_GPIO_InitTypeDef gpio_config = {0};

  LL_GPIO_ResetOutputPin(RF_CE_GPIO_PORT, RF_CE_PIN);
  gpio_config.Pin = RF_CE_PIN;
  gpio_config.Mode = LL_GPIO_MODE_OUTPUT;
  gpio_config.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  gpio_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  gpio_config.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(RF_CE_GPIO_PORT, &gpio_config);
}

static void rf_cs_pin_init(void)
{
  RF_GPIO_SS_CLK_EN();
  LL_GPIO_InitTypeDef gpio_config = {0};

  LL_GPIO_ResetOutputPin(RF_SS_GPIO_PORT, RF_SPI_SS_PIN);
  gpio_config.Pin = RF_SPI_SS_PIN;
  gpio_config.Mode = LL_GPIO_MODE_OUTPUT;
  gpio_config.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  gpio_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  gpio_config.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(RF_SS_GPIO_PORT, &gpio_config);
}

static void rf_irq_pin_init(void)
{
  RF_GPIO_IRQ_CLK_EN();

  LL_EXTI_InitTypeDef exti_config = {0};

  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE10);

  exti_config.Line_0_31 = RF_IRQ_EXTI_LINE;
  exti_config.LineCommand = ENABLE;
  exti_config.Mode = LL_EXTI_MODE_IT;
  exti_config.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&exti_config);

  LL_GPIO_SetPinPull(RF_IRQ_GPIO_PORT, RF_IRQ_PIN, LL_GPIO_PULL_DOWN);
  LL_GPIO_SetPinMode(RF_IRQ_GPIO_PORT, RF_IRQ_PIN, LL_GPIO_MODE_INPUT);

  NVIC_SetPriority(RF_IRQn, 1);
  NVIC_EnableIRQ(RF_IRQn);
}

void gpio_rf_ce_write(uint8_t value)
{
  PIN_WRITE(RF_CE_PIN, RF_CE_GPIO_PORT, value);
}

void gpio_rf_cs_write(uint8_t value)
{
  PIN_WRITE(RF_SPI_SS_PIN, RF_SS_GPIO_PORT, value);
}

void gpio_rf_irq_register(exti_cb_t callback)
{
  rf_irq_cb = callback;
}

void gpio_init(void)
{
  rf_ce_pin_init();
  rf_cs_pin_init();
  rf_irq_pin_init();
}

void RF_IRQHandler(void)
{
  DISABLE_IRQ();
  LL_EXTI_ClearFlag_0_31(RF_IRQ_EXTI_LINE);
  if (rf_irq_cb != NULL) {
    rf_irq_cb();
  }
  ENABLE_IRQ();
}
