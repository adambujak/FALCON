#include "device_com.h"

#include "radio.h"

#include "falcon_packet.h"
#include "fs_decoder.h"
#include "fp_decode.h"
#include "ff_encoder.h"

#include <string.h>

#define RTOS_TIMEOUT_TICKS 25

static fs_decoder_t decoder;

static uint8_t tx_buffer[32];

static void rx_handler(uint8_t *data, fp_type_t packetType)
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

void decoder_callback(uint8_t *data, fp_type_t packetType)
{
  /* Once complete frame is decoded, send response frame with queued packets */
  rx_handler(data, packetType);
}

static void device_com_task(void *pvParameters)
{
  LOG_DEBUG("Device com task started\r\n");

  uint8_t rx_buffer[32];

  while(1) {
    LOG_DEBUG("Device com task process\r\n");
    radio_process();
    if (radio_get_data(rx_buffer, 32) == 32) {
      fs_decoder_decode(&decoder, rx_buffer, 32);
      radio_send_data(tx_buffer, 32);
    }
    rtos_delay_ms(1);
  }
}

static void temp_func()
{
  ff_encoder_t encoder;
  ff_encoder_init(&encoder);
  ff_encoder_set_buffer(&encoder, tx_buffer);

  fpc_flight_control_t control = {
    {
      2.2,2.4,1.6,1.8
    }
  };

  if (ff_encoder_append_packet(&encoder, &control, FPT_FLIGHT_CONTROL_COMMAND) == FLN_ERR) {
    error_handler();
  }

  ff_encoder_append_footer(&encoder);
}


void device_com_setup(void)
{
  temp_func();
  radio_init();

  /* Falcon Packet Decoder Init */
  fs_decoder_config_t decoder_config = {
    .callback = decoder_callback
  };

  fs_decoder_init(&decoder, &decoder_config);
}

void device_com_start(void)
{
  BaseType_t taskStatus = xTaskCreate(device_com_task,
                          "device_com_task",
                          5*configMINIMAL_STACK_SIZE,
                          NULL,
                          device_com_TASK_PRIORITY,
                          NULL);

  RTOS_ERR_CHECK(taskStatus);
}
