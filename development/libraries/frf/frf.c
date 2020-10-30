/******************************************************************************
 * @file     frf.c
 * @brief    Falcon RF Library Source File
 * @version  1.0
 * @date     2020-06-07
 * @author   Adam Bujak
 ******************************************************************************/

#include "frf.h"

#define FRF_DEFAULT_SIZE_PACKET 32
#define FRF_NB_BITS_FOR_ERROR_RATE_CALC 100000
#define FRF_NB_BYTES_FOR_ERROR_RATE_CALC 12500
#define FRF_ADDR_WIDTH NRF24L01_AW_5BYTES

//ToDo: add state variable to determine whether or not to toggle ce


#define FRF_ENABLE() (RFCKEN = 1);
#define FRF_IRQ_ENABLE() (RF = 1);
#define FRF_IRQ_DISABLE() (RF = 0);

#define CE_LOW()  (instance->setCE(0))
#define CE_HIGH() (instance->setCE(1))


/***********************************************************
************************ Public ****************************
***********************************************************/

/*********************** Inits ****************************/
void frf_init(frf_t *instance, frf_config_t config)
{
  instance->setCE = config.setCE;

  nRF24L01_initialize(&instance->rfInstance, config.blockingTransfer, config.spiCtx, config.setCS);
}

void frf_start(frf_t *instance, uint8_t channel, uint8_t payload_len,
               uint8_t rxAddr[FRF_ADDR_WIDTH], uint8_t txAddr[FRF_ADDR_WIDTH])
{
  CE_LOW();
  // Set RF channel
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

  nRF24L01_open_pipe(&instance->rfInstance, NRF24L01_ALL, true);

  nRF24L01_set_auto_retr(&instance->rfInstance, 0xF, 1000);

  nRF24L01_set_address_width(&instance->rfInstance, FRF_ADDR_WIDTH);

  nRF24L01_set_address(&instance->rfInstance, NRF24L01_PIPE1, rxAddr);
  nRF24L01_set_address(&instance->rfInstance, NRF24L01_PIPE0, txAddr);
  nRF24L01_set_address(&instance->rfInstance, NRF24L01_TX, txAddr);


  frf_powerUpRx(instance);
}

/*********************** Setters *************************/

void frf_powerUpRx(frf_t *instance)
{
  CE_LOW();
  // Start listening
  nRF24L01_clear_irq_flags_get_status(&instance->rfInstance);
  nRF24L01_set_operation_mode(&instance->rfInstance, NRF24L01_PRX);
  nRF24L01_set_power_mode(&instance->rfInstance, NRF24L01_PWR_UP);

  CE_HIGH();
}

void frf_powerUpTx(frf_t *instance)
{
  CE_LOW();

  nRF24L01_clear_irq_flags_get_status(&instance->rfInstance);
  nRF24L01_set_operation_mode(&instance->rfInstance, NRF24L01_PTX);
  nRF24L01_set_power_mode(&instance->rfInstance, NRF24L01_PWR_UP);
}

/* Send data */
void frf_send(frf_t *instance, uint8_t* data, uint8_t payload_len)
{
  /* Go to Standby-I first */
  CE_LOW();

  nRF24L01_flush_tx(&instance->rfInstance);

  nRF24L01_write_tx_payload(&instance->rfInstance, data, payload_len);

  /* Start the transmission */
  CE_HIGH();
}

/*********************** Getters *************************/

/* Checks if data is available for reading */
uint8_t frf_dataReady(frf_t *instance)
{
  uint8_t status = nRF24L01_nop(&instance->rfInstance);

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
  nRF24L01_clear_irq_flags_get_status(&instance->rfInstance);
}

/* Returns the number of retransmissions occured for the last message */
uint8_t frf_retransmissionCount(frf_t *instance)
{
  return nRF24L01_get_transmit_attempts(&instance->rfInstance);
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
