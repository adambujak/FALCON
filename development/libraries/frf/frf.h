/******************************************************************************
 * @file     frf.h
 * @brief    Falcon RF Library Header File
 * @version  1.0
 * @date     2020-06-07
 * @author   Adam Bujak
 ******************************************************************************/

#include "nRF24L01.h"

#include <stdbool.h>

#define RF_TEST_MAX_CHANNEL 81   //!< Upper radio channel
#define FRF_TRANSMISSON_OK 1
#define FRF_MESSAGE_LOST 0

typedef void (*gpio_setter_t) (uint8_t val);
typedef void (*spi_transfer_t) (void *context, uint8_t *tx_buf, uint16_t tx_len,
                                uint8_t *rx_buf, uint16_t rx_len);

enum frf_state_e {
  FRF_PWR_STATE_OFF = 0,
  FRF_PWR_STATE_STANDBY,
  FRF_PWR_STATE_RX,
  FRF_PWR_STATE_TX
};

typedef struct {
  gpio_setter_t   setCE;
  gpio_setter_t   setCS;
  spi_transfer_t  blockingTransfer;
  void           *spiCtx;
} frf_config_t;

typedef struct {
  enum frf_state_e state;
  bool            ceState;
  gpio_setter_t   setCE;
  nRF24L01_t      rfInstance;
} frf_t;

void frf_init(frf_t *instance, frf_config_t config);

void frf_start(frf_t *instance, uint8_t channel, uint8_t payload_len, uint8_t *rxAddr, uint8_t *txAddr);

void frf_powerUp(frf_t *instance);

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