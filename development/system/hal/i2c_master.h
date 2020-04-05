/******************************************************************************
 * @file     i2c_master.h
 * @brief   Falcon I2C Hardware Abstraction Layer Header File
 * @version  1.0
 * @date     2020-04-01
 * @author   Adam Bujak
 ******************************************************************************/

#ifndef FI2C_MASTER_HAL_H
#define FI2C_MASTER_HAL_H


#include "fln_bsp.h"
#include "fiodt.h"
#include <stdbool.h>
#include <stdlib.h>


#define FI2C_MAX_TRANSFER_LENGTH 65535 
#define FI2C_NO_TIMEOUT          0

typedef uint8_t   fi2c_addr_t;
typedef uint8_t   fi2c_reg_t;
typedef uint16_t  fi2c_len_t;

typedef struct fi2c_struct fi2c_t;
typedef struct fi2c_transfer_data_struct fi2c_transfer_t;

typedef void          ( * fi2c_cb_t )    ( fi2c_t * instance );                                                     /* Callback function pointer type */
typedef fio_retval_e  ( * fi2c_btf_t )   ( fi2c_t * instance, fi2c_transfer_t * transferData );                     /* Blocking transfer function pointer type */
typedef fio_retval_e  ( * fi2c_nbtf_t )  ( fi2c_t * instance, fi2c_transfer_t * transferData, fi2c_cb_t callback);  /* Non blocking transfer function pointer type */

typedef enum
{
  FI2C_TRANSFER_DIR_WRITE = 0,
  FI2C_TRANSFER_DIR_READ,
  FI2C_TRANSFER_DIR_CNT
} fi2c_transfer_dir_e;




struct fi2c_transfer_data_struct
{
  /* @public */
  fi2c_addr_t            slaveAddr;  /* Address of slave device */

  fi2c_addr_t            readReg;    /* Register address to be read from */
  fi2c_reg_t           * rxBuf;      /* Buffer to be written to */

  fi2c_addr_t            writeReg;   /* Register address to be written to */
  fi2c_reg_t           * txBuf;      /* Buffer to be read from */

  fi2c_len_t             length;     /* Length of transfer */

  uint32_t               timeout;    /* Timeout value */

  /* @private */
  fi2c_transfer_dir_e    direction;  /* Receive or Trasmit */
  fi2c_cb_t              xferDoneCB; /* Callback function for when transfer is done */
};

struct fi2c_struct
{
  /* @public */
  fi2c_btf_t        blockingWrite;        /* Write function pointer */
  fi2c_btf_t        blockingRead;         /* Read function pointer */

  fi2c_nbtf_t       nonBlockingWrite;     /* Write function pointer */
  fi2c_nbtf_t       nonBlockingRead;      /* Read function pointer */

  /* @private */
  bool              isInitialized;        /* Module is initialized flag */
  volatile bool     isTransferring;       /* Currently transferring flag */
  fio_module_t      module;               /* I2C module value */
  bio_module_t      baseAddr;             /* Hardware I2C module base address */
  fi2c_len_t        xferIndex;            /* Current index of current transfer */
  fi2c_transfer_t * transferData;         /* Transfer Data of current packet */
};


fi2c_t * FI2C_Initialize   ( fio_module_t module, const eUSCI_I2C_MasterConfig * config );
fi2c_t * FI2C_Deinitialize ( fi2c_t * instance );

#endif // FI2C_MASTER_HAL_H
