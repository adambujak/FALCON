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
#define FRF_MAX_SIZE_PACKET 32
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
  instance->state = FRF_PWR_STATE_OFF;
  instance->setCE = config.setCE;

  nRF24L01_initialize(&instance->rfInstance, config.blockingTransfer, config.spiCtx, config.setCS);
}

void frf_start(frf_t *instance, uint8_t channel, uint8_t payload_len, uint8_t *rxAddr, uint8_t *txAddr)
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

  nRF24L01_open_pipe(&instance->rfInstance, NRF24L01_ALL, true);

  nRF24L01_set_auto_retr(&instance->rfInstance, 0xF, 1000);

  nRF24L01_set_address_width(&instance->rfInstance, FRF_ADDR_WIDTH);

  nRF24L01_set_address(&instance->rfInstance, NRF24L01_PIPE1, rxAddr);
  nRF24L01_set_address(&instance->rfInstance, NRF24L01_PIPE0, txAddr);
  nRF24L01_set_address(&instance->rfInstance, NRF24L01_TX, txAddr);


  frf_powerUp(instance);
}

/*********************** Setters *************************/

void frf_powerUp(frf_t *instance)
{
  if (instance->state == FRF_PWR_STATE_OFF)
  {
    nRF24L01_set_power_mode(&instance->rfInstance, NRF24L01_PWR_UP);
    //TODO: add delay
  }
}

void frf_powerUpRx(frf_t *instance)
{
  if (instance->state != FRF_PWR_STATE_RX)
  {
    /* Clear any values in rx fifo */
    nRF24L01_flush_rx(&instance->rfInstance);

    CE_LOW();

    /* Power up chip if off */
    frf_powerUp(instance);

    /* Clear clear interrupt flags on chip */
    nRF24L01_get_clear_irq_flags(&instance->rfInstance);

    /* Put chip in rx mode */
    nRF24L01_set_operation_mode(&instance->rfInstance, NRF24L01_PRX);

    instance->state = FRF_PWR_STATE_RX;
  }
}

void frf_powerUpTx(frf_t *instance)
{
  if (instance->state != FRF_PWR_STATE_TX)
  {
    CE_LOW();

    /* Power up chip if off */
    frf_powerUp(instance);

    /* Clear clear interrupt flags on chip */
    nRF24L01_get_clear_irq_flags(&instance->rfInstance);

    /* Put chip in tx mode */
    nRF24L01_set_operation_mode(&instance->rfInstance, NRF24L01_PTX);

    instance->state = FRF_PWR_STATE_TX;
  }
}

/* Power down chip */
void frf_powerDown(frf_t *instance)
{
  if (instance->state != FRF_PWR_STATE_OFF)
  {
    nRF24L01_set_power_mode(&instance->rfInstance, NRF24L01_PWR_DOWN);
    instance->state = FRF_PWR_STATE_OFF;
    CE_LOW();
  }
}

/* Put chip into standby */
void frf_standby(frf_t *instance)
{
  instance->state = FRF_PWR_STATE_STANDBY;
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

/* Clear all interrupts */
void frf_clearInterrupts(frf_t *instance)
{
  CE_LOW();
  nRF24L01_get_clear_irq_flags(&instance->rfInstance);
  CE_HIGH();
}

/* Send data */
void frf_send(frf_t *instance, uint8_t* data, uint8_t payload_len)
{
  /* Go to Standby-I first */
  CE_LOW();

  if (instance->state != FRF_PWR_STATE_TX)
  {
    frf_powerUpTx(instance);
  }

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

/*********************** Readers/Writers *********************/

void frf_read(frf_t *instance, uint8_t *rxBuf)
{
  frf_powerUpRx(instance);

  CE_HIGH();
}


uint8_t frf_blockingRead(frf_t *instance, uint8_t *rxBuf, uint16_t timeout)
{
  frf_powerUpRx(instance);

  CE_HIGH();

  uint32_t i = 0;
  while (!frf_dataReady(instance))
  {
    i++;
    if (i == timeout)
    {
      break;
    }
  }

  uint8_t rxByteCnt = frf_payloadLength(instance);

  frf_getData(instance, rxBuf);

  frf_standby(instance);

  return rxByteCnt;
}

void frf_write(frf_t *instance, uint8_t *txBuf, uint8_t payloadLen)
{
  frf_powerUpTx(instance);

  frf_send(instance, txBuf, payloadLen);
}


uint8_t frf_blockingWrite(frf_t *instance, uint8_t *txBuf, uint8_t payloadLen, uint16_t timeout)
{
  frf_powerUpTx(instance);

  frf_send(instance, txBuf, payloadLen);

  /* Wait for transmission to end */
  uint16_t i = 0;

  while(frf_lastMessageStatus(instance) != FRF_TRANSMISSON_OK)
  {
    i++;
    if (i == timeout )
    {
      break;
    }
  }

  frf_standby(instance);

  return frf_lastMessageStatus(instance);
}
