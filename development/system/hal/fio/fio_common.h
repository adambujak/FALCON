/******************************************************************************
* @file     fio_common.h
* @brief    Falcon Input/Output Common Data
* @version  1.0
* @date     2020-04-02
* @author   Adam Bujak
******************************************************************************/

#ifndef FIO_COMMON_H
#define FIO_COMMON_H

#include <stdint.h>

#define FIO_MODULE_CNT    8

/* UART modules only 0-3 and I2C only 4-7 */
#define FIO_MODULE_0      0
#define FIO_MODULE_1      1
#define FIO_MODULE_2      2
#define FIO_MODULE_3      3
#define FIO_MODULE_4      4
#define FIO_MODULE_5      5
#define FIO_MODULE_6      6
#define FIO_MODULE_7      7
typedef uint8_t   fio_module_t;

#define FIO_MODE_I2C      1
#define FIO_MODE_SPI      (1 << 1)
#define FIO_MODE_UART     (1 << 2)
typedef uint8_t   fio_mode_t;


#define FIO_ERROR_NONE            0
#define FIO_ERROR_GENERAL         (1)      /* General error */
#define FIO_ERROR_INIT            (1 << 1) /* Initialization related error */
#define FIO_ERROR_NAK             (1 << 2) /* NAK error */
#define FIO_ERROR_TIMEOUT         (1 << 3) /* Timeout error */
typedef uint8_t    fio_error_t;

#define FIO_TRANSFER_DIR_WRITE    0
#define FIO_TRANSFER_DIR_READ     1
typedef uint8_t    fio_transfer_dir_t;

typedef uint8_t    fio_addr_t;                                     /* Addresss type for IO transfers */
typedef uint8_t    fio_reg_t;                                      /* Register type for IO transfers */
typedef uint16_t   fio_len_t;                                      /* Length type for IO transfers */

typedef struct
{
  uint32_t port;
  uint32_t pin;
} fio_gpio_t;

typedef union
{
  fio_gpio_t spiCSPin;     /* SPI chip select pin */
  fio_addr_t i2cSlaveAddr; /* I2C address of slave device */
} fio_peer_info_t;

typedef void ( *fio_cb_func_t ) (void *context, uint8_t status);        /* Callback function pointer type */

typedef struct
{
  fio_cb_func_t callbackFunction;
  void *        context;
} fio_callback_t;

typedef struct fio_transfer_data_struct
{
  /* @public */
  fio_peer_info_t    peerInfo;      /* Slave info - ie. slave address for i2c, CS for SPI */

  fio_addr_t         readReg;       /* Register address to be read from */
  fio_reg_t *        rxBuf;         /* Buffer to be written to */

  fio_addr_t         writeReg;      /* Register address to be written to */
  fio_reg_t *        txBuf;         /* Buffer to be read from */

  fio_len_t          length;        /* Length of transfer */

  uint32_t           timeout;       /* Timeout value */

  /* @private */
  fio_transfer_dir_t direction;     /* Receive or Trasmit */
  fio_callback_t     xferDoneCB;    /* Callback function for when transfer is done */
} fio_transfer_t;

#endif // FIO_COMMON_H
