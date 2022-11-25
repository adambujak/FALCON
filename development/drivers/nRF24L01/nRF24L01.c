/** @file
 * @brief Implementation of nRF24L01
 */
/*lint -t2 */
/*lint -esym( 534, nRF24L01_write_reg ) */
/*lint -esym( 534, nRF24L01_rw ) */
/*lint -e708 */
/*lint -e46 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "drivers_common.h"
#include "nRF24L01.h"

#define NRF24L01_SET_BIT(pos)    ((uint8_t) (1U << ((uint8_t) (pos))))
#define NRF24L01_UINT8(t)        ((uint8_t) (t))

#define CSN_LOW()                (instance->setCS(0))
#define CSN_HIGH()               (instance->setCS(1))

/** Basis function read_reg.
 * Use this function to read the contents
 * of one radios register.
 *
 * @param reg Register to read
 * @return Register contents
 */
uint8_t nRF24L01_read_reg(nRF24L01_t *instance, uint8_t reg);

/** Basis function write_reg.
 * Use this function to write a new value to
 * a radio register.
 *
 * @param reg Register to write
 * @param value New value to write
 * @return Status register
 */
uint8_t nRF24L01_write_reg(nRF24L01_t *instance, uint8_t reg, uint8_t value);

/** Basis function, read_multibyte register .
 * Use this function to read multiple bytes from
 * a multibyte radio-register
 *
 * @param reg Multibyte register to read from
 * @param *pbuf Pointer to buffer in which to store read bytes to
 *
 * @return pipe# of received data (MSB), if operation used by a nRF24L01_read_rx_pload
 * @return length of read data (LSB), either for nRF24L01_read_rx_pload or
 * for nRF24L01_get_address.
 */
uint16_t nRF24L01_read_multibyte_reg(nRF24L01_t *instance, uint8_t reg, uint8_t *pbuf);

/** Basis function, write_multibyte register.
 * Use this function to write multiple bytes to
 * a multiple radio register.
 *
 * @param reg Register to write
 * @param *pbuf pointer to buffer in which data to write is
 * @param length \# of bytes to write
 */
void nRF24L01_write_multibyte_reg(nRF24L01_t *instance, uint8_t reg, const uint8_t *pbuf, uint8_t length);

/**
 * Typedef for the CONFIG register. Contains all the bitaddressable
 * settings in the bits struct and the value sent to the radio in the uint8_t
 */
typedef union {
  uint8_t value;
  struct {
    uint8_t prim_rx     : 1;
    uint8_t pwr_up      : 1;
    uint8_t crc0        : 1;
    uint8_t en_crc      : 1;
    uint8_t mask_max_rt : 1;
    uint8_t mask_tx_ds  : 1;
    uint8_t mask_rx_dr  : 1;
    const   uint8_t     : 1;
  } bits;
} nRF24L01_config_t;

/**
 * Typedef for the EN_AA, EN_RXADDR and DYNPD registers. Contains all the
 * bitaddressable settings in the bits struct and the value sent to the radio
 * in the uint8_t
 */
typedef union {
  uint8_t value;
  struct {
    uint8_t pipe_0  : 1;
    uint8_t pipe_1  : 1;
    uint8_t pipe_2  : 1;
    uint8_t pipe_3  : 1;
    uint8_t pipe_4  : 1;
    uint8_t pipe_5  : 1;
    const   uint8_t : 2;
  } bits;
} nRF24L01_en_pipes_t;

/**
 * Typedef for the SETUP_AW register. Contains all the bitaddressable
 * settings in the bits struct and the value sent to the radio in the uint8_t
 */
typedef union {
  uint8_t value;
  struct {
    uint8_t aw      : 2;
    const   uint8_t : 6;
  } bits;
} nRF24L01_setup_aw_t;

/**
 * Typedef for the SETUP_RETR register. Contains all the bitaddressable
 * settings in the bits struct and the value sent to the radio in the uint8_t
 */
typedef union {
  uint8_t value;
  struct {
    uint8_t arc : 4;
    uint8_t ard : 4;
  } bits;
} nRF24L01_setup_retr_t;

/**
 * Typedef for the RF_CH register. Contains all the bitaddressable
 * settings in the bits struct and the value sent to the radio in the uint8_t
 */
typedef union {
  uint8_t value;
  struct {
    uint8_t rf_ch   : 7;
    const   uint8_t : 1;
  } bits;
} nRF24L01_rf_ch_t;

