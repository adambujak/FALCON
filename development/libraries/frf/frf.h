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

typedef void (*gpio_setter_t) (uint8_t val);
typedef void (*spi_transfer_t) (void *context, uint8_t *tx_buf, uint16_t tx_len,
                                uint8_t *rx_buf, uint16_t rx_len);
typedef enum {
  FRF_POWER_STATE_OFF = 0,
  FRF_POWER_STATE_STANDBY,
  FRF_POWER_STATE_ACTIVE
} frf_power_state_t;

typedef enum {
  FRF_TRANSFER_STATE_RX = 0,
  FRF_TRANSFER_STATE_TX
} frf_transfer_state_t;

typedef struct {
  frf_power_state_t    powerState;
  frf_transfer_state_t transferState;
  gpio_setter_t    setCE;
  nRF24L01_t       rfInstance;
  frf_fifo_t       rxFifo;
  frf_fifo_t       txFifo;
} frf_t;

void frf_isr(frf_t *instance);

void frf_init(frf_t *instance, spi_transfer_t transferFunc, void *spiCtx,
              gpio_setter_t setCS, gpio_setter_t setCE);

void frf_start(frf_t *instance, uint8_t channel, uint8_t payload_len,
               uint8_t rxAddr[FRF_ADDR_WIDTH], uint8_t txAddr[FRF_ADDR_WIDTH]);

void frf_powerUp(frf_t *instance);

int frf_getPacket(frf_t *instance, frf_packet_t packet);
void frf_powerUpRx(frf_t *instance);

void frf_powerUpTx(frf_t *instance);

void frf_standby(frf_t *instance);

void frf_powerDown(frf_t *instance);

void frf_read(frf_t *instance, uint8_t *rxBuf);

uint8_t frf_blockingRead(frf_t *instance, uint8_t *rxBuf, uint16_t timeout);

void frf_write(frf_t *instance, uint8_t *txBuf, uint8_t payloadLen);

uint8_t frf_blockingWrite(frf_t *instance, uint8_t *txBuf, uint8_t payloadLen, uint16_t timeout);

/* Set the RX address */
void frf_rx_address(frf_t *instance, uint8_t *addr);

/* Set the TX address */
void frf_tx_address(frf_t *instance, uint8_t* adr);

void frf_clearInterrupts(frf_t *instance);

/* Checks if data is available for reading */
/* Returns 1 if data is ready ... */
uint8_t frf_dataReady(frf_t *instance);

/* Checks if receive FIFO is empty or not */
uint8_t frf_rxFifoEmpty(frf_t *instance);

/* Returns the length of data waiting in the RX fifo */
uint8_t frf_payloadLength(frf_t *instance);

/* Reads payload bytes into data array */
void frf_getData(frf_t *instance, uint8_t* data);

/* Returns the number of retransmissions occured for the last message */
uint8_t frf_retransmissionCount(frf_t *instance);

// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
void frf_send(frf_t *instance, uint8_t* data, uint8_t payload_len);

uint8_t frf_isSending(frf_t *instance);

uint8_t frf_lastMessageStatus(frf_t *instance);

#endif // FRF_H
