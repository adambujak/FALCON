/******************************************************************************
 * @file     i2c_master.c
 * @brief    I2C Hardware Abstraction Layer Source File
 * @version  1.0
 * @date     2020-04-01
 * @author   Adam Bujak
 ******************************************************************************/

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "i2c_master.h"


static fi2c_t fi2cInstances[FIO_MODULE_CNT];


static bool i2c_sendMultiByteStart ( bio_module_t baseAddr, fi2c_reg_t byte, uint32_t timeout )
{
  if ( timeout > 0 )
  {
    return MAP_I2C_masterSendMultiByteStartWithTimeout(baseAddr, byte, timeout);
  }
  else
  {
    MAP_I2C_masterSendMultiByteStart(baseAddr, byte);
    return true;
  }
}

static bool i2c_sendMultiByteNext ( bio_module_t baseAddr, fi2c_reg_t byte, uint32_t timeout )
{
  if ( timeout > 0 )
  {
    return MAP_I2C_masterSendMultiByteNextWithTimeout(baseAddr, byte, timeout);
  }
  else
  {
    MAP_I2C_masterSendMultiByteNext(baseAddr, byte);
    return true;
  }
}

static bool i2c_sendMultiByteStop ( bio_module_t baseAddr, uint32_t timeout )
{
  if ( timeout > 0 )
  {
    return MAP_I2C_masterSendMultiByteStopWithTimeout(baseAddr, timeout);
  }
  else
  {
    MAP_I2C_masterSendMultiByteStop(baseAddr);
    return true;
  }
}


/**
  * @brief   Returns hardware level base value for IO module
  * @param   module - IO Module Base Value
  * @retval  None
  */
static bio_module_t get_board_i2c_module ( fio_module_t module )
{
  switch ( module )
  {
    case FIO_MODULE_0:
      return BIO_MODULE_0;
    case FIO_MODULE_1:
      return BIO_MODULE_1;
    case FIO_MODULE_2:
      return BIO_MODULE_2;
    case FIO_MODULE_3:
      return BIO_MODULE_3;
    default:
      return 0;
  }
}

/**
  * @brief   Enables IRQ for specified I2C module
  * @param   module - IO Module Base Value
  * @retval  None
  */
static void enable_i2c_irq ( fio_module_t module )
{
  switch ( module )
  {
    case FIO_MODULE_0:
      MAP_Interrupt_enableInterrupt(INT_EUSCIB0);
      return;
    case FIO_MODULE_1:
      MAP_Interrupt_enableInterrupt(INT_EUSCIB1);
      return;
    case FIO_MODULE_2:
      MAP_Interrupt_enableInterrupt(INT_EUSCIB2);
      return;
    case FIO_MODULE_3:
      MAP_Interrupt_enableInterrupt(INT_EUSCIB3);
      return;
    default:
      return;
  }
}

/**
  * @brief   Disables IRQ for specified I2C module
  * @param   module - IO Module Base Value
  * @retval  None
  */
static void disable_i2c_irq ( fio_module_t module )
{
  switch ( module )
  {
    case FIO_MODULE_0:
      MAP_Interrupt_disableInterrupt(INT_EUSCIB0);
      return;
    case FIO_MODULE_1:
      MAP_Interrupt_disableInterrupt(INT_EUSCIB1);
      return;
    case FIO_MODULE_2:
      MAP_Interrupt_disableInterrupt(INT_EUSCIB2);
      return;
    case FIO_MODULE_3:
      MAP_Interrupt_disableInterrupt(INT_EUSCIB3);
      return;
    default:
      return;
  }
}


/**
  * @brief   Blocking I2C Write Function
  * @param * instance     - i2c instance pointer
  * @param * transferData - i2c transfer data
  * @retval  error        - error code
  */
static fio_retval_e blocking_write ( fi2c_t * instance, fi2c_transfer_t * transferData )
{
  instance->transferData = transferData;
  instance->transferData->direction = FI2C_TRANSFER_DIR_WRITE;
  uint32_t timeout = transferData->timeout;

  MAP_I2C_setSlaveAddress(instance->baseAddr, transferData->slaveAddr);

  if (i2c_sendMultiByteStart(instance->baseAddr, transferData->writeReg, timeout) == false )
  {
    return FIO_RETVAL_ERROR;
  }

  for ( uint32_t xferIndex = 0; xferIndex < transferData->length; xferIndex++ )
  {
    if ( i2c_sendMultiByteNext(instance->baseAddr, transferData->txBuf[xferIndex], timeout) == false )
    {
      return FIO_RETVAL_ERROR;
    }
  }

  if ( i2c_sendMultiByteStop(instance->baseAddr, timeout) )
  {
    return FIO_RETVAL_ERROR;
  }

  return FIO_RETVAL_SUCCESS;  // ToDo add NAK detection
}

