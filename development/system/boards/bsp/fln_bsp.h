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

#include "fio_hal.h"

extern const eUSCI_I2C_MasterConfig fi2c_default_config;

extern const fio_config_t fio_default_i2c_config;

/**
  * @brief   Deinitialize IO module GPIO pins
  * @param   module - IO Module Base Value
  * @param   mode   - IO Mode - I2C, SPI, etc
  * @retval  None
  */
void deinitialize_io_pins ( fio_module_t module, fio_mode_t mode );

/**
  * @brief   Setup IO module GPIO pins
  * @param   module - IO Module Base Value
  * @param   mode   - IO Mode - I2C, SPI, etc
  * @retval  None
  */
void initialize_io_pins ( fio_module_t module, fio_mode_t mode );



#endif // FALCON_BSP_H
