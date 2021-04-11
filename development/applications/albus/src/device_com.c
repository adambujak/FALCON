#include "device_com.h"

#include "falcon_common.h"

#include "radio.h"
#include "frf.h"
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

typedef struct {
  uint32_t last_tx_time;
  fifo_t packet_fifo;
} radio_manager_t;

static radio_manager_t radio_manager;
static uint8_t uart_rx_buffer[MAX_FRAME_SIZE];
static uint8_t rf_rx_buffer[MAX_FRAME_SIZE];
static uint8_t rf_tx_buffer[RF_TX_SIZE];

static fs_decoder_t decoder;
static ff_encoder_t encoder;

static void rf_enqueue_packet(uint8_t *data, uint8_t length);

static void decoder_callback(uint8_t *data, fp_type_t packet_type)
{
  switch (packet_type) {
    //case FPT_FLIGHT_CONTROL_COMMAND: {
    //  fpc_flight_control_t control = {{0}};
    //  fpc_flight_control_decode(data, &control);
    //  LOG_DEBUG("received motor cmd\r\n");
    //  LOG_DEBUG("MOTOR COMMAND: %f, %f, %f, %f\r\n", control.fcsControlCmd.yaw, control.fcsControlCmd.pitch,
    //            control.fcsControlCmd.roll, control.fcsControlCmd.alt);
    //}
    default:
      rf_enqueue_packet(data, fp_get_packet_length(packet_type));
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

static void encoder_init(void)
{
  ff_encoder_init(&encoder);
}

static void handle_uart(void)
{
  if (uart_read(uart_rx_buffer, MAX_FRAME_SIZE) == MAX_FRAME_SIZE) {
    decode_frame(uart_rx_buffer, MAX_FRAME_SIZE);
  }
}

static void rf_enqueue_packet(uint8_t *data, uint8_t length)
{
  fifo_push(&radio_manager.packet_fifo, &length, 1);
  fifo_push(&radio_manager.packet_fifo, data, length);
}

static void rf_tx(void)
{
  uint32_t now = system_time_get();
  if (system_time_cmp_ms(radio_manager.last_tx_time, now) > RF_TX_INTERVAL_MS) {
    uint8_t length;
    uint8_t frame_buffer[MAX_FRAME_SIZE];
    uint8_t packet_buffer[MAX_PACKET_SIZE];

    memset(frame_buffer, 0, MAX_FRAME_SIZE);
    memset(packet_buffer, 0, MAX_PACKET_SIZE);

    ff_encoder_set_buffer(&encoder, frame_buffer);

    while (fifo_peek(&radio_manager.packet_fifo, &length, 1) == 1) {
      // If not enough room in frame, finish up
      if (length > ff_encoder_get_remaining_bytes(&encoder)) {
        break;
      }
      fifo_drop(&radio_manager.packet_fifo, 1);

      if (fifo_pop(&radio_manager.packet_fifo, packet_buffer, length) == -1) {
        LOG_ERROR("Less data than length byte\r\n");
        return;
      }

      if (ff_encoder_append_data(&encoder, packet_buffer, length) == FLN_ERR) {
        LOG_ERROR("Error appending data to frame\r\n");
        error_handler();
      }
    }

    uint8_t frame_length = ff_encoder_append_footer(&encoder);
    if (frame_length < FRF_PACKET_SIZE) {
      memset(frame_buffer + frame_length, 0, FRF_PACKET_SIZE - frame_length);
      frame_length = FRF_PACKET_SIZE;
    }
    radio_data_send(frame_buffer, frame_length);
    radio_manager.last_tx_time = now;
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

void temp_send(void)
{
  static uint32_t time = 0;
  static uint8_t buffer[MAX_FRAME_SIZE];

  if (system_time_cmp_ms(time, system_time_get()) > 300) {

    ff_encoder_t temp_encoder;
    ff_encoder_init(&temp_encoder);
    ff_encoder_set_buffer(&temp_encoder, buffer);

    fpc_flight_control_t control = {{1.2, 1.4, 1.6, 1.8}};

    if (ff_encoder_append_packet(&temp_encoder, &control, FPT_FLIGHT_CONTROL_COMMAND) == FLN_ERR) {
      error_handler();
    }

    ff_encoder_append_footer(&temp_encoder);
    decode_frame(buffer, MAX_FRAME_SIZE);
    time = system_time_get();
  }
}


void device_com_task(void *pvParameters)
{
  LOG_DEBUG("Device com task started\r\n");

  while (1) {
    handle_uart();
    temp_send();
    rf_process();
  }
}

void device_com_setup(void)
{
  decoder_init();
  encoder_init();
  radio_init();

  radio_manager.last_tx_time = system_time_get();
  ASSERT(fifo_init(&radio_manager.packet_fifo, rf_tx_buffer, RF_TX_SIZE) == 0);
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
