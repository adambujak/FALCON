#include "device_com.h"

#include "falcon_common.h"
#include "radio_common.h"
#include "frf.h"
#include "bsp.h"

#include "falcon_packet.h"
#include "fs_decoder.h"
#include "fp_decode.h"

#include <string.h>

#define RTOS_TIMEOUT_TICKS 25

static fs_decoder_t decoder;

static uint8_t hedwigAddress[RADIO_ADDRESS_LENGTH];
static uint8_t albusAddress[RADIO_ADDRESS_LENGTH];

uint8_t packetCnt = 0;
uint8_t frame_buffer[MAX_FRAME_SIZE];

static frf_t radio;
static volatile bool rfRxReady = false;
static SemaphoreHandle_t frfMutex;

static void rf_spi_transfer(void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
  bsp_rf_transceive(tx_buf, tx_len, rx_buf, rx_len);
}

static inline void rfISR(void)
{
  frf_isr(&radio);
}

static inline void lock_frf(void)
{
  xSemaphoreTake(frfMutex, RTOS_TIMEOUT_TICKS);
}

static inline void unlock_frf(void)
{
  xSemaphoreGive(frfMutex);
}

static inline void createFRFMutex(void)
{
  frfMutex = xSemaphoreCreateMutex();
  if (frfMutex == NULL) {
    error_handler();
  }
}

static inline void handleRFRx(void)
{
  if (rfRxReady) {
    frf_packet_t packet;
    while (frf_getPacket(&radio, packet) == 0) {
      /* Convert frf packet to byte array - in this case just cast */
      uint8_t *buffer = (uint8_t *) packet;
      memcpy(&frame_buffer[packetCnt*FRF_PACKET_SIZE], buffer, FRF_PACKET_SIZE);
      packetCnt++;
      //fp_decoder_decode(&decoder, buffer);
    }
    fs_decoder_decode(&decoder, frame_buffer, FRF_PACKET_SIZE*packetCnt);
    packetCnt = 0;

    rfRxReady = false;
  }
}

static inline void rfProcess(void)
{
  lock_frf();
  handleRFRx();
  frf_process(&radio);
  unlock_frf();
}

static void rf_event_callback(frf_event_t event)
{
  switch(event) {
    case FRF_EVENT_TX_FAILED:
      LOG_DEBUG("RF TX FAILED\r\n");
      break;
    case FRF_EVENT_TX_SUCCESS:
      LOG_DEBUG("RF TX SUCCESS\r\n");
      break;
    case FRF_EVENT_RX:
      rfRxReady = true;
      LOG_DEBUG("RF RX Event\r\n");
      break;
  }
}

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
    rfProcess();
    rtos_delay_ms(1);
  }
}

void device_com_setup(void)
{
  FLN_ERR_CHECK(bsp_rf_init(rfISR));

  radio_get_hedwig_address(hedwigAddress);
  radio_get_albus_address(albusAddress);

  memset(frame_buffer, 0, MAX_FRAME_SIZE);

  /* RF Module Init */
  frf_config_t config = {
    .transferFunc = rf_spi_transfer,
    .role = FRF_DEVICE_ROLE_RX,
    .spiCtx = NULL,
    .setCS = bsp_rf_cs_set,
    .setCE = bsp_rf_ce_set,
    .delay_us = delay_us,
    .eventCallback = rf_event_callback
  };

  frf_init(&radio, &config);
  frf_start(&radio, 2, FRF_PACKET_SIZE, hedwigAddress, albusAddress);

  /* Falcon Packet Decoder Init */
  fs_decoder_config_t decoder_config = {
    .callback = decoder_callback
  };

  fs_decoder_init(&decoder, &decoder_config);

  createFRFMutex();
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
