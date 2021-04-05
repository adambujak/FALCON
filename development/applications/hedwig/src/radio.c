#include "radio.h"

#include "falcon_common.h"
#include "radio_common.h"
#include "frf.h"
#include "bsp.h"

#define IS_POWER_OF_TWO(num) (((num) & ((num) - 1)) == 0) ? true : false

typedef struct {
  uint8_t *buffer;
  uint32_t write_index;
  uint32_t read_index;
  uint32_t bytes_available;
  uint32_t size;
} fifo_t;

#define RF_RX_BUFFER_SIZE 512
#define RF_TX_BUFFER_SIZE 512

static uint8_t rx_buffer[RF_RX_BUFFER_SIZE];
static uint8_t tx_buffer[RF_TX_BUFFER_SIZE];

static fifo_t rx_fifo;
static fifo_t tx_fifo;

static uint8_t hedwigAddress[RADIO_ADDRESS_LENGTH];
static uint8_t albusAddress[RADIO_ADDRESS_LENGTH];

static frf_t radio;

static void fifo_init(fifo_t *fifo, uint8_t *buffer, uint32_t size)
{
  ASSERT(IS_POWER_OF_TWO(size));
  fifo->buffer = buffer;
  fifo->size = size;
  fifo->write_index = 0;
  fifo->read_index = 0;
  fifo->bytes_available = 0;
}

static void fifo_push(fifo_t *fifo, uint8_t *buffer, uint32_t length)
{
  for (uint32_t i = 0; i < length; i++) {
    fifo->buffer[fifo->write_index] = buffer[i];
    fifo->write_index = (fifo->write_index + 1) & (fifo->size - 1);
  }

  fifo->bytes_available = (fifo->bytes_available + length) & (fifo->size - 1);
}

static int fifo_pop(fifo_t *fifo, uint8_t *dest, uint32_t length)
{
  if (length > fifo->bytes_available) {
    return 0;
  }

  for (uint32_t i = 0; i < length; i++) {
    dest[i] = fifo->buffer[fifo->read_index];
    fifo->read_index = (fifo->read_index + 1) & (fifo->size - 1);
  }
  fifo->bytes_available -= length;
  return length;
}

static void rf_spi_transfer(void * context, uint8_t * tx_buf, uint16_t tx_len,
                             uint8_t * rx_buf, uint16_t rx_len)
{
  bsp_rf_transceive(tx_buf, tx_len, rx_buf, rx_len);
}

static inline void rf_isr(void)
{
  frf_isr(&radio);
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
    {
      uint8_t temp[FRF_PACKET_SIZE];
      frf_getPacket(&radio, temp);
      fifo_push(&rx_fifo, temp, FRF_PACKET_SIZE);
      LOG_DEBUG("RF RX Event\r\n");
      break;
    }
  }
}

uint32_t radio_send_data(uint8_t *source, uint32_t length)
{
  if (length > RF_TX_BUFFER_SIZE) {
    return 0;
  }

  fifo_push(&tx_fifo, source, length);
  return length;
}

uint32_t radio_get_data(uint8_t *dest, uint32_t length)
{
  return fifo_pop(&rx_fifo, dest, length);
}

void radio_init(void)
{
  radio_get_hedwig_address(hedwigAddress);
  radio_get_albus_address(albusAddress);

  fifo_init(&rx_fifo, rx_buffer, RF_RX_BUFFER_SIZE);
  fifo_init(&tx_fifo, tx_buffer, RF_TX_BUFFER_SIZE);

  FLN_ERR_CHECK(bsp_rf_init(rf_isr));

  /* RF Module Init */
  frf_config_t config = {
    .transferFunc = rf_spi_transfer,
    .role = FRF_DEVICE_ROLE_RX,
    .spiCtx = NULL,
    .setCS = bsp_rf_cs_set,
    .setCE = bsp_rf_ce_set,
    .delay = rtos_delay_ms,
    .eventCallback = rf_event_callback
  };

  frf_init(&radio, &config);
  frf_start(&radio, 2, FRF_PACKET_SIZE, hedwigAddress, albusAddress);
}

void radio_process(void)
{
  frf_process(&radio);

  if (!frf_isSending(&radio)) {
    uint8_t temp[FRF_PACKET_SIZE];
    if (fifo_pop(&tx_fifo, temp, FRF_PACKET_SIZE) == FRF_PACKET_SIZE) {
      frf_sendPacket(&radio, temp);
    }
  }
}