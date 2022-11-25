/******************************************************************************
* @file     fi2c_hal.h
* @brief    Falcon I2C Hardware Abstraction Layer Header File
* @version  1.0
* @date     2020-04-01
* @author   Adam Bujak
******************************************************************************/

#ifndef FI2C_HAL_H
#define FI2C_HAL_H

#include "fio_common.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

typedef struct fi2c_struct fi2c_t;


struct fi2c_struct {
  uint32_t        baseAddr;                /* Hardware I2C module base address */
  fio_transfer_t *transferData;            /* Transfer data */
  fio_callback_t  callback;                /* Callback and context for when transfer is complete */
  uint16_t        xferIndex;               /* Current index of current transfer */
};

void FI2C_Initialize(fi2c_t *instance, uint32_t baseAddr, const eUSCI_I2C_MasterConfig *config);
void FI2C_Deinitialize(fi2c_t *instance);
void FI2C_Transfer(fi2c_t *instance, fio_transfer_t *transferData, fio_callback_t callback);
void FI2C_IRQ_Handler(fi2c_t *instance);

#endif // FI2C_HAL_H
