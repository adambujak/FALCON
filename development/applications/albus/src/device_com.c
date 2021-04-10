#include "device_com.h"

#include "falcon_common.h"

#include "radio.h"
#include "system_time.h"
#include "uart.h"
#include "fifo.h"

#include "falcon_packet.h"
#include "ff_encoder.h"
#include "fp_decode.h"
#include "fp_encode.h"
#include "fs_decoder.h"

#define min(a, b)          (((a) > (b)) ? b : a)
#define RF_TX_SIZE         512
#define RF_TX_INTERVAL_MS  200

static uint8_t uart_rx_buffer[MAX_FRAME_SIZE];
static uint8_t rf_rx_buffer[MAX_FRAME_SIZE];
static uint8_t rf_tx_buffer[RF_TX_SIZE];

static fifo_t rf_tx_fifo;

static fs_decoder_t decoder;
static uint32_t last_rf_tx_time;

static void decoder_callback(uint8_t *data, fp_type_t packetType)
{
  switch (packetType) {
    case FPT_FLIGHT_CONTROL_COMMAND: {
      fpc_flight_control_t control = {{0}};
      fpc_flight_control_decode(data, &control);
      LOG_DEBUG("received motor cmd\r\n");
      LOG_DEBUG("MOTOR COMMAND: %f, %f, %f, %f\r\n", control.fcsControlCmd.yaw, control.fcsControlCmd.pitch,
                control.fcsControlCmd.roll, control.fcsControlCmd.alt);
    } break;
    default:
      break;
  }
}

static void decode_frame(uint8_t *data, uint32_t length)
{
  fs_decoder_decode(&decoder, data, length);
}

static void decoder_init(void)
{
  fs_decoder_config_t decoder_config = {.callback = decoder_callback};
  fs_decoder_init(&decoder, &decoder_config);
}

static void handle_uart(void)
{
  if (uart_read(uart_rx_buffer, MAX_FRAME_SIZE) == MAX_FRAME_SIZE) {
    decode_frame(uart_rx_buffer, MAX_FRAME_SIZE);
  }
}

static void rf_enqueue(uint8_t *data, uint32_t length)
{
  uint8_t length8 = (uint8_t) length;
  fifo_push(&rf_tx_fifo, &length8, 1);
  fifo_push(&rf_tx_fifo, data, length);
}

static void rf_tx(void)
{
  uint32_t now = system_time_get();
  if (system_time_cmp_ms(last_rf_tx_time, now) > RF_TX_INTERVAL_MS) {
    uint8_t length;
    uint8_t buffer[RF_TX_SIZE];

    if (fifo_pop(&rf_tx_fifo, &length, 1) == 1) {
      if (fifo_pop(&rf_tx_fifo, buffer, length) == -1) {
        LOG_ERROR("Less data than length byte\r\n");
        return;
      }
    }
    else {
      // If no data in fifo, just send 0xFF
      length = 32;
      memset(buffer, 0xFF, length);
      return;
    }

    radio_data_send(buffer, length);
    last_rf_tx_time = now;
  }
}

static void rf_process(void)
{
  radio_process();
  rf_tx();
  uint32_t byte_cnt = radio_rx_cnt_get();

  if (byte_cnt == 0) {
    return;
  }

  byte_cnt = min(byte_cnt, MAX_FRAME_SIZE);

  if (radio_data_get(rf_rx_buffer, byte_cnt)) {
    fs_decoder_decode(&decoder, rf_rx_buffer, byte_cnt);
  }
}

void temp_func(uint8_t *buffer)
{
  ff_encoder_t encoder;
  ff_encoder_init(&encoder);
  ff_encoder_set_buffer(&encoder, buffer);

  fpc_flight_control_t control = {{1.2, 1.4, 1.6, 1.8}};

  if (ff_encoder_append_packet(&encoder, &control, FPT_FLIGHT_CONTROL_COMMAND) == FLN_ERR) {
    error_handler();
  }

  ff_encoder_append_footer(&encoder);
}

void device_com_task(void *pvParameters)
{
  LOG_DEBUG("Device com task started\r\n");

  uint32_t time = system_time_get();
  uint8_t buffer[32];
  temp_func(buffer);
  while (1) {
    handle_uart();
    rf_process();
    if (system_time_cmp_ms(time, system_time_get()) > 400) {
      rf_enqueue(buffer, 32);
      time = system_time_get();
    }
  }
}

void device_com_setup(void)
{
  last_rf_tx_time = system_time_get();
  decoder_init();
  radio_init();

  ASSERT(fifo_init(&rf_tx_fifo, rf_tx_buffer, RF_TX_SIZE) == 0);
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
