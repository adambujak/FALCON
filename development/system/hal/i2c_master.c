/******************************************************************************
 * @file     i2c_master.c
 * @brief    I2C Hardware Abstraction Layer Source File
 * @version  1.0
 * @date     2020-04-01
 * @author   Adam Bujak
 ******************************************************************************/

#include "i2c_master.h"


static fi2c_t fi2cInstances[FIO_MODULE_CNT];



/**
  * @brief   Returns hardware level base value for IO module
  * @param   module - IO Module Base Value
  * @retval  None
  */
bio_module_t get_board_i2c_module ( fio_module_t module )
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
  * @brief   Blocking I2C Write Function 
  * @param * instance  - i2c instance pointer
  * @param   slaveAddr - i2c device address of slave
  * @param * txArray   - transfer array - data will be written from here to slave
  * @param   size      - number of bytes to write 
  * @retval  error     - error code
  */
static fio_retval_e blocking_write ( fi2c_t * instance, fi2c_addr_t slaveAddr, fi2c_reg_t writeReg, fi2c_reg_t * txArray, uint32_t size )
{
  MAP_I2C_setSlaveAddress(instance->module, slaveAddr);
  
  MAP_I2C_masterSendMultiByteStart(instance->module, writeReg);

  for ( uint32_t xferIndex = 0; xferIndex < size; xferIndex++ )
  {
    MAP_I2C_masterSendMultiByteNext(instance->module, txArray[xferIndex]);
  } 

  MAP_I2C_masterSendMultiByteStop(instance->module);

  return FIO_RETVAL_SUCCESS;  // ToDo add NAK detection
}

/**
  * @brief   Blocking I2C Read Function 
  * @param * instance  - i2c instance pointer
  * @param   slaveAddr - i2c device address of slave
  * @param   readReg   - address to read from
  * @param * rxArray   - transfer array - read data will be copied into here
  * @param   size      - number of bytes to read 
  * @retval  error     - error code
  */
static fio_retval_e blocking_read ( fi2c_t * instance, fi2c_addr_t slaveAddr, fi2c_reg_t readReg, fi2c_reg_t * rxArray, uint32_t size )
{
  if ( instance->isTransferring )
  {
    return FIO_RETVAL_ERROR;
  }

  instance->currentTransferData.size = size;
  instance->currentTransferData.xferIndex = 0;
  instance->currentTransferData.slaveAddr = slaveAddr;
  instance->currentTransferData.packet.type = FI2C_TRANSFER_TYPE_READ;
  instance->currentTransferData.packet.packetUnion.readPacket.readReg = readReg;
  instance->currentTransferData.packet.packetUnion.readPacket.rxArray = rxArray;


  MAP_I2C_setSlaveAddress(instance->module, slaveAddr);
  
  MAP_I2C_masterSendMultiByteStart(instance->module, readReg);

//  uint32_t xferIndex = 0;
  MAP_I2C_masterReceiveStart(instance->module);
//
//  for ( uint32_t xferIndex = 0; xferIndex < size - 1; xferIndex++ )
//  {
//    rxArray[xferIndex] = MAP_I2C_masterReceiveMultiByteNext(instance->module);
//  }
//
//  MAP_I2C_masterReceiveMultiByteStop(instance->module);
  //rxArray[size-1] = MAP_I2C_masterReceiveMultiByteNext(instance->module);

  return FIO_RETVAL_SUCCESS; // ToDo add NAK detection
}

/**
  * @brief   Deinitialize I2C module  
  * @param * instance  - i2c instance pointer
  * @retval  error     - error code
  */
static fio_retval_e deinitialize ( fi2c_t * instance )
{
  if ( !(instance->isInitialized) )
  {
    return FIO_RETVAL_ERROR;
  }
  // Deinit pins
  // Deinit i2c module
  instance->isInitialized = false;
  MAP_I2C_disableModule(instance->module);
  return FIO_RETVAL_SUCCESS;
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


  bio_module_t bModule = get_board_i2c_module(module);
  initialize_i2c_pins(module);
  
  MAP_I2C_initMaster(bModule, config);

  MAP_I2C_enableModule(bModule);

//  /* Enable and clear the interrupt flag */
//  MAP_I2C_clearInterruptFlag(bModule,
//          EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_NAK_INTERRUPT);
//
//  /* Enable master transmit interrupt */
//  MAP_I2C_enableInterrupt(bModule,
//          EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_NAK_INTERRUPT);
//  MAP_Interrupt_enableInterrupt(INT_EUSCIB0);

  instance->module         =  bModule;
  instance->isInitialized  =  true;
  instance->blockingWrite  = &blocking_write;
  instance->blockingRead   = &blocking_read;
  instance->isTransferring = false;
  instance->deinitialize   = &deinitialize;

  return instance;
}

static void i2c_interrupt_handler ( fi2c_t * instance )
{
//  uint_fast16_t status;
//
//  status = MAP_I2C_getEnabledInterruptStatus(EUSCI_B0_BASE);
//  MAP_I2C_clearInterruptFlag(EUSCI_B0_BASE, status);
//
//  if (status & EUSCI_B_I2C_NAK_INTERRUPT)
//  {
//      MAP_I2C_masterSendStart(EUSCI_B0_BASE);
//  }
//
//  if (status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0)
//  {
//      /* Check the byte counter */
//      if (TXByteCtr)
//      {
//          /* Send the next data and decrement the byte counter */
//          MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, TXData++);
//          TXByteCtr--;
//      } else
//      {
//          MAP_I2C_masterSendMultiByteStop(EUSCI_B0_BASE);
//          MAP_Interrupt_disableSleepOnIsrExit();
//      }
//  }





  uint_fast16_t status;

  status = MAP_I2C_getEnabledInterruptStatus(instance->module);
  MAP_I2C_clearInterruptFlag(instance->module, status);

  uint32_t size = instance->currentTransferData.size;
  uint32_t xferIndex = instance->currentTransferData.xferIndex;
  fi2c_reg_t * rxArray = instance->currentTransferData.packet.packetUnion.readPacket.rxArray = rxArray;

  /* Receives bytes into the receive buffer. If we have received all bytes,
   * send a STOP condition */
  if ( status & EUSCI_B_I2C_RECEIVE_INTERRUPT0 )
  {
    if ( xferIndex ==  size - 2 )
    {
      MAP_I2C_disableInterrupt(instance->module, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
      MAP_I2C_enableInterrupt(instance->module, EUSCI_B_I2C_STOP_INTERRUPT);

      /*
       * Switch order so that stop is being set during reception of last
       * byte read byte so that next byte can be read.
       */

      MAP_I2C_masterReceiveMultiByteStop(instance->module);
      rxArray[xferIndex++] = MAP_I2C_masterReceiveMultiByteNext(instance->module);
    } else
    {
      rxArray[xferIndex++] = MAP_I2C_masterReceiveMultiByteNext(instance->module);
    }
  } else if ( status & EUSCI_B_I2C_STOP_INTERRUPT )
  {
    MAP_Interrupt_disableSleepOnIsrExit();
    MAP_I2C_disableInterrupt(instance->module, EUSCI_B_I2C_STOP_INTERRUPT);
  }
}

void EUSCIB0_IRQHandler(void)
{
  i2c_interrupt_handler(&fi2cInstances[0]);
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


