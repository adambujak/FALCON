/******************************************************************************
 * @file     fln_bsp.c
 * @brief    Falcon Board Support Package
 * @version  1.0
 * @date     2020-04-02
 * @author   Adam Bujak
 ******************************************************************************/

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include "fln_bsp.h"


 const eUSCI_I2C_MasterConfig fi2c_default_config =
{
  EUSCI_B_I2C_CLOCKSOURCE_SMCLK,
  3000000,
  EUSCI_B_I2C_SET_DATA_RATE_100KBPS,
  0,
  EUSCI_B_I2C_NO_AUTO_STOP
};


void deinitialize_i2c_pins ( fio_module_t module )
{

}

void initialize_i2c_pins ( fio_module_t module )
{
  switch ( module )
  {
    case FIO_MODULE_0:
      MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                                     GPIO_PIN6 + GPIO_PIN7,
                                                     GPIO_PRIMARY_MODULE_FUNCTION);
      break;
    case FIO_MODULE_1:
      MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                                     GPIO_PIN4 + GPIO_PIN5,
                                                     GPIO_PRIMARY_MODULE_FUNCTION);
    break;
    case FIO_MODULE_2:
      MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
                                                     GPIO_PIN6 + GPIO_PIN7,
                                                     GPIO_PRIMARY_MODULE_FUNCTION);
    break;
    case FIO_MODULE_3:
      MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                                     GPIO_PIN6 + GPIO_PIN7,
                                                     GPIO_PRIMARY_MODULE_FUNCTION);
    break;
  }
}
