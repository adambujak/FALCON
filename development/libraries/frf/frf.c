/******************************************************************************
 * @file     frf.c
 * @brief    Falcon RF Library Source File
 * @version  1.0
 * @date     2020-06-07
 * @author   Adam Bujak
 ******************************************************************************/

#include "frf.h"

#include <string.h>

#define FRF_DEFAULT_SIZE_PACKET 32
#define FRF_NB_BITS_FOR_ERROR_RATE_CALC 100000
#define FRF_NB_BYTES_FOR_ERROR_RATE_CALC 12500
#define FRF_ADDR_WIDTH NRF24L01_AW_5BYTES

#define FRF_ENABLE() (RFCKEN = 1);
#define FRF_IRQ_ENABLE() (RF = 1);
#define FRF_IRQ_DISABLE() (RF = 0);

#define CE_LOW()  (instance->setCE(0))
#define CE_HIGH() (instance->setCE(1))

static int fifo_push(frf_fifo_t *fifo, frf_packet_t packet)
{
  if (fifo->byteCnt > FRF_FIFO_SIZE) {
    return -1;
  }

  memcpy(fifo->packets[fifo->writeIndex], packet, FRF_PACKET_SIZE);
  fifo->byteCnt++;
  fifo->writeIndex = (fifo->writeIndex + 1) & (FRF_FIFO_SIZE - 1);
  return 0;
}

static int fifo_drop(frf_fifo_t *fifo)
{
  if (fifo->byteCnt == 0) {
    return -1;
  }

  fifo->byteCnt--;
  fifo->readIndex = (fifo->readIndex + 1) & (FRF_FIFO_SIZE - 1);
  return 0;
}

static int fifo_peek(frf_fifo_t *fifo, frf_packet_t packet)
{
  if (fifo->byteCnt == 0) {
    return -1;
  }

  memcpy(packet, fifo->packets[fifo->readIndex], FRF_PACKET_SIZE);
  return 0;
}

static int fifo_pop(frf_fifo_t *fifo, frf_packet_t packet)
{
  if (fifo->byteCnt == 0) {
    return -1;
  }

  memcpy(packet, fifo->packets[fifo->readIndex], FRF_PACKET_SIZE);
  fifo->byteCnt--;
  fifo->readIndex = (fifo->readIndex + 1) & (FRF_FIFO_SIZE - 1);
  return 0;
}

static bool fifo_isEmpty(frf_fifo_t *fifo)
{
  if (fifo->byteCnt > 0) {
    return false;
  }
  return true;
}

static void powerUpRx(frf_t *instance)
{
  if (instance->transferState == FRF_TRANSFER_STATE_RX) {
    return;
  }
  CE_LOW();

  instance->transferState = FRF_TRANSFER_STATE_RX;
  instance->isSending = false;

  nRF24L01_clear_irq_flags_get_status(&instance->rfInstance);
  nRF24L01_set_operation_mode(&instance->rfInstance, NRF24L01_PRX);
  instance->delay(1);
  nRF24L01_flush_rx(&instance->rfInstance);

  CE_HIGH();
}

static void powerUpTx(frf_t *instance)
{
  if (instance->transferState == FRF_TRANSFER_STATE_TX) {
    return;
  }
  CE_LOW();

  instance->transferState = FRF_TRANSFER_STATE_TX;

  nRF24L01_clear_irq_flags_get_status(&instance->rfInstance);
  nRF24L01_set_operation_mode(&instance->rfInstance, NRF24L01_PTX);
  instance->delay(1);
}

static void readPacket(frf_t *instance)
{
  frf_packet_t packet;
  nRF24L01_read_rx_payload(&instance->rfInstance, packet);
  nRF24L01_clear_irq_flags_get_status(&instance->rfInstance);
  fifo_push(&instance->rxFifo, packet);
}

static void sendPayload(frf_t *instance)
{
  powerUpTx(instance);
  frf_packet_t packet;
  if (fifo_peek(&instance->txFifo, packet) == 0) {
	  nRF24L01_flush_tx(&instance->rfInstance);
    nRF24L01_write_tx_payload(&instance->rfInstance, packet, FRF_PACKET_SIZE);
    instance->isSending = true;
    CE_HIGH();
  }
}

