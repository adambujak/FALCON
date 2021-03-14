#include "device_com.h"

#include "falcon_common.h"
#include "radio_common.h"
#include "frf.h"
#include "bsp.h"

#include "falcon_packet.h"

#define RTOS_TIMEOUT_TICKS 25

static uint8_t hedwigAddress[RADIO_ADDRESS_LENGTH];
static uint8_t albusAddress[RADIO_ADDRESS_LENGTH];

static frf_t radio;
static volatile bool rfTxReady = false;
static volatile bool rfRxReady = false;
static TimerHandle_t rfTxTimer;
static SemaphoreHandle_t rfTxReadyMutex;
static SemaphoreHandle_t frfMutex;

static void rf_spi_transfer (void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
  bsp_rf_transceive(tx_buf, tx_len, rx_buf, rx_len);
}

static inline void rfISR(void)
{
  frf_isr(&radio);
}

static inline void lock_rf_tx_ready_flag(void)
{
  xSemaphoreTake(rfTxReadyMutex, RTOS_TIMEOUT_TICKS);
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

static inline void setRFTxReadyFlag(bool val)
{
  lock_rf_tx_ready_flag();
  rfTxReady = val;
  unlock_rf_tx_ready_flag();
}

static inline bool getRFTxReadyFlag(void)
{
  bool retval;
  lock_rf_tx_ready_flag();
  retval = rfTxReady;
  unlock_rf_tx_ready_flag();
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
  lock_frf();
  //handleRFRx();
  handleRFTx();
  frf_process(&radio);
  unlock_frf();
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
  lock_frf();
  for (uint8_t i = 0; i < MAX_FRAME_SIZE; i += FRF_PACKET_SIZE) {
    frf_pushPacket(&radio, &frame[i]);
    DEBUG_LOG("frame send\r\n");
  }
  unlock_frf();
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

  createFRFMutex();
  createRFTxTimer();
  createRFTxReadyMutex();
}

void device_com_task(void *pvParameters)
{
  DEBUG_LOG("Device com task started\r\n");

  while(1) {
    rfProcess();
    albus_delay(1);
  }
}

