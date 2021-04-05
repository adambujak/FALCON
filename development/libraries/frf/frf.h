/******************************************************************************
 * @file     frf.h
 * @brief    Falcon RF Library Header File
 * @version  1.0
 * @date     2020-06-07
 * @author   Adam Bujak
 ******************************************************************************/
#ifndef FRF_H
#define FRF_H
#include "nRF24L01.h"

#include <stdbool.h>

#define RF_TEST_MAX_CHANNEL 81   //!< Upper radio channel
#define FRF_TRANSMISSON_OK 1
#define FRF_MESSAGE_LOST 0

#define FRF_ADDR_WIDTH NRF24L01_AW_5BYTES

#define FRF_PACKET_SIZE 32

/* Must be power of 2 */
#define FRF_FIFO_SIZE 32

#define FRF_DEFAULT_TX_TIMER_PERIOD_MS 5

typedef uint8_t frf_packet_t[FRF_PACKET_SIZE];
typedef struct {
  uint32_t writeIndex;
  uint32_t readIndex;
  uint32_t byteCnt;
  frf_packet_t packets[FRF_FIFO_SIZE];
} frf_fifo_t;

typedef enum {
  FRF_EVENT_TX_FAILED = 0,
  FRF_EVENT_TX_SUCCESS,
  FRF_EVENT_RX
} frf_event_t;

typedef void (*frf_cb_t) (frf_event_t arg);
typedef void (*gpio_setter_t) (uint8_t val);
typedef void (*frf_delay_t) (uint32_t ms);
typedef void (*spi_transfer_t) (void *context, uint8_t *tx_buf, uint16_t tx_len,
                                uint8_t *rx_buf, uint16_t rx_len);
typedef enum {
  FRF_POWER_STATE_OFF = 0,
  FRF_POWER_STATE_STANDBY,
  FRF_POWER_STATE_ACTIVE
} frf_power_state_t;

typedef enum {
  FRF_DEVICE_ROLE_RX = 0,
  FRF_DEVICE_ROLE_TX
} frf_device_role_t;

typedef struct {
  spi_transfer_t transferFunc;
  frf_device_role_t role;
  void          *spiCtx;
  gpio_setter_t  setCS;
  gpio_setter_t  setCE;
  frf_delay_t    delay_us;
  frf_cb_t       eventCallback;
} frf_config_t;

typedef struct {
  frf_power_state_t    powerState;
  frf_device_role_t    role;
  gpio_setter_t        setCE;
  frf_delay_t          delay_us;
  nRF24L01_t           rfInstance;
  frf_fifo_t           rxFifo;
  frf_fifo_t           txFifo;
  frf_cb_t             eventCallback;
  volatile bool        isSending;
  volatile bool        interruptFired;
  volatile bool        txScheduled;
} frf_t;

void frf_init(frf_t *instance, frf_config_t *config);

void frf_start(frf_t *instance, uint8_t channel, uint8_t payload_len,
               uint8_t rxAddr[FRF_ADDR_WIDTH], uint8_t txAddr[FRF_ADDR_WIDTH]);

void frf_isr(frf_t *instance);
void frf_process(frf_t *instance);

void frf_tx(frf_t *instance);
bool frf_isSending(frf_t *instance);

int frf_getPacket(frf_t *instance, frf_packet_t packet);
int frf_pushPacket(frf_t *instance, frf_packet_t packet);
void frf_finishSending(frf_t *instance);

void frf_sendPacket(frf_t *instance, frf_packet_t packet);

void frf_powerUp(frf_t *instance);
void frf_standby(frf_t *instance);
void frf_powerDown(frf_t *instance);

#endif // FRF_H
