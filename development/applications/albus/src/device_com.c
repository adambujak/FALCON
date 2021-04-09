#include "device_com.h"

#include "falcon_common.h"

#include "radio.h"
#include "uart.h"
#include "system_time.h"

#include "falcon_packet.h"
#include "fp_decode.h"
#include "fs_decoder.h"
#include "fp_encode.h"
#include "ff_encoder.h"

static uint8_t uart_rx_buffer[MAX_FRAME_SIZE];
static uint8_t rf_tx_buffer[MAX_FRAME_SIZE];
static fs_decoder_t decoder;
static uint32_t last_rf_tx_time;

static void decoder_callback(uint8_t *data, fp_type_t packetType)
{
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wswitch"
  switch (packetType) {
  case FPT_FLIGHT_CONTROL_COMMAND:
  {
    fpc_flight_control_t control = {{0}};
    fpc_flight_control_decode(data, &control);
    LOG_DEBUG("received motor cmd\r\n");
    LOG_DEBUG("MOTOR COMMAND: %f, %f, %f, %f\r\n",
           control.fcsControlCmd.yaw,
           control.fcsControlCmd.pitch,
           control.fcsControlCmd.roll,
           control.fcsControlCmd.alt);
  }
    break;
  }
  #pragma GCC diagnostic pop
}

static void decode_frame(uint8_t *data, uint32_t length)
{
  fs_decoder_decode(&decoder, data, length);
}

static void decoder_init(void)
{
   fs_decoder_config_t decoder_config = {
     .callback = decoder_callback
   };
   fs_decoder_init(&decoder, &decoder_config);
}

void temp_func(void)
{
  ff_encoder_t encoder;
  ff_encoder_init(&encoder);
  ff_encoder_set_buffer(&encoder, rf_tx_buffer);

  fpc_flight_control_t control = {
    {
      1.2,1.4,1.6,1.8
    }
  };

  if (ff_encoder_append_packet(&encoder, &control, FPT_FLIGHT_CONTROL_COMMAND) == FLN_ERR) {
    error_handler();
  }

  ff_encoder_append_footer(&encoder);
}

static void handle_uart(void)
{
  if (uart_read(uart_rx_buffer, MAX_FRAME_SIZE) == MAX_FRAME_SIZE) {
    decode_frame(uart_rx_buffer, MAX_FRAME_SIZE);
    LOG_DEBUG("received frame\r\n");
  }
}

static void handle_rf(void)
{
  radio_process();

  uint8_t temp[32];
  if (radio_data_get(temp, 32)) {
    fs_decoder_decode(&decoder, temp, 32);
  }

  uint32_t now = system_time_get();
  if (system_time_cmp_ms(last_rf_tx_time, now) > 500) {
    radio_data_send(rf_tx_buffer, 32);
    last_rf_tx_time = now;
  }
}

void device_com_task(void *pvParameters)
{
  LOG_DEBUG("Device com task started\r\n");

  while(1) {
    handle_uart();
    handle_rf();
  }
}

void device_com_setup(void)
{
  last_rf_tx_time = system_time_get();
  decoder_init();
  radio_init();
  temp_func();
}

void device_com_start(void)
{
  int32_t taskStatus;

  taskStatus = xTaskCreate(device_com_task,
                        "device_com_task",
                        1024,
                        NULL,
                        tskIDLE_PRIORITY + 1,
                        NULL);

  ASSERT(taskStatus == pdTRUE);
}