/**
 * Typedef for the RF_SETUP register. Contains all the bitaddressable
 * settings in the bits struct and the value sent to the radio in the uint8_t
 */
typedef union {
  uint8_t value;
  struct {
    const   uint8_t    : 1;
    uint8_t rf_pwr     : 2;
    uint8_t rf_dr_high : 1;
    uint8_t pll_lock   : 1;
    uint8_t rf_dr_low  : 1;
    const   uint8_t    : 1;
    uint8_t cont_wave  : 1;
  } bits;
} nRF24L01_rf_setup_t;

/**
 * Typedef for the RX_PW_Px registers. Contains all the bitaddressable
 * settings in the bits struct and the value sent to the radio in the uint8_t
 */
typedef union {
  uint8_t value;
  struct {
    uint8_t rx_pw   : 6;
    const   uint8_t : 2;
  } bits;
} nRF24L01_rx_pw_t;

/**
 * Typedef for the FEATURE register. Contains all the bitaddressable
 * settings in the bits struct and the value sent to the radio in the uint8_t
 */
typedef union {
  uint8_t value;
  struct {
    uint8_t en_dyn_ack : 1;
    uint8_t en_ack_pay : 1;
    uint8_t en_dpl     : 1;
    const   uint8_t    : 5;
  } bits;
} nRF24L01_feature_t;

void nRF24L01_set_operation_mode(nRF24L01_t *instance, nRF24L01_operation_mode_t op_mode)
{
  nRF24L01_config_t config;

  config.value = nRF24L01_read_reg(instance, NRF24L01_CONFIG);

  if (op_mode == NRF24L01_PRX) {
    config.bits.prim_rx = 1U;
  }
  else{
    config.bits.prim_rx = 0U;
  }

  nRF24L01_write_reg(instance, NRF24L01_CONFIG, config.value);
}

void nRF24L01_set_power_mode(nRF24L01_t *instance, nRF24L01_pwr_mode_t pwr_mode)
{
  nRF24L01_config_t config;

  config.value = nRF24L01_read_reg(instance, NRF24L01_CONFIG);

  if (pwr_mode == NRF24L01_PWR_UP) {
    config.bits.pwr_up = 1U;
  }
  else{
    config.bits.pwr_up = 0U;
  }

  nRF24L01_write_reg(instance, NRF24L01_CONFIG, config.value);
}

void nRF24L01_set_crc_mode(nRF24L01_t *instance, nRF24L01_crc_mode_t crc_mode)
{
  nRF24L01_config_t config;

  config.value = nRF24L01_read_reg(instance, NRF24L01_CONFIG);

  switch (crc_mode) {
  case NRF24L01_CRC_OFF:
    config.bits.en_crc = 0U;
    break;

  case NRF24L01_CRC_8BIT:
    config.bits.en_crc = 1U;
    config.bits.crc0 = 0U;
    break;

  case NRF24L01_CRC_16BIT:
    config.bits.en_crc = 1U;
    config.bits.crc0 = 1U;
    break;

  default:
    break;
  }

  nRF24L01_write_reg(instance, NRF24L01_CONFIG, config.value);
}

void nRF24L01_set_irq_mode(nRF24L01_t *instance, nRF24L01_irq_source_t int_source, bool irq_state)
{
  nRF24L01_config_t config;

  config.value = nRF24L01_read_reg(instance, NRF24L01_CONFIG);

  switch (int_source) {
  case NRF24L01_MAX_RT:
    config.bits.mask_max_rt = irq_state ? 0U : 1U;
    break;

  case NRF24L01_TX_DS:
    config.bits.mask_tx_ds = irq_state ? 0U : 1U;
    break;

  case NRF24L01_RX_DR:
    config.bits.mask_rx_dr = irq_state ? 0U : 1U;
    break;
  }

  nRF24L01_write_reg(instance, NRF24L01_CONFIG, config.value);
}

uint8_t nRF24L01_get_clear_irq_flags(nRF24L01_t *instance)
{
  uint8_t retval;

  retval = nRF24L01_write_reg(instance, NRF24L01_STATUS, (DRV_BIT6 | DRV_BIT5 | DRV_BIT4));

  return retval & (DRV_BIT6 | DRV_BIT5 | DRV_BIT4);
}

