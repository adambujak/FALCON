/** @file
@brief Register definitions for nRF24L01+

@details Header file defining register mapping with bit definitions.
This file is radio-chip dependent, and are included with the nRF24L01.h

@ingroup nRF24L0124l01p_nRF24L01
 */

#ifndef NRF24L01_REG_H__
#define NRF24L01_REG_H__

/** @name - Instruction Set - */
//@{
/* nRF24L01 Instruction Definitions */
#define NRF24L01_R_REGISTER         0x00U
#define NRF24L01_W_REGISTER         0x20U  /**< Register write command */
#define NRF24L01_R_RX_PAYLOAD       0x61U  /**< Read RX payload command */
#define NRF24L01_W_TX_PAYLOAD       0xA0U  /**< Write TX payload command */
#define NRF24L01_FLUSH_TX           0xE1U  /**< Flush TX register command */
#define NRF24L01_FLUSH_RX           0xE2U  /**< Flush RX register command */
#define NRF24L01_REUSE_TX_PL        0xE3U  /**< Reuse TX payload command */
#define NRF24L01_ACTIVATE           0x50U  /**< Activate features */
#define NRF24L01_R_RX_PL_WID        0x60U  /**< Read RX payload command */
#define NRF24L01_W_ACK_PAYLOAD      0xA8U  /**< Write ACK payload command */
#define NRF24L01_W_TX_PAYLOAD_NOACK 0xB0U  /**< Write ACK payload command */
#define NRF24L01_NOP                0xFFU  /**< No Operation command, used for reading status register */
//@}

/** @name  - Register Memory Map - */
//@{
/* nRF24L01 * Register Definitions * */
#define NRF24L01_CONFIG        0x00U  /**< nRF24L01 config register */
#define NRF24L01_EN_AA         0x01U  /**< nRF24L01 enable Auto-Acknowledge register */
#define NRF24L01_EN_RXADDR     0x02U  /**< nRF24L01 enable RX addresses register */
#define NRF24L01_SETUP_AW      0x03U  /**< nRF24L01 setup of address width register */
#define NRF24L01_SETUP_RETR    0x04U  /**< nRF24L01 setup of automatic retransmission register */
#define NRF24L01_RF_CH         0x05U  /**< nRF24L01 RF channel register */
#define NRF24L01_RF_SETUP      0x06U  /**< nRF24L01 RF setup register */
#define NRF24L01_STATUS        0x07U  /**< nRF24L01 status register */
#define NRF24L01_OBSERVE_TX    0x08U  /**< nRF24L01 transmit observe register */
#define NRF24L01_CD            0x09U  /**< nRF24L01 carrier detect register */
#define NRF24L01_RX_ADDR_P0    0x0AU  /**< nRF24L01 receive address data pipe0 */
#define NRF24L01_RX_ADDR_P1    0x0BU  /**< nRF24L01 receive address data pipe1 */
#define NRF24L01_RX_ADDR_P2    0x0CU  /**< nRF24L01 receive address data pipe2 */
#define NRF24L01_RX_ADDR_P3    0x0DU  /**< nRF24L01 receive address data pipe3 */
#define NRF24L01_RX_ADDR_P4    0x0EU  /**< nRF24L01 receive address data pipe4 */
#define NRF24L01_RX_ADDR_P5    0x0FU  /**< nRF24L01 receive address data pipe5 */
#define NRF24L01_TX_ADDR       0x10U  /**< nRF24L01 transmit address */
#define NRF24L01_RX_PW_P0      0x11U  /**< nRF24L01 \# of bytes in rx payload for pipe0 */
#define NRF24L01_RX_PW_P1      0x12U  /**< nRF24L01 \# of bytes in rx payload for pipe1 */
#define NRF24L01_RX_PW_P2      0x13U  /**< nRF24L01 \# of bytes in rx payload for pipe2 */
#define NRF24L01_RX_PW_P3      0x14U  /**< nRF24L01 \# of bytes in rx payload for pipe3 */
#define NRF24L01_RX_PW_P4      0x15U  /**< nRF24L01 \# of bytes in rx payload for pipe4 */
#define NRF24L01_RX_PW_P5      0x16U  /**< nRF24L01 \# of bytes in rx payload for pipe5 */
#define NRF24L01_FIFO_STATUS   0x17U  /**< nRF24L01 FIFO status register */
#define NRF24L01_DYNPD         0x1CU  /**< nRF24L01 Dynamic payload setup */
#define NRF24L01_FEATURE       0x1DU  /**< nRF24L01 Exclusive feature setup */

//@}

/* nRF24L01 related definitions */
/* Interrupt definitions */
/* Operation mode definitions */

/** An enum describing the radio's irq sources.
 *
 */
typedef enum {
    NRF24L01_MAX_RT = 4,     /**< Max retries interrupt */
    NRF24L01_TX_DS,          /**< TX data sent interrupt */
    NRF24L01_RX_DR           /**< RX data received interrupt */
} nRF24L01_irq_source_t;

/* Operation mode definitions */
/** An enum describing the radio's power mode.
 *
 */
typedef enum {
    NRF24L01_PTX,            /**< Primary TX operation */
    NRF24L01_PRX             /**< Primary RX operation */
} nRF24L01_operation_mode_t;

/** An enum describing the radio's power mode.
 *
 */
typedef enum {
    NRF24L01_PWR_DOWN,       /**< Device power-down */
    NRF24L01_PWR_UP          /**< Device power-up */
} nRF24L01_pwr_mode_t;

/** An enum describing the radio's output power mode's.
 *
 */
