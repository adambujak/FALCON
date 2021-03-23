#include "device_com.h"

#include "falcon_common.h"
#include "radio_common.h"
#include "frf.h"
#include "bsp.h"

#include "falcon_packet.h"
#include "fs_decoder.h"
#include "fp_decode.h"


#define RTOS_TIMEOUT_TICKS 25

typedef enum {
  UART_STATE_IDLE = 0,
  UART_STATE_READING,
  UART_STATE_READY,
} uart_state_t;

static uint8_t hedwigAddress[RADIO_ADDRESS_LENGTH];
static uint8_t albusAddress[RADIO_ADDRESS_LENGTH];

static uint8_t uartBuffer[MAX_FRAME_SIZE];

static fs_decoder_t decoder;

static frf_t radio;
static volatile bool rfTxReady = false;
static volatile bool rfRxReady = false;
static volatile uart_state_t uartState = UART_STATE_IDLE;
static TimerHandle_t rfTxTimer;
static SemaphoreHandle_t rfTxReadyMutex;
static SemaphoreHandle_t frfMutex;
static SemaphoreHandle_t uartMutex;


static void decoder_callback(uint8_t *data, fp_type_t packetType)
{
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wswitch"
  switch (packetType) {
  case FPT_FLIGHT_CONTROL_COMMAND:
  {
    fpc_flight_control_t control = {{0}};
    fpc_flight_control_decode(data, &control);
    DEBUG_LOG("received motor cmd\r\n");
    DEBUG_LOG("MOTOR COMMAND: %f, %f, %f, %f\r\n",
           control.fcsControlCmd.yaw,
           control.fcsControlCmd.pitch,
           control.fcsControlCmd.roll,
           control.fcsControlCmd.alt);
  }
    break;
//    case FPT_SET_DESTINATION_COMMAND:
//    {
//      fpc_set_destination_t dest = {0};
//      fpc_set_destination_decode(data, &dest);
//      printf("decoded dest: %f\r\n", dest.gpsData.latitude);
//    }
//      break;
//    case FPT_FLIGHT_CONTROL_POSITION_COMMAND:
//    {
//      fpc_flight_control_position_t pos = {0};
//      fpc_flight_control_position_decode(data, &pos);
//      printf("decoded posi: %f %f %f %f\r\n", pos.positionReferenceCMD.x, pos.positionReferenceCMD.y, pos.positionReferenceCMD.z, pos.positionReferenceCMD.yaw);
//    }
//      break;
//    default:
//      break;
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

static inline void createUartMutex(void)
{
  uartMutex = xSemaphoreCreateMutex();
  if (uartMutex == NULL) {
    error_handler();
  }
}

static inline uint32_t lock_uart(void)
{
  return xSemaphoreTake(uartMutex, RTOS_TIMEOUT_TICKS);
}

static inline void unlock_uart(void)
{
  xSemaphoreGive(uartMutex);
}

static void uart_rx_callback(void)
{
    uartState = UART_STATE_READY;
    DEBUG_LOG("data\r\n");
}

static void uartProcess(void)
{
	switch(uartState) {
	  case UART_STATE_IDLE:
		bsp_uart_read(uartBuffer, 2, uart_rx_callback);
		uartState = UART_STATE_READING;
		break;
	  case UART_STATE_READING:
		break;
	  case UART_STATE_READY:
		uartState = UART_STATE_IDLE;
		decode_frame(uartBuffer, MAX_FRAME_SIZE);
	}

}

static void rf_spi_transfer (void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
  bsp_rf_transceive(tx_buf, tx_len, rx_buf, rx_len);
}

static inline void rfISR(void)
{
  frf_isr(&radio);
}

static inline uint32_t lock_rf_tx_ready_flag(void)
{
  return xSemaphoreTake(rfTxReadyMutex, RTOS_TIMEOUT_TICKS);
}

static inline void unlock_rf_tx_ready_flag(void)
{
  xSemaphoreGive(rfTxReadyMutex);
}

static inline void createRFTxReadyMutex(void)
{
  rfTxReadyMutex = xSemaphoreCreateMutex();
  if (rfTxReadyMutex == NULL) {
    error_handler();
  }
}

static inline uint32_t lock_frf(void)
{
  return xSemaphoreTake(frfMutex, RTOS_TIMEOUT_TICKS);
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

static inline void setRFTxReadyFlag(bool val)
{
  if (lock_rf_tx_ready_flag() == pdTRUE) {
    rfTxReady = val;
    unlock_rf_tx_ready_flag();
  }
}

static inline bool getRFTxReadyFlag(void)
{
  bool retval = false;
  if (lock_rf_tx_ready_flag() == pdTRUE) {
    retval = rfTxReady;
    unlock_rf_tx_ready_flag();
  }
  return retval;
}

static void rfTxTimerCallback(TimerHandle_t timer)
{
  setRFTxReadyFlag(true);
}

static inline void createRFTxTimer(void)
{
  rfTxTimer = xTimerCreate("rfTxTimer",
                    TICKS_TO_MS(FRF_DEFAULT_TX_TIMER_PERIOD_MS*80),
                    pdTRUE,
                    (void *) 0,
                    rfTxTimerCallback
                    );
  xTimerStart(rfTxTimer, RTOS_TIMEOUT_TICKS);
}

static inline void handleRFTx(void)
{
  if (getRFTxReadyFlag()) {
    frf_tx(&radio);
    setRFTxReadyFlag(false);
  }
}

static inline void handleRFRx(void)
{
  if (rfRxReady) {
    frf_packet_t packet;
    if (frf_getPacket(&radio, packet) == 0) {
      DEBUG_LOG("HEDWIG: %s\r\n", (char*)packet);
    }
    rfRxReady = false;
  }
}

static inline void rfProcess(void)
{
  if (lock_frf() == pdTRUE) {
    //handleRFRx();
    handleRFTx();
    frf_process(&radio);
    unlock_frf();
  }
}

static void rf_event_callback(frf_event_t event)
{
  switch(event) {
    case FRF_EVENT_TX_FAILED:
      DEBUG_LOG("RF TX FAILED\r\n");
      xTimerChangePeriod(rfTxTimer, 1000, 25);
      break;
    case FRF_EVENT_TX_SUCCESS:
      DEBUG_LOG("RF TX SUCCESS\r\n");
      break;
    case FRF_EVENT_RX:
      rfRxReady = true;
      DEBUG_LOG("RF RX Event\r\n");
      break;
  }
}

void device_com_send_frame(uint8_t *frame)
{
  if (lock_frf() == pdTRUE) {
    for (uint8_t i = 0; i < MAX_FRAME_SIZE; i += FRF_PACKET_SIZE) {
      frf_pushPacket(&radio, &frame[i]);
      DEBUG_LOG("frame send\r\n");
    }
    unlock_frf();
  }
}

void device_com_setup(void)
{
  FLN_ERR_CHECK(bsp_rf_init(rfISR));

  radio_get_hedwig_address(hedwigAddress);
  radio_get_albus_address(albusAddress);

  frf_config_t config = {
    .transferFunc = rf_spi_transfer,
    .spiCtx = NULL,
    .setCS = bsp_rf_cs_set,
    .setCE = bsp_rf_ce_set,
    .delay = albus_delay,
    .eventCallback = rf_event_callback
  };

  frf_init(&radio, &config);
  frf_start(&radio, 2, FRF_PACKET_SIZE, albusAddress, hedwigAddress);

  decoder_init();

  createFRFMutex();
  createRFTxTimer();
  createRFTxReadyMutex();
  createUartMutex();
}

void device_com_task(void *pvParameters)
{
  DEBUG_LOG("Device com task started\r\n");

  while(1) {
    rfProcess();
    uartProcess();
    albus_delay(1);
  }
}