static void handleInterrupt(frf_t *instance)
{
  instance->interruptFired = false;
  uint8_t irqFlags = nRF24L01_get_clear_irq_flags(&instance->rfInstance);

  /* Max RT */
  if (irqFlags & (1 << 4)) {
    instance->isSending = false;
    nRF24L01_flush_tx(&instance->rfInstance);
    fifo_drop(&instance->txFifo);
    nRF24L01_clear_irq_flags_get_status(&instance->rfInstance);
    powerUpRx(instance);
  }

  /* TX Event */
  if (irqFlags & (1 << 5)) {
    instance->isSending = false;
    nRF24L01_clear_irq_flags_get_status(&instance->rfInstance);
    fifo_drop(&instance->txFifo);
    powerUpRx(instance);
  }

  /* RX Event */
  if (irqFlags & (1 << 6)) {
    nRF24L01_clear_irq_flags_get_status(&instance->rfInstance);
    readPacket(instance);
  }
}

/*********************** Public ***********************/

void frf_init(frf_t *instance, spi_transfer_t transferFunc, void *spiCtx,
              gpio_setter_t setCS, gpio_setter_t setCE, frf_delay_t delay)
{
  instance->setCE = setCE;
  instance->delay = delay;
  instance->interruptFired = false;

  nRF24L01_initialize(&instance->rfInstance, transferFunc, spiCtx, setCS);
}

void frf_start(frf_t *instance, uint8_t channel, uint8_t payload_len,
               uint8_t rxAddr[FRF_ADDR_WIDTH], uint8_t txAddr[FRF_ADDR_WIDTH])
{
  CE_LOW();
  instance->delay(100);

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

  nRF24L01_set_irq_mode(&instance->rfInstance, 4, true);
  nRF24L01_set_irq_mode(&instance->rfInstance, 5, true);
  nRF24L01_set_irq_mode(&instance->rfInstance, 6, true);

  instance->powerState = FRF_POWER_STATE_OFF;
  instance->transferState = FRF_TRANSFER_STATE_NONE;

  frf_powerUp(instance);
}

void frf_isr(frf_t *instance)
{
  instance->interruptFired = true;
}

void frf_process(frf_t *instance)
{
  if (instance->interruptFired) {
    handleInterrupt(instance);
  }

  if ((!instance->isSending) && (!fifo_isEmpty(&instance->txFifo))) {
    sendPayload(instance);
  }
}

int frf_getPacket(frf_t *instance, frf_packet_t packet)
{
  return fifo_pop(&instance->rxFifo, packet);
}

int frf_sendPacket(frf_t *instance, frf_packet_t packet)
{
  if (instance->powerState != FRF_POWER_STATE_ACTIVE) {
    return -1;
  }

  int retval = fifo_push(&instance->txFifo, packet);
  if (retval != 0) {
    return retval;
  }

  if (!instance->isSending) {
    sendPayload(instance);
  }
  return 0;
}

bool frf_isSending(frf_t *instance)
{
  /* If actively sending */
  if (instance->isSending) {
    return true;
  }

  /* If packets in tx queue */
  if (!fifo_isEmpty(&instance->txFifo)) {
    return true;
  }

  return false;
}

void frf_finishSending(frf_t *instance)
{
  while (frf_isSending(instance)) {
    frf_process(instance);
  }
}

void frf_powerUp(frf_t *instance)
{
  CE_LOW();

  if (instance->powerState == FRF_POWER_STATE_OFF) {
    nRF24L01_set_power_mode(&instance->rfInstance, NRF24L01_PWR_UP);
    instance->delay(2);
  }

  instance->transferState = FRF_TRANSFER_STATE_NONE;
  instance->powerState = FRF_POWER_STATE_ACTIVE;
  powerUpRx(instance);
}

void frf_standby(frf_t *instance)
{
  CE_LOW();

  instance->powerState = FRF_POWER_STATE_STANDBY;
  instance->transferState = FRF_TRANSFER_STATE_NONE;
}

void frf_powerDown(frf_t *instance)
{
  CE_LOW();

  instance->powerState = FRF_POWER_STATE_OFF;
  instance->transferState = FRF_TRANSFER_STATE_NONE;

  nRF24L01_set_power_mode(&instance->rfInstance, NRF24L01_PWR_DOWN);
}
