/******************************************************************************
 * @file     fiom.h 
 * @brief    Falcon Input/Output Module 
 * @version  1.0
 * @date     2020-04-05
 * @author   Adam Bujak
 ******************************************************************************/

#ifndef FIOM_H
#define FIOM_H

#include "i2c_master.h"
#include "fiodt.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct fiom_struct fiom_t;

typedef fio_retval_e  ( * fiom_btf_t )   ( fiom_t * instance, fio_transfer_t * transferData );                                /* Blocking transfer function pointer type */
typedef fio_retval_e  ( * fiom_nbtf_t )  ( fiom_t * instance, fio_transfer_t * transferData, fio_cb_t cb, void * context );   /* Non blocking transfer function pointer type */


typedef enum
{
  FIOM_TYPE_I2C,
  FIOM_TYPE_SPI,
  FIOM_TYPE_UART,
  FIOM_TYPE_CNT
} fiom_type_e;

typedef union
{
  eUSCI_I2C_MasterConfig * i2cConfig;
  eUSCI_SPI_MasterConfig * spiConfig;
  eUSCI_UART_Config      * uartConfig;
} fiom_config_union;

typedef struct
{
  fiom_type_e type;
  fiom_config_union config;
} fiom_config_t;

struct fiom_struct
{
  /* @public */
  fiom_btf_t        blockingWrite;        /* Write function pointer */
  fiom_btf_t        blockingRead;         /* Read function pointer */

  fiom_nbtf_t       nonBlockingWrite;     /* Write function pointer */
  fiom_nbtf_t       nonBlockingRead;      /* Read function pointer */

  /* @private */
  bool              isInitialized;        /* Module is initialized flag */
  volatile bool     isTransferring;       /* Currently transferring flag */
  fio_module_t      module;               /* I2C module value */
  bio_module_t      baseAddr;             /* Hardware I2C module base address */
  fio_transfer_t  * transferData;         /* Transfer Data of current packet */
};

fiom_t * FIOM_Initialize   ( fio_module_t module, fiom_config_t * config );
fiom_t * FIOM_Deinitialize ( fiom_t * instance );

#endif // FI2C_MASTER_HAL_H
