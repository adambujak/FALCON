/******************************************************************************
 * @file     frf.c
 * @brief    Falcon RF Library Source File
 * @version  1.0
 * @date     2020-06-07
 * @author   Adam Bujak
 ******************************************************************************/

#include "frf.h"


#include <stdbool.h>

#define FRF_DEFAULT_SIZE_PACKET 32
#define FRF_NB_BITS_FOR_ERROR_RATE_CALC 100000
#define FRF_NB_BYTES_FOR_ERROR_RATE_CALC 12500
#define FRF_MAX_SIZE_PACKET 32
#define FRF_ADDR_WIDTH NRF24L01_AW_5BYTES

//ToDo: add state variable to determine whether or not to toggle ce


#define FRF_ENABLE() (RFCKEN = 1);
#define FRF_IRQ_ENABLE() (RF = 1);
#define FRF_IRQ_DISABLE() (RF = 0);

#define CE_LOW() (instance->setCE(0))
#define CE_HIGH() (instance->setCE(1))

void frf_init(frf_t *instance, frf_config_t config)
{
  instance->direction = config.direction;
  instance->setCE = config.setCE;

  nRF24L01_initialize(&instance->rfInstance, config.blockingTransfer, config.spiCtx, config.setCS);
}

void frf_start(frf_t *instance, uint8_t channel, uint8_t payload_len)
{
  nRF24L01_set_rf_channel(&instance->rfInstance, channel);
  nRF24L01_set_rx_payload_width(&instance->rfInstance, NRF24L01_PIPE0, 0);
  nRF24L01_set_rx_payload_width(&instance->rfInstance, NRF24L01_PIPE1, payload_len);
  nRF24L01_set_rx_payload_width(&instance->rfInstance, NRF24L01_PIPE2, 0);
  nRF24L01_set_rx_payload_width(&instance->rfInstance, NRF24L01_PIPE3, 0);
  nRF24L01_set_rx_payload_width(&instance->rfInstance, NRF24L01_PIPE4, 0);
  nRF24L01_set_rx_payload_width(&instance->rfInstance, NRF24L01_PIPE5, 0);

  nRF24L01_set_output_power(&instance->rfInstance, NRF24L01_0DBM);
  nRF24L01_set_datarate(&instance->rfInstance, NRF24L01_1MBPS);

  nRF24L01_set_crc_mode(&instance->rfInstance, NRF24L01_CRC_8BIT);
  nRF24L01_set_auto_retr(&instance->rfInstance, 0xF, 1000);

  nRF24L01_set_address_width(&instance->rfInstance, FRF_ADDR_WIDTH);

  if (instance->direction == FRF_DIR_RX)
  {
    nRF24L01_get_clear_irq_flags(&instance->rfInstance);
    nRF24L01_set_irq_mode(&instance->rfInstance, NRF24L01_RX_DR, true);
  }

  frf_powerUpRx(instance);
}

void frf_powerUpRx(frf_t *instance)
{
  nRF24L01_flush_rx(&instance->rfInstance);


  CE_LOW();
  nRF24L01_get_clear_irq_flags(&instance->rfInstance);

  nRF24L01_set_operation_mode(&instance->rfInstance, NRF24L01_PRX);
  nRF24L01_set_power_mode(&instance->rfInstance, NRF24L01_PWR_UP);
  CE_HIGH();
}

void frf_powerUpTx(frf_t *instance)
{
  CE_LOW();
  nRF24L01_get_clear_irq_flags(&instance->rfInstance);

  nRF24L01_set_operation_mode(&instance->rfInstance, NRF24L01_PTX);
  nRF24L01_set_power_mode(&instance->rfInstance, NRF24L01_PWR_UP);
  CE_HIGH();
}

void frf_powerDown(frf_t *instance)
{
  nRF24L01_set_power_mode(&instance->rfInstance, NRF24L01_PWR_DOWN);
  CE_LOW();
}

