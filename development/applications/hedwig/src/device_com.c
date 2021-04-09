#include "device_com.h"

#include "radio.h"

#include "falcon_packet.h"
#include "fs_decoder.h"
#include "fp_decode.h"
#include "ff_encoder.h"

#include <string.h>

#define min(a,b) (((a) > (b)) ? b : a)

#define RTOS_TIMEOUT_TICKS 25
#define BUFFER_SIZE        128

static fs_decoder_t decoder;

static uint8_t decode_buffer[BUFFER_SIZE];
static TimerHandle_t radio_reset_timer;

static void radio_reset_timeout(TimerHandle_t xTimer)
{
  LOG_WARN("Radio RESET TIMEOUT!!\r\n");
  radio_reset();
}

static void radio_reset_timer_pet(void)
{
  BaseType_t reset_status = xTimerReset(radio_reset_timer, 0);
  if (reset_status != pdPASS) {
    LOG_DEBUG("radio reset timer reset failed\r\n");
  }
}

static inline void rf_process(void)
{
  radio_process();
  uint32_t byte_cnt = radio_rx_cnt_get();

  if (byte_cnt == 0) {
    return;
  }

  byte_cnt = min(byte_cnt, BUFFER_SIZE);

  radio_reset_timer_pet();
  radio_data_get(decode_buffer, byte_cnt);
  fs_decoder_decode(&decoder, decode_buffer, byte_cnt);
}

void decoder_callback(uint8_t *data, fp_type_t packetType)
{
  switch (packetType) {
    case FPT_FLIGHT_CONTROL_COMMAND:
    {
      fpc_flight_control_t controlInput = {};
      fpc_flight_control_decode(data, &controlInput);
      LOG_INFO("CONTROL INPUT: %f, %f, %f, %f\r\n",
                controlInput.fcsControlCmd.yaw,
                controlInput.fcsControlCmd.pitch,
                controlInput.fcsControlCmd.roll,
                controlInput.fcsControlCmd.alt);
    }
    break;
    case FPT_MODE_COMMAND:
    {
      fpc_mode_t mode = {};
      fpc_mode_decode(data, &mode);
      LOG_INFO("MODE COMMAND: %d\r\n", mode.mode);
    }
    break;
    default:
      break;
  }
}

static void device_com_task(void *pvParameters)
{
  LOG_DEBUG("Device com task started\r\n");

  while(1) {
    rf_process();
    rtos_delay_ms(1);
  }
}

void device_com_setup(void)
{
  radio_init();
  radio_reset_timer = xTimerCreate("reset timer", 1000, pdTRUE, 0, radio_reset_timeout);
  /* Falcon Packet Decoder Init */
  fs_decoder_config_t decoder_config = {
    .callback = decoder_callback
  };

  fs_decoder_init(&decoder, &decoder_config);
}

void device_com_start(void)
{
  BaseType_t timer_status  = xTimerStart(radio_reset_timer, 0);
  RTOS_ERR_CHECK(timer_status);
  BaseType_t task_status = xTaskCreate(device_com_task,
                           "device_com_task",
                           DEVICE_COM_STACK_SIZE,
                           NULL,
                           device_com_TASK_PRIORITY,
                           NULL);

  RTOS_ERR_CHECK(task_status);
}
