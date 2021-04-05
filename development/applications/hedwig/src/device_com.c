#include "device_com.h"

#include "radio.h"

#include "falcon_packet.h"
#include "fs_decoder.h"
#include "fp_decode.h"
#include "ff_encoder.h"

#include <string.h>

#define RTOS_TIMEOUT_TICKS 25

static fs_decoder_t decoder;

static void rx_handler(uint8_t *data, fp_type_t packetType)
{
  switch (packetType) {
    case FPT_FLIGHT_CONTROL_COMMAND:
    {
      fpc_flight_control_t controlInput = {};
      fpc_flight_control_decode(data, &controlInput);
      LOG_DEBUG("CONTROL INPUT: %f, %f, %f, %f\r\n",
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
      LOG_DEBUG("MODE COMMAND: %d\r\n", mode.mode);
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

  while(1) {
    radio_process();
  }
}

void device_com_setup(void)
{
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
                          4*configMINIMAL_STACK_SIZE,
                          NULL,
                          device_com_TASK_PRIORITY,
                          NULL);

  RTOS_ERR_CHECK(taskStatus);
}