uint8_t nRF24L01_clear_irq_flags_get_status(nRF24L01_t *instance)
{
  uint8_t retval;

  // When RFIRQ is cleared (when calling write_reg), pipe information is unreliable (read again with read_reg)
  retval =
    nRF24L01_write_reg(instance, NRF24L01_STATUS, (DRV_BIT6 | DRV_BIT5 | DRV_BIT4)) & (DRV_BIT6 | DRV_BIT5 | DRV_BIT4);
  retval |= nRF24L01_read_reg(instance, NRF24L01_STATUS) & (DRV_BIT3 | DRV_BIT2 | DRV_BIT1 | DRV_BIT0);

  return retval;
}

void nRF24L01_clear_irq_flag(nRF24L01_t *instance, nRF24L01_irq_source_t int_source)
{
  nRF24L01_write_reg(instance, NRF24L01_STATUS, NRF24L01_SET_BIT(int_source));
}

uint8_t nRF24L01_get_irq_flags(nRF24L01_t *instance)
{
  return nRF24L01_nop(instance) & (DRV_BIT6 | DRV_BIT5 | DRV_BIT4);
}

void nRF24L01_open_pipe(nRF24L01_t *instance, nRF24L01_address_t pipe_num, bool auto_ack)
{
  nRF24L01_en_pipes_t en_rxaddr;
  nRF24L01_en_pipes_t en_aa;

  en_rxaddr.value = nRF24L01_read_reg(instance, NRF24L01_EN_RXADDR);
  en_aa.value = nRF24L01_read_reg(instance, NRF24L01_EN_AA);

  switch (pipe_num) {
  case NRF24L01_PIPE0:
  case NRF24L01_PIPE1:
  case NRF24L01_PIPE2:
  case NRF24L01_PIPE3:
  case NRF24L01_PIPE4:
  case NRF24L01_PIPE5:
    en_rxaddr.value = en_rxaddr.value | NRF24L01_SET_BIT(pipe_num);

    if (auto_ack) {
      en_aa.value = en_aa.value | NRF24L01_SET_BIT(pipe_num);
    }
    else{
      en_aa.value = en_aa.value & (uint8_t) ~NRF24L01_SET_BIT(pipe_num);
    }
    break;

  case NRF24L01_ALL:
    en_rxaddr.value = (uint8_t) (~(DRV_BIT6 | DRV_BIT7));

    if (auto_ack) {
      en_aa.value = (uint8_t) (~(DRV_BIT6 | DRV_BIT7));
    }
    else{
      en_aa.value = 0U;
    }
    break;

  case NRF24L01_TX:
  default:
    break;
  }

  nRF24L01_write_reg(instance, NRF24L01_EN_RXADDR, en_rxaddr.value);
  nRF24L01_write_reg(instance, NRF24L01_EN_AA, en_aa.value);
}

void nRF24L01_close_pipe(nRF24L01_t *instance, nRF24L01_address_t pipe_num)
{
  nRF24L01_en_pipes_t en_rxaddr;
  nRF24L01_en_pipes_t en_aa;

  en_rxaddr.value = nRF24L01_read_reg(instance, NRF24L01_EN_RXADDR);
  en_aa.value = nRF24L01_read_reg(instance, NRF24L01_EN_AA);

  switch (pipe_num) {
  case NRF24L01_PIPE0:
  case NRF24L01_PIPE1:
  case NRF24L01_PIPE2:
  case NRF24L01_PIPE3:
  case NRF24L01_PIPE4:
  case NRF24L01_PIPE5:
    en_rxaddr.value = en_rxaddr.value & (uint8_t) ~NRF24L01_SET_BIT(pipe_num);
    en_aa.value = en_aa.value & (uint8_t) ~NRF24L01_SET_BIT(pipe_num);
    break;

  case NRF24L01_ALL:
    en_rxaddr.value = 0U;
    en_aa.value = 0U;
    break;

  case NRF24L01_TX:
  default:
    break;
  }

  nRF24L01_write_reg(instance, NRF24L01_EN_RXADDR, en_rxaddr.value);
  nRF24L01_write_reg(instance, NRF24L01_EN_AA, en_aa.value);
}

