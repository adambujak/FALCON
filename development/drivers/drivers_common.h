/******************************************************************************
* @file     drivers_common.h
* @brief    Common header file for drivers
* @version  1.0
* @date     2020-04-09
* @author   Adam Bujak
******************************************************************************/

#ifndef DRIVERS_COMMON_H
#define DRIVERS_COMMON_H

/** Swaps the upper byte with the lower byte in a 16 bit variable */
//lint -emacro((572),SWAP) // Suppress warning 572 "Excessive shift value"
#define SWAP(x)      ((((x) & 0xFFU) << 8) | (((x) >> 8) & 0xFFU))

/** The upper 8 bits of a 16 bit value */
#define MSB(a)       (((a) & 0xFF00U) >> 8)
/** The lower 8 bits (of a 16 bit value) */
#define LSB(a)       (((a) & 0xFFU))

/** Leaves the minimum of the two arguments */
#define MIN(a, b)    ((a) < (b) ? (a) : (b))
/** Leaves the maximum of the two arguments */
#define MAX(a, b)    ((a) < (b) ? (b) : (a))

#define DRV_BIT0     0x01U   /**< The value of bit 0 */
#define DRV_BIT1     0x02U   /**< The value of bit 1 */
#define DRV_BIT2     0x04U   /**< The value of bit 2 */
#define DRV_BIT3     0x08U   /**< The value of bit 3 */
#define DRV_BIT4     0x10U   /**< The value of bit 4 */
#define DRV_BIT5     0x20U   /**< The value of bit 5 */
#define DRV_BIT6     0x40U   /**< The value of bit 6 */
#define DRV_BIT7     0x80U   /**< The value of bit 7 */
#define DRV_BIT8     0x0100U /**< The value of bit 8 */
#define DRV_BIT9     0x0200U /**< The value of bit 9 */
#define DRV_BIT10    0x0400U /**< The value of bit 10 */
#define DRV_BIT11    0x0800U /**< The value of bit 11 */
#define DRV_BIT12    0x1000U /**< The value of bit 12 */
#define DRV_BIT13    0x2000U /**< The value of bit 13 */
#define DRV_BIT14    0x4000U /**< The value of bit 14 */
#define DRV_BIT15    0x8000U /**< The value of bit 15 */

typedef void ( *drv_gpio_set_t ) (void *context, uint8_t state);

typedef struct {
  drv_gpio_set_t set;
  void          *gpioCtx;
} drv_gpio_t;

typedef void ( *drv_i2c_tf_t ) (void *context, uint8_t slave, uint8_t reg, uint8_t *buf, uint16_t len);
typedef void ( *drv_spi_tf_t ) (void *context, uint8_t *tx_buf, uint16_t tx_len, uint8_t *rx_buf, uint16_t rx_len);

#endif // DRIVERS_COMMON_H