typedef enum {
    NRF24L01_18DBM,          /**< Output power set to -18dBm */
    NRF24L01_12DBM,          /**< Output power set to -12dBm */
    NRF24L01_6DBM,           /**< Output power set to -6dBm  */
    NRF24L01_0DBM            /**< Output power set to 0dBm   */
} nRF24L01_output_power_t;

/** An enum describing the radio's on-air datarate.
 *
 */
typedef enum {
    NRF24L01_1MBPS,          /**< Datarate set to 1 Mbps  */
    NRF24L01_2MBPS,          /**< Datarate set to 2 Mbps  */
    NRF24L01_250KBPS         /**< Datarate set to 250 kbps*/
} nRF24L01_datarate_t;

/** An enum describing the radio's CRC mode.
 *
 */
typedef enum {
    NRF24L01_CRC_OFF,    /**< CRC check disabled */
    NRF24L01_CRC_8BIT,   /**< CRC check set to 8-bit */
    NRF24L01_CRC_16BIT   /**< CRC check set to 16-bit */
} nRF24L01_crc_mode_t;

/** An enum describing the read/write payload command.
 *
 */
typedef enum {
    NRF24L01_TX_PLOAD = 7,   /**< TX payload definition */
    NRF24L01_RX_PLOAD,        /**< RX payload definition */
    NRF24L01_ACK_PLOAD
} nRF24L01_pload_command_t;

/** Structure containing the radio's address map.
 * Pipe0 contains 5 unique address bytes,
 * while pipe[1..5] share the 4 MSB bytes, set in pipe1.
 * <p><b> - Remember that the LSB byte for all pipes have to be unique! -</b>
 */
// nRF24L01 Address struct


//typedef struct {
//   uint8_t p0[5];            /**< Pipe0 address, 5 bytes */
//    uint8_t p1[5];            /**< Pipe1 address, 5 bytes, 4 MSB bytes shared for pipe1 to pipe5 */
//    uint8_t p2[1];            /**< Pipe2 address, 1 byte */
//    uint8_t p3[1];            /**< Pipe3 address, 1 byte */
//   uint8_t p4[1];            /**< Pipe3 address, 1 byte */
//   uint8_t p5[1];            /**< Pipe3 address, 1 byte */
//   uint8_t tx[5];            /**< TX address, 5 byte */
//} nRF24L01_l01_addr_map;


/** An enum describing the nRF24L01 pipe addresses and TX address.
 *
 */
typedef enum {
    NRF24L01_PIPE0 = 0,          /**< Select pipe0 */
    NRF24L01_PIPE1,              /**< Select pipe1 */
    NRF24L01_PIPE2,              /**< Select pipe2 */
    NRF24L01_PIPE3,              /**< Select pipe3 */
    NRF24L01_PIPE4,              /**< Select pipe4 */
    NRF24L01_PIPE5,              /**< Select pipe5 */
    NRF24L01_TX,                 /**< Refer to TX address*/
    NRF24L01_ALL = 0xFF          /**< Close or open all pipes*/
                                /**< @see nRF24L01_set_address @see nRF24L01_get_address
                                 @see nRF24L01_open_pipe  @see nRF24L01_close_pipe */
} nRF24L01_address_t;

/** An enum describing the radio's address width.
 *
 */
typedef enum {
    NRF24L01_AW_3BYTES = 3,      /**< Set address width to 3 bytes */
    NRF24L01_AW_4BYTES,          /**< Set address width to 4 bytes */
    NRF24L01_AW_5BYTES           /**< Set address width to 5 bytes */
} nRF24L01_address_width_t;


/** @name CONFIG register bit definitions */
//@{

#define NRF24L01_MASK_RX_DR    6     /**< CONFIG register bit 6 */
#define NRF24L01_MASK_TX_DS    5     /**< CONFIG register bit 5 */
#define NRF24L01_MASK_MAX_RT   4     /**< CONFIG register bit 4 */
#define NRF24L01_EN_CRC        3     /**< CONFIG register bit 3 */
#define NRF24L01_CRCO          2     /**< CONFIG register bit 2 */
#define NRF24L01_PWR_UP        1     /**< CONFIG register bit 1 */
#define NRF24L01_PRIM_RX       0     /**< CONFIG register bit 0 */
//@}

/** @name RF_SETUP register bit definitions */
//@{
#define NRF24L01_PLL_LOCK      4     /**< RF_SETUP register bit 4 */
#define NRF24L01_RF_DR         3     /**< RF_SETUP register bit 3 */
#define NRF24L01_RF_PWR1       2     /**< RF_SETUP register bit 2 */
#define NRF24L01_RF_PWR0       1     /**< RF_SETUP register bit 1 */
#define NRF24L01_LNA_HCURR     0     /**< RF_SETUP register bit 0 */
//@}

/* STATUS 0x07 */
/** @name STATUS register bit definitions */
//@{
#define NRF24L01_RX_DR         6     /**< STATUS register bit 6 */
#define NRF24L01_TX_DS         5     /**< STATUS register bit 5 */
#define NRF24L01_MAX_RT        4     /**< STATUS register bit 4 */
#define NRF24L01_TX_FULL       0     /**< STATUS register bit 0 */
//@}

/* FIFO_STATUS 0x17 */
/** @name FIFO_STATUS register bit definitions */
//@{
#define NRF24L01_TX_REUSE      6     /**< FIFO_STATUS register bit 6 */
#define NRF24L01_TX_FIFO_FULL  5     /**< FIFO_STATUS register bit 5 */
#define NRF24L01_TX_EMPTY      4     /**< FIFO_STATUS register bit 4 */
#define NRF24L01_RX_FULL       1     /**< FIFO_STATUS register bit 1 */
#define NRF24L01_RX_EMPTY      0     /**< FIFO_STATUS register bit 0 */
//@}

#endif // NRF24L01_REG_H__
/** @} */