/**
  * @brief   Non-Blocking I2C Read Function
  * @param * instance     - i2c instance pointer
  * @param * transferData - i2c transfer data
  * @retval  error        - error code
  */
static fio_retval_e non_blocking_read ( fi2c_t * instance, fi2c_transfer_t * transferData )
{
  if ( instance->isTransferring )
  {
    return FIO_RETVAL_ERROR;
  }

  instance->transferData = transferData;
  instance->isTransferring = true;
  instance->xferIndex = 0;
  instance->transferData->direction = FI2C_TRANSFER_DIR_READ;

  enable_i2c_irq(instance->module);

  MAP_I2C_setSlaveAddress(instance->baseAddr, instance->transferData->slaveAddr);


  MAP_I2C_masterSendMultiByteStart(instance->baseAddr, instance->transferData->readReg);
  MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
  MAP_I2C_enableInterrupt(instance->baseAddr, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
  MAP_I2C_enableInterrupt(instance->baseAddr, EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT);

  return FIO_RETVAL_SUCCESS; // ToDo add NAK detection
}


/**
  * @brief   Blocking I2C Read Function
  * @param * instance     - i2c instance pointer
  * @param * transferData - i2c transfer data
  * @retval  error        - error code
  */

static fio_retval_e blocking_read ( fi2c_t * instance, fi2c_transfer_t * transferData )
{
  if ( instance->isTransferring )
  {
    return FIO_RETVAL_ERROR;
  }
//  MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
  bool ret = non_blocking_read(instance, transferData);

  uint32_t timeout = 0;
  while ( instance->isTransferring )
  {
    timeout++;
    if ( transferData->timeout != 0 && timeout >= transferData->timeout )
    {
      instance->isTransferring = false;
      return FIO_RETVAL_ERROR;
    }
  }
  return ret; // ToDo add NAK detection
}


/**
  * @brief   Deinitialize I2C module  
  * @param * instance  - i2c instance pointer
  * @retval  error     - error code
  */
fi2c_t * FI2C_Deinitialize ( fi2c_t * instance )
{
  if ( !(instance->isInitialized) )
  {
    return NULL;
  }

  /* Clear init flag */
  instance->isInitialized = false;

  /* Disable I2C Module */
  MAP_I2C_disableModule(instance->baseAddr);

  /* Disable I2C IRQ */
  disable_i2c_irq(instance->module);
  return NULL;
}


/**
  * @brief   FI2C_Initialization Fuction 
  * @param   module    - i2c module base value
  * @param * config    - i2c configuration
  * @retval  instance pointer
  */
fi2c_t * FI2C_Initialize ( fio_module_t module, const eUSCI_I2C_MasterConfig * config )
{
  fi2c_t * instance = &fi2cInstances[module];

  /* Initialize i2c pins in bsp */
  initialize_i2c_pins(module);
  
  /* Get base i2c hardware address from bsp */
  bio_module_t bModule = get_board_i2c_module(module);

  MAP_I2C_initMaster(bModule, config);

  MAP_I2C_enableModule(bModule);


  instance->module         =  module;
  instance->baseAddr       =  bModule;
  instance->isInitialized  =  true;
  instance->isTransferring =  false;
  instance->blockingWrite  = &blocking_write;
  instance->blockingRead   = &blocking_read;
//  instance->blockingWrite  = &non_blocking_write;
//  instance->blockingRead   = &non_blocking_read;

  return instance;
}

static void i2c_interrupt_handler ( fi2c_t * instance )
{
//  uint_fast16_t status;
//
//  status = MAP_I2C_getEnabledInterruptStatus(instance->baseAddr);
//  MAP_I2C_clearInterruptFlag(instance->baseAddr, status);
//
//  if ( instance->transferData->direction == FI2C_TRANSFER_DIR_WRITE )
//  {
//
//  }
//  else if ( instance->transferData->direction == FI2C_TRANSFER_DIR_READ )
//  {
//    fi2c_len_t   length = instance->transferData->length;
//    fi2c_len_t * xferIndex = &(instance->xferIndex);
//    fi2c_reg_t * rxArray = instance->transferData->rxBuf;
//
//    /* If transmit interrupt 0 received, send another start to initiate receive */
//    if ( status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0 )
//    {
//      MAP_I2C_disableInterrupt(instance->baseAddr, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
//      MAP_I2C_enableInterrupt(instance->baseAddr, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
//      MAP_I2C_masterReceiveStart(instance->baseAddr);
//    }
//    /* Receives bytes into the receive buffer. If we have received all bytes,
//     * send a STOP condition */
//    if ( status & EUSCI_B_I2C_RECEIVE_INTERRUPT0 )
//    {
//      if ( (*xferIndex) ==  length - 2 )
//      {
//        MAP_I2C_disableInterrupt(instance->baseAddr, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
//        MAP_I2C_enableInterrupt(instance->baseAddr, EUSCI_B_I2C_STOP_INTERRUPT);
//
//        /*
//         * Switch order so that stop is being set during reception of last
//         * byte read byte so that next byte can be read.
//         */
//        MAP_I2C_masterReceiveMultiByteStop(instance->baseAddr);
//        rxArray[(*xferIndex)++] = MAP_I2C_masterReceiveMultiByteNext(instance->baseAddr);
//
//      }
//      else
//      {
//        rxArray[(*xferIndex)++] = MAP_I2C_masterReceiveMultiByteNext(instance->baseAddr);
//      }
//    }
//    else if ( status & EUSCI_B_I2C_STOP_INTERRUPT )
//    {
//      rxArray[(*xferIndex)++] = MAP_I2C_masterReceiveMultiByteNext(instance->baseAddr);
//      MAP_I2C_disableInterrupt(instance->baseAddr, EUSCI_B_I2C_STOP_INTERRUPT);
//      instance->isTransferring = false;
//      disable_i2c_irq(instance->module);
//    }
//    else if ( status & EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT )
//    {
//
//    }
//  }
}

void EUSCIB0_IRQHandler(void)
{
//  i2c_interrupt_handler(&fi2cInstances[0]);
  uint_fast16_t status;

#define instance (&fi2cInstances[0])
  status = MAP_I2C_getEnabledInterruptStatus(instance->baseAddr);
  MAP_I2C_clearInterruptFlag(instance->baseAddr, status);

  if ( instance->transferData->direction == FI2C_TRANSFER_DIR_WRITE )
  {

  }
  else if ( instance->transferData->direction == FI2C_TRANSFER_DIR_READ )
  {
    fi2c_len_t   length = instance->transferData->length;
    fi2c_len_t * xferIndex = &(instance->xferIndex);
    fi2c_reg_t * rxArray = instance->transferData->rxBuf;

    /* If transmit interrupt 0 received, send another start to initiate receive */
    if ( status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0 )
    {
      MAP_I2C_disableInterrupt(instance->baseAddr, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
      MAP_I2C_enableInterrupt(instance->baseAddr, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
      MAP_I2C_masterReceiveStart(instance->baseAddr);
      MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
    }
    /* Receives bytes into the receive buffer. If we have received all bytes,
     * send a STOP condition */
    if ( status & EUSCI_B_I2C_RECEIVE_INTERRUPT0 )
    {
      if ( (*xferIndex) ==  length - 2 )
      {
        MAP_I2C_disableInterrupt(instance->baseAddr, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
        MAP_I2C_enableInterrupt(instance->baseAddr, EUSCI_B_I2C_STOP_INTERRUPT);

        /*
         * Switch order so that stop is being set during reception of last
         * byte read byte so that next byte can be read.
         */
        MAP_I2C_masterReceiveMultiByteStop(instance->baseAddr);
        rxArray[(*xferIndex)++] = MAP_I2C_masterReceiveMultiByteNext(instance->baseAddr);

      }
      else
      {
        rxArray[(*xferIndex)++] = MAP_I2C_masterReceiveMultiByteNext(instance->baseAddr);
      }
    }
    else if ( status & EUSCI_B_I2C_STOP_INTERRUPT )
    {
      rxArray[(*xferIndex)++] = MAP_I2C_masterReceiveMultiByteNext(instance->baseAddr);
      MAP_I2C_disableInterrupt(instance->baseAddr, EUSCI_B_I2C_STOP_INTERRUPT);
      instance->isTransferring = false;
      disable_i2c_irq(instance->module);
    }
    else if ( status & EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT )
    {

    }
  }
#undef instance
}
void EUSCIB1_IRQHandler(void)
{
  i2c_interrupt_handler(&fi2cInstances[1]);
}
void EUSCIB2_IRQHandler(void)
{
  i2c_interrupt_handler(&fi2cInstances[2]);
}
void EUSCIB3_IRQHandler(void)
{
  i2c_interrupt_handler(&fi2cInstances[3]);
}