void frf_standby(frf_t *instance)
{
  CE_LOW();
}

/* Set the RX address */
void frf_rx_address(frf_t *instance, uint8_t *addr)
{
  CE_LOW();
  nRF24L01_set_address(&instance->rfInstance, NRF24L01_PIPE1, addr);
  CE_HIGH();
}

/* Set the TX address */
void frf_tx_address(frf_t *instance, uint8_t* addr)
{
  /* RX_ADDR_P0 must be set to the sending addr for auto ack to work. */
  CE_LOW();
  nRF24L01_set_address(&instance->rfInstance, NRF24L01_PIPE0, addr);
  nRF24L01_set_address(&instance->rfInstance, NRF24L01_TX, addr);
  CE_HIGH();
}

/* Checks if data is available for reading */
/* Returns 1 if data is ready ... */
uint8_t frf_dataReady(frf_t *instance)
{
  // See note in getData() function - just checking NRF24L01_RX_DR isn't good enough
  uint8_t status = nRF24L01_nop(&instance->rfInstance);

  // We can short circuit on NRF24L01_RX_DR, but if it's not set, we still need
  // to check the FIFO for any pending packets
  if ( status & (1 << NRF24L01_RX_DR) )
  {
      return 1;
  }

  return !frf_rxFifoEmpty(instance);
}

/* Checks if receive FIFO is empty or not */
uint8_t frf_rxFifoEmpty(frf_t *instance)
{
  return nRF24L01_rx_fifo_empty(&instance->rfInstance);
}

/* Returns the length of data waiting in the RX fifo */
uint8_t frf_payloadLength(frf_t *instance)
{
  return nRF24L01_read_rx_payload_width(&instance->rfInstance);
}

/* Reads payload bytes into data array */
void frf_getData(frf_t *instance, uint8_t* data)
{
  nRF24L01_read_rx_payload(&instance->rfInstance, data);
}

/* Returns the number of retransmissions occured for the last message */
uint8_t frf_retransmissionCount(frf_t *instance)
{
  return nRF24L01_get_transmit_attempts(&instance->rfInstance);
}

// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
void frf_send(frf_t *instance, uint8_t* data, uint8_t payload_len)
{
  /* Go to Standby-I first */
  CE_LOW();

  /* Set to transmitter mode , Power up if needed */
  frf_powerUpTx(instance);

  /* Do we really need to flush TX fifo each time ? */
  #if 1
   //   nRF24L01_flush_tx(&instance->rfInstance);
  #endif

  nRF24L01_write_tx_payload(&instance->rfInstance, data, payload_len);

  /* Start the transmission */
  CE_HIGH();
}

uint8_t frf_isSending(frf_t *instance)
{
  /* read the current status */
  uint8_t status = nRF24L01_nop(&instance->rfInstance);

  /* if sending successful (NRF24L01_TX_DS) or max retries exceded (NRF24L01_MAX_RT). */
  if((status & ((1 << NRF24L01_TX_DS)  | (1 << NRF24L01_MAX_RT))))
  {
      return 0; /* false */
  }

  return 1; /* true */
}

uint8_t frf_lastMessageStatus(frf_t *instance)
{
  uint8_t status = nRF24L01_nop(&instance->rfInstance);

  /* Transmission went OK */
  if((status & ((1 << NRF24L01_TX_DS))))
  {
      return FRF_TRANSMISSON_OK;
  }
  /* Maximum retransmission count is reached */
  /* Last message probably went missing ... */
  else if((status & ((1 << NRF24L01_MAX_RT))))
  {
      return FRF_MESSAGE_LOST;
  }
  /* Probably still sending ... */
  else
  {
      return 0xFF;
  }
}

void frf_clearInterrupts(frf_t *instance)
{
  CE_LOW();
  nRF24L01_get_clear_irq_flags(&instance->rfInstance);
  CE_HIGH();
}