void nRF24L01_set_address(nRF24L01_t *instance, const nRF24L01_address_t address, const uint8_t *addr)
{
  switch (address) {
  case NRF24L01_TX:
  case NRF24L01_PIPE0:
  case NRF24L01_PIPE1:
    nRF24L01_write_multibyte_reg(instance,
                                 NRF24L01_W_REGISTER + NRF24L01_RX_ADDR_P0 + (uint8_t) address,
                                 addr,
                                 nRF24L01_get_address_width(instance));
    break;

  case NRF24L01_PIPE2:
  case NRF24L01_PIPE3:
  case NRF24L01_PIPE4:
  case NRF24L01_PIPE5:
    nRF24L01_write_reg(instance, NRF24L01_RX_ADDR_P0 + (uint8_t) address, *addr);
    break;

  case NRF24L01_ALL:
  default:
    break;
  }
}

uint8_t nRF24L01_get_address(nRF24L01_t *instance, uint8_t address, uint8_t *addr)
{
  switch (address) {
  case NRF24L01_PIPE0:
  case NRF24L01_PIPE1:
  case NRF24L01_TX:
    return (uint8_t) nRF24L01_read_multibyte_reg(instance, address, addr);

  default:
    *addr = nRF24L01_read_reg(instance, NRF24L01_RX_ADDR_P0 + address);
    return 1U;
  }
}

void nRF24L01_set_auto_retr(nRF24L01_t *instance, uint8_t retr, uint16_t delay)
{
  nRF24L01_setup_retr_t setup_retr;

  setup_retr.bits.ard = (uint8_t) (delay >> 8);
  setup_retr.bits.arc = retr;

  nRF24L01_write_reg(instance, NRF24L01_SETUP_RETR, setup_retr.value);
}

void nRF24L01_set_address_width(nRF24L01_t *instance, nRF24L01_address_width_t address_width)
{
  nRF24L01_setup_aw_t setup_aw;

  setup_aw.value = 0U;
  setup_aw.bits.aw = (uint8_t) address_width - 2U;

  nRF24L01_write_reg(instance, NRF24L01_SETUP_AW, setup_aw.value);
}

uint8_t nRF24L01_get_address_width(nRF24L01_t *instance)
{
  return nRF24L01_read_reg(instance, NRF24L01_SETUP_AW) + 2U;
}

void nRF24L01_set_rx_payload_width(nRF24L01_t *instance, uint8_t pipe_num, uint8_t pload_width)
{
  nRF24L01_write_reg(instance, NRF24L01_RX_PW_P0 + pipe_num, pload_width);
}

uint8_t nRF24L01_get_pipe_status(nRF24L01_t *instance, uint8_t pipe_num)
{
  nRF24L01_en_pipes_t en_rxaddr;
  nRF24L01_en_pipes_t en_aa;
  uint8_t en_rx_r, en_aa_r;

  en_rxaddr.value = nRF24L01_read_reg(instance, NRF24L01_EN_RXADDR);
  en_aa.value = nRF24L01_read_reg(instance, NRF24L01_EN_AA);

  switch (pipe_num) {
  case 0:
    en_rx_r = en_rxaddr.bits.pipe_0;
    en_aa_r = en_aa.bits.pipe_0;
    break;

  case 1:
    en_rx_r = en_rxaddr.bits.pipe_1;
    en_aa_r = en_aa.bits.pipe_1;
    break;

  case 2:
    en_rx_r = en_rxaddr.bits.pipe_2;
    en_aa_r = en_aa.bits.pipe_2;
    break;

  case 3:
    en_rx_r = en_rxaddr.bits.pipe_3;
    en_aa_r = en_aa.bits.pipe_3;
    break;

  case 4:
    en_rx_r = en_rxaddr.bits.pipe_4;
    en_aa_r = en_aa.bits.pipe_4;
    break;

  case 5:
    en_rx_r = en_rxaddr.bits.pipe_5;
    en_aa_r = en_aa.bits.pipe_5;
    break;

  default:
    en_rx_r = 0U;
    en_aa_r = 0U;
    break;
  }

  return (uint8_t) (en_aa_r << 1) + en_rx_r;
}

uint8_t nRF24L01_get_auto_retr_status(nRF24L01_t *instance)
{
  return nRF24L01_read_reg(instance, NRF24L01_OBSERVE_TX);
}

uint8_t nRF24L01_get_packet_lost_ctr(nRF24L01_t *instance)
{
  return (nRF24L01_read_reg(instance, NRF24L01_OBSERVE_TX) & (DRV_BIT7 | DRV_BIT6 | DRV_BIT5 | DRV_BIT4)) >> 4;
}

