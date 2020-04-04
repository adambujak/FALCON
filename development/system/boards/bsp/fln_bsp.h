/******************************************************************************
 * @file     fln_bsp.h
 * @brief    Falcon Board Support Package
 * @version  1.0
 * @date     2020-04-02
 * @author   Adam Bujak
 ******************************************************************************/

#ifndef FALCON_BSP_H
#define FALCON_BSP_H

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "fiodt.h"

extern const eUSCI_I2C_MasterConfig fi2c_default_config;

/**
  * @brief   Deinitialize I2C module GPIO pins
  * @param   module - I2C Module Base Value
  * @retval  None
  */
void deinitialize_i2c_pins ( fio_module_t module );
/**
  * @brief   Setup I2C module GPIO pins
  * @param   module - I2C Module Base Value
  * @retval  None
  */
void initialize_i2c_pins ( fio_module_t module );



#endif // FALCON_BSP_H
