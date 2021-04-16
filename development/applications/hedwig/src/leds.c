#include "leds.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "bsp.h"
#include "falcon_common.h"
#include "logger.h"

#define LED_CMD 0x3A000000

typedef enum {
  LED_PWR_STATE_OFF = 0,
  LED_PWR_STATE_ON
} led_pwr_state_t;

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  led_pwr_state_t pwrState;
} led_state_t;
static led_state_t led_state;

typedef struct {
  volatile bool isDone;
  volatile uint8_t index;
  uint32_t value;
} cmd_state_t;

static cmd_state_t cmd_state;

#define LED_LOW()  FLN_LED_PORT->BSRR = (uint32_t)FLN_LED_PIN << 16
#define LED_HIGH() FLN_LED_PORT->BSRR = FLN_LED_PIN

static void callback(void)
{
  bsp_leds_set(1);
  bsp_leds_set(0);

  if (cmd_state.value & (1 << cmd_state.index)) {
    bsp_leds_set(1);
    bsp_leds_set(0);
  }

  if (cmd_state.index == 0) {
    bsp_leds_timer_stop();
    cmd_state.isDone = true;
    return;
  }
  cmd_state.index--;
}

static void set_led(uint8_t r, uint8_t g, uint8_t b)
{
  bsp_leds_timer_start();
  uint32_t color = (r << 16) | (g << 8) | b;

  cmd_state.isDone = false;
  cmd_state.index = 32;
  cmd_state.value = LED_CMD | color;
}

void leds_set_color(uint8_t r, uint8_t g, uint8_t b)
{
  led_state.r = r;
  led_state.g = g;
  led_state.b = b;
  led_state.pwrState = LED_PWR_STATE_ON;
  set_led(r, g, b);
}

void leds_toggle(void)
{
  if (led_state.pwrState == LED_PWR_STATE_OFF) {
    led_state.pwrState = LED_PWR_STATE_ON;
    set_led(led_state.r, led_state.g, led_state.b);
  }
  else {
    led_state.pwrState = LED_PWR_STATE_OFF;
    set_led(0, 0, 0);
  }
}

void leds_task(void *pvParameters)
{
  while (1) {
    LOG_DEBUG("LED Task\r\n");
    leds_toggle();
    rtos_delay_ms(1500);
  }
}

void leds_task_setup(void)
{
  LED_LOW();
  FLN_ERR_CHECK(bsp_leds_init(callback));
  leds_set_color(0x15, 0x0, 0x10);
}

void leds_task_start(void)
{
  BaseType_t taskStatus = xTaskCreate(leds_task,
                           "led_task",
                           LED_STACK_SIZE,
                           NULL,
                           led_TASK_PRIORITY,
                           NULL);

  RTOS_ERR_CHECK(taskStatus);
}
