#include "leds.h"

#include "logger.h"
#include "falcon_common.h"
#include "bsp.h"

void leds_toggle(void)
{
  bsp_leds_toggle(FLN_LED_3);
}

void leds_task_setup(void)
{
  FLN_ERR_CHECK(bsp_leds_init(LED1_PIN | LED2_PIN | LED3_PIN));
}

void leds_task(void *pvParameters)
{
  while (1) {
    DEBUG_LOG("BLINKING LED\r\n");
    bsp_leds_toggle(LED1_PIN | LED2_PIN);
    vTaskDelay(500);
  }
}
