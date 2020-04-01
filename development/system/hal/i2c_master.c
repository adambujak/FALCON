/******************************************************************************
 * @file     i2c_master.c
 * @brief    I2C Hardware Abstraction Layer Source File
 * @version  1.0
 * @date     2020-04-01
 * @author   Adam Bujak
 ******************************************************************************/

#include "i2c_master.h"

/**
  * @brief   Blocking I2C Write Function 
  * @param * instance  - i2c instance pointer
  * @param   slaveAddr - i2c device address of slave
  * @param * txArray   - transfer array - data will be written from here to slave
  * @param   size      - number of bytes to write 
  * @retval  error     - error code
  */

static fi2c_retval_e blockingWrite ( fi2c_t * instance, fi2c_addr_t slaveAddr, fi2c_reg_t * txArray, uint32_t size )
{
  MAP_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);
}

/**
  * @brief   Blocking I2C Read Function 
  * @param * instance  - i2c instance pointer
  * @param   slaveAddr - i2c device address of slave
  * @param * rxArray   - transfer array - read data will be copied into here
  * @param   size      - number of bytes to read 
  * @retval  error     - error code
  */

static fi2c_retval_e blockingRead ( fi2c_t * instance, fi2c_addr_t slaveAddr, fi2c_reg_t * rxArray, uint32_t size )
{
  MAP_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);
}

/**
  * @brief   Deinitialize I2C module  
  * @param * instance  - i2c instance pointer
  * @retval  error     - error code
  */
static fi2c_retval_e deinitialize ( fi2c_t * instance )
{
  // Deinit pins
  // Deinit i2c module
}


/**
  * @brief   Setup I2C module GPIO pins
  * @param   module - I2C Module Base Value 
  * @retval  None
  */
static void initialize_i2c_gpios ( fi2c_module_t module )
{
  switch (module) 
  {
    case FI2C_MODULE_0:
      MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                                     GPIO_PIN6 + GPIO_PIN7, 
                                                     GPIO_PRIMARY_MODULE_FUNCTION);
      break;
    case FI2C_MODULE_1:
      MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                                     GPIO_PIN4 + GPIO_PIN5, 
                                                     GPIO_PRIMARY_MODULE_FUNCTION);
    break;
    case FI2C_MODULE_2:
      MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
                                                     GPIO_PIN6 + GPIO_PIN7, 
                                                     GPIO_PRIMARY_MODULE_FUNCTION);
    break;
    case FI2C_MODULE_3:
      MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                                     GPIO_PIN6 + GPIO_PIN7, 
                                                     GPIO_PRIMARY_MODULE_FUNCTION);
    break;
  }
}

fi2c_retval_e  FI2C_Initialize ( fi2c_t * instance, fi2c_module_t module, const eUSCI_I2C_MasterConfig * config )
{
  initialize_i2c_gpios(module);
  
  MAP_I2C_initMaster(module, config);

  instance->module        =  module;
  instance->blockingWrite = &blockingWrite;
  instance->blockingRead  = &blockingRead;
  instance->deinitialize  = &deinitialize;

}