#include "device_com.h"

#include "falcon_packet.h"
#include "ff_encoder.h"
#include "fs_decoder.h"
#include "fp_decode.h"
#include "fp_encode.h"

#include "radio.h"
#include "fifo.h"
#include "system_time.h"
#include "flight_control.h"

#include <string.h>

#define min(a, b)          (((a) > (b)) ? b : a)

#define RTOS_TIMEOUT_TICKS 25
#define PACKET_BUFFER_SIZE 256
#define RX_BUFFER_SIZE     128
#define RF_TX_INTERVAL_MS  300
#define STATUS_INTERVAL_MS 800

typedef struct {
  fifo_t packet_fifo;
  uint8_t packet_buffer[PACKET_BUFFER_SIZE];
  uint32_t last_tx_time;
  TimerHandle_t watchdog_timer;
} radio_manager_t;

static uint32_t last_status_process_time;

static radio_manager_t radio_manager;
static uint8_t decode_buffer[RX_BUFFER_SIZE];

static fs_decoder_t decoder;
static ff_encoder_t encoder;

static int cnt = 0;

static void radio_watchdog_timeout(TimerHandle_t xTimer)
{
  LOG_WARN("Radio RESET TIMEOUT!!\r\n");
  // TODO remove later
  error_handler();
  LOG_WARN("radio status: %d\r\n", radio_status_get());
  radio_reset();
}

static void radio_watchdog_pet(void)
{
  BaseType_t reset_status = xTimerReset(radio_manager.watchdog_timer, 0);
  if (reset_status != pdPASS) {
    LOG_DEBUG("radio reset timer reset failed\r\n");
  }
}

static void decoder_callback(uint8_t *data, fp_type_t packetType)
{
  switch (packetType) {
    case FPT_FLIGHT_CONTROL_COMMAND: {
      fpc_flight_control_t controlInput = {};
      fpc_flight_control_decode(data, &controlInput);
      flight_control_set_command_data(&controlInput);
      LOG_DEBUG("CONTROL INPUT: %f, %f, %f, %f\r\n",
                controlInput.fcsControlCmd.yaw,
                controlInput.fcsControlCmd.pitch,
                controlInput.fcsControlCmd.roll,
                controlInput.fcsControlCmd.alt);
    } break;
    case FPT_ATTITUDE_PARAMS_COMMAND:
    case FPT_YAW_PARAMS_COMMAND: 
    case FPT_ALT_PARAMS_COMMAND: {
      LOG_DEBUG("PARAMETER UPDATE COMMAND\r\n");
      flight_control_set_controller_params(data, packetType);
    } break;
    case FPT_FCS_MODE_COMMAND: {
      fpc_fcs_mode_t fcsMode = {};
      fpc_fcs_mode_decode(data, &fcsMode);
      flight_control_set_mode(fcsMode.mode);
      LOG_DEBUG("MODE COMMAND: %d\r\n", fcsMode.mode);
    } break;
    case FPT_TEST_QUERY: {
      LOG_DEBUG("TEST QUERY RECEIVED\r\n");
      fpr_test_t response = {cnt++};
      uint8_t buffer[MAX_PACKET_SIZE];
      uint8_t length = fpr_test_encode(buffer, &response);
      device_com_send_packet(buffer, length);
    } break;
    case FPT_CALIBRATE_COMMAND: {
      LOG_DEBUG("CALIBRATION COMMAND\r\n");
      flight_control_calibrate_sensors();
    }
    default:
      break;
  }
}

static void status_process(void)
{
  uint32_t now = system_time_get();
  if (system_time_cmp_ms(last_status_process_time, now) < STATUS_INTERVAL_MS) {
    return;
  }

  last_status_process_time = now;
  uint8_t length;
  uint8_t packet_buffer[MAX_PACKET_SIZE];

  fe_flight_mode_t flight_mode;
  if (flight_control_get_mode(&flight_mode) != FLN_OK) {
    LOG_ERROR("error getting flight mode\r\n");
    return;
  }

  if (flight_mode >= FE_FLIGHT_MODE_FCS_READY) {
    fpr_status_t status_response;
    flight_control_get_outputs(&status_response);
    length = fpr_status_encode(packet_buffer, &status_response);
    device_com_send_packet(packet_buffer, length);
  }
}

static void decoder_init()
{
  fs_decoder_config_t decoder_config = {.callback = decoder_callback};
  fs_decoder_init(&decoder, &decoder_config);
}

static void encoder_init(void)
{
  ff_encoder_init(&encoder);
}

static void rf_tx(void)
{
  uint32_t now = system_time_get();
  if (system_time_cmp_ms(radio_manager.last_tx_time, now) < RF_TX_INTERVAL_MS) {
    return;
  }

  uint8_t length;
  uint8_t frame_buffer[MAX_FRAME_SIZE];
  uint8_t packet_buffer[MAX_PACKET_SIZE];

  memset(frame_buffer, 0, MAX_FRAME_SIZE);
  memset(packet_buffer, 0, MAX_PACKET_SIZE);

  ff_encoder_set_buffer(&encoder, frame_buffer);
  uint32_t packet_cnt = 0;

  while (fifo_peek(&radio_manager.packet_fifo, &length, 1) == 1) {
    packet_cnt++;
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

  // Don't send empty frame from hedwig to albus
  if (packet_cnt == 0) {
    return;
  }

  uint8_t frame_length = ff_encoder_append_footer(&encoder);
  uint8_t mod = frame_length % FRF_PACKET_SIZE;

  if (mod != 0) {
    memset(frame_buffer + frame_length, 0, FRF_PACKET_SIZE - mod);
    frame_length += (FRF_PACKET_SIZE - mod);
  }

  radio_data_send(frame_buffer, frame_length);
  radio_manager.last_tx_time = now;
}

static inline void rf_process(void)
{
  radio_process();
  rf_tx();

  uint32_t byte_cnt = radio_rx_cnt_get();

  if (byte_cnt == 0) {
    return;
  }

  byte_cnt = min(byte_cnt, RX_BUFFER_SIZE);

  radio_watchdog_pet();
  radio_data_get(decode_buffer, byte_cnt);
  fs_decoder_decode(&decoder, decode_buffer, byte_cnt);
}

static void device_com_task(void *pvParameters)
{
  LOG_DEBUG("Device com task started\r\n");

  while (1) {
    status_process();
    rf_process();
    logger_process();
    rtos_delay_ms(1);
  }
}

void device_com_send_packet(uint8_t *data, uint8_t length)
{
  fifo_push(&radio_manager.packet_fifo, &length, 1);
  fifo_push(&radio_manager.packet_fifo, data, length);
}

void device_com_setup(void)
{
  radio_init();
  radio_manager.watchdog_timer = xTimerCreate("radio watchdog",
                                 RADIO_WATCHDOG_PERIOD,
                                 pdTRUE,
                                 0,
                                 radio_watchdog_timeout);

  decoder_init();
  encoder_init();
  radio_manager.last_tx_time = 0;
  last_status_process_time = 0;
  ASSERT(fifo_init(&radio_manager.packet_fifo, radio_manager.packet_buffer, PACKET_BUFFER_SIZE) == 0);
}

void device_com_start(void)
{
  BaseType_t timer_status = xTimerStart(radio_manager.watchdog_timer, 0);
  RTOS_ERR_CHECK(timer_status);
  BaseType_t task_status = xTaskCreate(device_com_task,
                           "device_com_task",
                           DEVICE_COM_STACK_SIZE,
                           NULL,
                           device_com_TASK_PRIORITY,
                           NULL);

  RTOS_ERR_CHECK(task_status);
}
