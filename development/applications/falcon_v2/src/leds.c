#include "leds.h"

#include <stdio.h>
#include <assert.h>

#include "logger.h"
#include "falcon_common.h"
#include "bsp.h"

void leds_toggle(void)
{
  bsp_leds_toggle();
}

void leds_task_setup(void)
{
  FLN_ERR_CHECK(bsp_leds_init());
}

void leds_task(void *pvParameters)
{
 while (1) {
	  DEBUG_LOG("LED Task1\r\n");
    bsp_leds_toggle();
    vTaskDelay(1500);
  }
}