uint8_t nRF24L01_get_rx_payload_width(nRF24L01_t *instance, uint8_t pipe_num)
{
  uint8_t pw;

  switch (pipe_num) {
  case 0:
    pw = nRF24L01_read_reg(instance, NRF24L01_RX_PW_P0);
    break;

  case 1:
    pw = nRF24L01_read_reg(instance, NRF24L01_RX_PW_P1);
    break;

  case 2:
    pw = nRF24L01_read_reg(instance, NRF24L01_RX_PW_P2);
    break;

  case 3:
    pw = nRF24L01_read_reg(instance, NRF24L01_RX_PW_P3);
    break;

  case 4:
    pw = nRF24L01_read_reg(instance, NRF24L01_RX_PW_P4);
    break;

  case 5:
    pw = nRF24L01_read_reg(instance, NRF24L01_RX_PW_P5);
    break;

  default:
    pw = 0U;
    break;
  }

  return pw;
}

void nRF24L01_set_rf_channel(nRF24L01_t *instance, uint8_t channel)
{
  nRF24L01_rf_ch_t rf_ch;

  rf_ch.value = 0U;
  rf_ch.bits.rf_ch = channel;
  nRF24L01_write_reg(instance, NRF24L01_RF_CH, rf_ch.value);
}

void nRF24L01_set_output_power(nRF24L01_t *instance, nRF24L01_output_power_t power)
{
  nRF24L01_rf_setup_t rf_setup;

  rf_setup.value = nRF24L01_read_reg(instance, NRF24L01_RF_SETUP);

  rf_setup.bits.rf_pwr = (uint8_t) power;

  nRF24L01_write_reg(instance, NRF24L01_RF_SETUP, rf_setup.value);
}

void nRF24L01_set_datarate(nRF24L01_t *instance, nRF24L01_datarate_t datarate)
{
  nRF24L01_rf_setup_t rf_setup;

  rf_setup.value = nRF24L01_read_reg(instance, NRF24L01_RF_SETUP);

  switch (datarate) {
  case NRF24L01_250KBPS:
    rf_setup.bits.rf_dr_low = 1U;
    rf_setup.bits.rf_dr_high = 0U;
    break;

  case NRF24L01_1MBPS:
    rf_setup.bits.rf_dr_low = 0U;
    rf_setup.bits.rf_dr_high = 0U;
    break;

  case NRF24L01_2MBPS:
  default:
    rf_setup.bits.rf_dr_low = 0U;
    rf_setup.bits.rf_dr_high = 1U;
    break;
  }

  nRF24L01_write_reg(instance, NRF24L01_RF_SETUP, rf_setup.value);
}

bool nRF24L01_rx_fifo_empty(nRF24L01_t *instance)
{
  return (bool) ((nRF24L01_read_reg(instance, NRF24L01_FIFO_STATUS) >> NRF24L01_RX_EMPTY) & 0x01U);
}

bool nRF24L01_rx_fifo_full(nRF24L01_t *instance)
{
  return (bool) ((nRF24L01_read_reg(instance, NRF24L01_FIFO_STATUS) >> NRF24L01_RX_FULL) & 0x01U);
}

bool nRF24L01_tx_fifo_empty(nRF24L01_t *instance)
{
  return (bool) ((nRF24L01_read_reg(instance, NRF24L01_FIFO_STATUS) >> NRF24L01_TX_EMPTY) & 0x01U);
}

bool nRF24L01_tx_fifo_full(nRF24L01_t *instance)
{
  return (bool) ((nRF24L01_read_reg(instance, NRF24L01_FIFO_STATUS) >> NRF24L01_TX_FIFO_FULL) & 0x01U);
}

uint8_t nRF24L01_get_tx_fifo_status(nRF24L01_t *instance)
{
  return (nRF24L01_read_reg(instance,
                            NRF24L01_FIFO_STATUS) & ((1U << NRF24L01_TX_FIFO_FULL) | (1U << NRF24L01_TX_EMPTY))) >> 4;
}

uint8_t nRF24L01_get_rx_fifo_status(nRF24L01_t *instance)
{
  return nRF24L01_read_reg(instance, NRF24L01_FIFO_STATUS) & ((1U << NRF24L01_RX_FULL) | (1U << NRF24L01_RX_EMPTY));
}

uint8_t nRF24L01_get_fifo_status(nRF24L01_t *instance)
{
  return nRF24L01_read_reg(instance, NRF24L01_FIFO_STATUS);
}

