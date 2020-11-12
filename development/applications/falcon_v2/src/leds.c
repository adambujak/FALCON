#include "leds.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "logger.h"
#include "falcon_common.h"
#include "bsp.h"

void leds_toggle(void)
{
}

volatile uint8_t i = 32;
uint32_t ledValue = 0x3A0F0F00;

#define LED_LOW() FLN_LED_PORT->BSRR = (uint32_t)FLN_LED_PIN << 16
#define LED_HIGH() FLN_LED_PORT->BSRR = FLN_LED_PIN

static void callback(void)
{
  bsp_leds_set(1);
  bsp_leds_set(0);

  if (ledValue & (1 << i)) {
	bsp_leds_set(1);
	bsp_leds_set(0);
  }

  if (i == 0) {
    bsp_leds_timer_stop();
    return;
  }
  i--;
}

void led_set(uint8_t r, uint8_t g, uint8_t b)
{
  bsp_leds_timer_start();
  ledValue = (0x3A << 24) | (r << 16) | (g << 8) | b;
}

void leds_task_setup(void)
{
  LED_LOW();
  for (int i = 0; i < 300; i++);
  FLN_ERR_CHECK(bsp_leds_init(callback));
  led_set(0x0,0x0f,0);
}

void leds_task(void *pvParameters)
{
 while (1) {
	  DEBUG_LOG("LED Task\r\n");
    vTaskDelay(1500);
  }
}
