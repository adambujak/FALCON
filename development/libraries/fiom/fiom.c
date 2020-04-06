/******************************************************************************
 * @file     fiom.c 
 * @brief    Falcon Input/Output Module 
 * @version  1.0
 * @date     2020-04-05
 * @author   Adam Bujak
 ******************************************************************************/
//struct fiom_struct
//{
//  /* @public */
//  fiom_btf_t        blockingWrite;        /* Write function pointer */
//  fiom_btf_t        blockingRead;         /* Read function pointer */
//
//  fiom_nbtf_t       nonBlockingWrite;     /* Write function pointer */
//  fiom_nbtf_t       nonBlockingRead;      /* Read function pointer */
//
//  /* @private */
//  bool              isInitialized;        /* Module is initialized flag */
//  volatile bool     isTransferring;       /* Currently transferring flag */
//  fio_module_t      module;               /* I2C module value */
//  bio_module_t      baseAddr;             /* Hardware I2C module base address */
//  fio_transfer_t  * transferData;         /* Transfer Data of current packet */
//};

#include "fiom.h"

static fiom_t ioModules[FIO_MODULE_CNT];

fiom_t * FIOM_Initialize   ( fio_module_t module, fiom_config_t * config )
{
	if ( (config->type == FIOM_TYPE_I2C) && (module > FIO_MODULE_3) )
	{
	  // Configure I2C
	}
	else if ( config->type == FIOM_TYPE_SPI )
	{
	  // Configure SPI
	}
	else if ( (config->type == FIOM_TYPE_I2C) && (module < FIO_MODULE_4) )
	{
	  // Configure UART
	}
	else
	{
	  /* Invalid configuration */
	  return NULL;
	}
}
fiom_t * FIOM_Deinitialize ( fiom_t * instance );