uint8_t nRF24L01_get_transmit_attempts(nRF24L01_t *instance)
{
  return nRF24L01_read_reg(instance, NRF24L01_OBSERVE_TX) & (DRV_BIT3 | DRV_BIT2 | DRV_BIT1 | DRV_BIT0);
}

bool nRF24L01_get_carrier_detect(nRF24L01_t *instance)
{
  return (bool) (nRF24L01_read_reg(instance, NRF24L01_CD) & 0x01U);
}

void nRF24L01_activate_features(nRF24L01_t *instance)
{
  return;
}

void nRF24L01_setup_dynamic_payload(nRF24L01_t *instance, uint8_t setup)
{
  nRF24L01_en_pipes_t dynpd;

  dynpd.value = setup & (uint8_t) ~0xC0U;

  nRF24L01_write_reg(instance, NRF24L01_DYNPD, dynpd.value);
}

void nRF24L01_enable_dynamic_payload(nRF24L01_t *instance, bool enable)
{
  nRF24L01_feature_t feature;

  feature.value = nRF24L01_read_reg(instance, NRF24L01_FEATURE);
  feature.bits.en_dpl = (enable) ? 1U : 0U;

  nRF24L01_write_reg(instance, NRF24L01_FEATURE, feature.value);
}

void nRF24L01_enable_ack_payload(nRF24L01_t *instance, bool enable)
{
  nRF24L01_feature_t feature;

  feature.value = nRF24L01_read_reg(instance, NRF24L01_FEATURE);
  feature.bits.en_ack_pay = (enable) ? 1U : 0U;

  nRF24L01_write_reg(instance, NRF24L01_FEATURE, feature.value);
}

void nRF24L01_enable_dynamic_ack(nRF24L01_t *instance, bool enable)
{
  nRF24L01_feature_t feature;

  feature.value = nRF24L01_read_reg(instance, NRF24L01_FEATURE);
  feature.bits.en_dyn_ack = (enable) ? 1U : 0U;

  nRF24L01_write_reg(instance, NRF24L01_FEATURE, feature.value);
}

void nRF24L01_write_tx_payload(nRF24L01_t *instance, const uint8_t *tx_pload, uint8_t length)
{
  nRF24L01_write_multibyte_reg(instance, NRF24L01_W_TX_PAYLOAD, tx_pload, length);
}

void nRF24L01_write_tx_payload_noack(nRF24L01_t *instance, const uint8_t *tx_pload, uint8_t length)
{
  nRF24L01_write_multibyte_reg(instance, NRF24L01_W_TX_PAYLOAD_NOACK, tx_pload, length);
}

void nRF24L01_write_ack_payload(nRF24L01_t *instance, uint8_t pipe, const uint8_t *tx_pload, uint8_t length)
{
  nRF24L01_write_multibyte_reg(instance, NRF24L01_W_ACK_PAYLOAD | pipe, tx_pload, length);
}

uint8_t nRF24L01_read_rx_payload_width(nRF24L01_t *instance)
{
  return nRF24L01_read_reg(instance, NRF24L01_R_RX_PL_WID);
}

uint16_t nRF24L01_read_rx_payload(nRF24L01_t *instance, uint8_t *rx_pload)
{
  return nRF24L01_read_multibyte_reg(instance, NRF24L01_UINT8(NRF24L01_RX_PLOAD), rx_pload);
}

uint8_t nRF24L01_get_rx_data_source(nRF24L01_t *instance)
{
  return (nRF24L01_nop(instance) & (DRV_BIT3 | DRV_BIT2 | DRV_BIT1)) >> 1;
}

void nRF24L01_reuse_tx(nRF24L01_t *instance)
{
  CSN_LOW();
  nRF24L01_rw(instance, NRF24L01_REUSE_TX_PL);
  CSN_HIGH();
}

bool nRF24L01_get_reuse_tx_status(nRF24L01_t *instance)
{
  return (bool) ((nRF24L01_get_fifo_status(instance) & (1U << NRF24L01_TX_REUSE)) >> NRF24L01_TX_REUSE);
}

void nRF24L01_flush_rx(nRF24L01_t *instance)
{
  CSN_LOW();
  nRF24L01_rw(instance, NRF24L01_FLUSH_RX);
  CSN_HIGH();
}

void nRF24L01_flush_tx(nRF24L01_t *instance)
{
  CSN_LOW();
  nRF24L01_rw(instance, NRF24L01_FLUSH_TX);
  CSN_HIGH();
}

