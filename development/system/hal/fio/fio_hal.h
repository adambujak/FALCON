/******************************************************************************
* @file     fiom.h
* @brief    Falcon Input/Output Module
* @version  1.0
* @date     2020-04-05
* @author   Adam Bujak
******************************************************************************/

#ifndef FIO_HAL_H
#define FIO_HAL_H



#include "fi2c_hal.h"
#include "fio_common.h"
//#include "fspi_hal.h"
//#include "fuart_hal.h"
#include <stdbool.h>
#include <stdlib.h>


#define FIO_I2C_MODULE_CHECK(module)     (module > FIO_MODULE_3)?1:0
#define FIO_UART_MODULE_CHECK(module)    (module < FIO_MODULE_4)?1:0

/* Board IO Module values */
#define BIO_AMODULE_0                    EUSCI_A0_BASE
#define BIO_AMODULE_1                    EUSCI_A1_BASE
#define BIO_AMODULE_2                    EUSCI_A2_BASE
#define BIO_AMODULE_3                    EUSCI_A3_BASE
#define BIO_BMODULE_0                    EUSCI_B0_BASE
#define BIO_BMODULE_1                    EUSCI_B1_BASE
#define BIO_BMODULE_2                    EUSCI_B2_BASE
#define BIO_BMODULE_3                    EUSCI_B3_BASE
typedef uint32_t bio_module_t;

/* FIO custom type definitions */
#define FIO_MAX_TRANSFER_LENGTH    65535
#define FIO_NO_TIMEOUT             0


typedef union
{
  const eUSCI_I2C_MasterConfig *i2cConfig;
  const eUSCI_SPI_MasterConfig *spiConfig;
  const eUSCI_UART_Config *     uartConfig;
} fio_config_union;

typedef union
{
  fi2c_t i2cModule;
  //fspi_t;
  //fuart_t;
} fio_peripheral_t;

typedef struct
{
  fio_mode_t       mode;
  fio_config_union peripheralConfig;
} fio_config_t;


typedef struct fio_struct fio_hal_t;

struct fio_struct
{
  bool             isInitialized;           /* Module is initialized flag */
  volatile bool    isTransferring;          /* Currently transferring flag */
  fio_module_t     module;                  /* IO module value */
  fio_mode_t       mode;                    /* Bus type ie. i2c, spi, uart */
  fio_transfer_t * transferData;            /* Transfer Data of current packet */
  fio_peripheral_t peripheralModule;        /* Peripheral module data */
};


fio_hal_t * FIO_Initialize(fio_module_t module, const fio_config_t *config);
void        FIO_Deinitialize(fio_hal_t *instance);
fio_error_t FIO_Transfer(fio_hal_t *instance, fio_transfer_t *transferData);
fio_error_t FIO_IsModuleInit(fio_module_t module);


#endif // FIO_HAL_H