uint8_t nRF24L01_nop(nRF24L01_t *instance)
{
  uint8_t retval;

  CSN_LOW();
  retval = nRF24L01_rw(instance, NRF24L01_NOP);
  CSN_HIGH();

  return retval;
}

void nRF24L01_set_pll_mode(nRF24L01_t *instance, bool pll_lock)
{
  nRF24L01_rf_setup_t rf_setup;

  rf_setup.value = nRF24L01_read_reg(instance, NRF24L01_RF_SETUP);
  rf_setup.bits.pll_lock = (pll_lock) ? 1U : 0U;

  nRF24L01_write_reg(instance, NRF24L01_RF_SETUP, rf_setup.value);
}

void nRF24L01_enable_continious_wave(nRF24L01_t *instance, bool enable)
{
  nRF24L01_rf_setup_t rf_setup;

  rf_setup.value = nRF24L01_read_reg(instance, NRF24L01_RF_SETUP);
  rf_setup.bits.cont_wave = (enable ? 1U : 0U);

  nRF24L01_write_reg(instance, NRF24L01_RF_SETUP, rf_setup.value);
}

uint8_t nRF24L01_read_reg(nRF24L01_t *instance, uint8_t reg)
{
  uint8_t tx_buf[1];
  uint8_t rx_buf[2];

  tx_buf[0] = NRF24L01_R_REGISTER | reg;

  CSN_LOW();
  instance->blockingTransfer(instance->spiCtx, tx_buf, 1, rx_buf, 2);
  CSN_HIGH();

  return rx_buf[1];
}

uint8_t nRF24L01_write_reg(nRF24L01_t *instance, uint8_t reg, uint8_t value)
{
  uint8_t retval;
  uint8_t tx_buf[2];

  tx_buf[0] = NRF24L01_W_REGISTER | reg;
  tx_buf[1] = value;

  CSN_LOW();
  instance->blockingTransfer(instance->spiCtx, tx_buf, 2, &retval, 1);
  CSN_HIGH();
  return retval;
}

uint16_t nRF24L01_read_multibyte_reg(nRF24L01_t *instance, uint8_t reg, uint8_t *pbuf)
{
  uint8_t length;
  uint8_t command;

  switch (reg) {
  case NRF24L01_PIPE0:
  case NRF24L01_PIPE1:
  case NRF24L01_TX:
    length = nRF24L01_get_address_width(instance);

    command = NRF24L01_RX_ADDR_P0 + reg;
    break;

  case NRF24L01_RX_PLOAD:
    reg = nRF24L01_get_rx_data_source(instance);
    if (reg < 7U) {
      length = 32;
      command = NRF24L01_R_RX_PAYLOAD;
    }
    else{
      length = 0U;
    }
    break;

  default:
    length = 0U;
    break;
  }

  if (length == 0) {
    return (uint16_t) reg << 8;
  }

  CSN_LOW();
  uint8_t status;

  instance->blockingTransfer(instance->spiCtx, &command, 1, &status, 1);
  instance->blockingTransfer(instance->spiCtx, NULL, 0, pbuf, length);
  CSN_HIGH();

  return ((uint16_t) reg << 8) | length;
}

void nRF24L01_write_multibyte_reg(nRF24L01_t *instance, uint8_t reg, const uint8_t *pbuf, uint8_t length)
{
  CSN_LOW();
  uint8_t status;

  instance->blockingTransfer(instance->spiCtx, &reg, 1, &status, 1);
  instance->blockingTransfer(instance->spiCtx, (uint8_t *) pbuf, length, NULL, 0);
  CSN_HIGH();
}

uint8_t nRF24L01_rw(nRF24L01_t *instance, uint8_t value)
{
  CSN_LOW();
  uint8_t status;

  instance->blockingTransfer(instance->spiCtx, &value, 1, &status, 1);
  CSN_HIGH();

  return status;
}

void nRF24L01_initialize(nRF24L01_t *instance, drv_spi_tf_t blockingTransfer, void *spiCtx,
                         void (*setCS)(uint8_t val))
{
  instance->blockingTransfer = blockingTransfer;
  instance->spiCtx = spiCtx;
  instance->setCS = setCS;
}

void nRF24L01_deinitialize(nRF24L01_t *instance)
{
  instance->blockingTransfer = NULL;
  instance->spiCtx = NULL;
}
