/******************************************************************************
 * @file     fi2c_hal.c
 * @brief    Falcon I2C Hardware Abstraction Layer Source File
 * @version  1.0
 * @date     2020-04-01
 * @author   Adam Bujak
 ******************************************************************************/


#include "fi2c_hal.h"

//
//static inline bool i2c_sendMultiByteStart ( bio_module_t baseAddr, fio_reg_t byte, uint32_t timeout )
//{
//  if ( timeout > 0 )
//  {
//    return MAP_I2C_masterSendMultiByteStartWithTimeout(baseAddr, byte, timeout);
//  }
//  else
//  {
//    MAP_I2C_masterSendMultiByteStart(baseAddr, byte);
//    return true;
//  }
//}
//
//static inline bool i2c_sendMultiByteNext ( bio_module_t baseAddr, fio_reg_t byte, uint32_t timeout )
//{
//  if ( timeout > 0 )
//  {
//    return MAP_I2C_masterSendMultiByteNextWithTimeout(baseAddr, byte, timeout);
//  }
//  else
//  {
//    MAP_I2C_masterSendMultiByteNext(baseAddr, byte);
//    return true;
//  }
//}
//
//static inline bool i2c_sendMultiByteStop ( bio_module_t baseAddr, uint32_t timeout )
//{
//  if ( timeout > 0 )
//  {
//    return MAP_I2C_masterSendMultiByteStopWithTimeout(baseAddr, timeout);
//  }
//  else
//  {
//    MAP_I2C_masterSendMultiByteStop(baseAddr);
//    return true;
//  }
//}

/**
  * @brief   Non-Blocking I2C Write Function
  * @param * instance   - i2c instance pointer
  * @param * slaveAddr  - i2c slave address
  * @param * reg        - register to write to
  * @param * rxBuf      - location to read data from
  * @param * length     - i2c transfer length
  * @retval  None
  */
static inline void i2c_write ( fi2c_t * instance )
{
  MAP_I2C_masterSendMultiByteStart(instance->baseAddr, instance->transferData->writeReg );

  MAP_I2C_enableInterrupt(instance->baseAddr, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
  MAP_I2C_enableInterrupt(instance->baseAddr, EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT);
}


/**
  * @brief   Non-Blocking I2C Read Function
  * @param * instance   - i2c instance pointer
  * @param * slaveAddr  - i2c slave address
  * @param * reg        - register to read
  * @param * rxBuf      - location to write data
  * @param * length     - i2c transfer length
  * @retval  None
  */
static inline void i2c_read ( fi2c_t * instance )
{
  MAP_I2C_masterSendMultiByteStart(instance->baseAddr, instance->transferData->readReg );

  MAP_I2C_enableInterrupt(instance->baseAddr, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
  MAP_I2C_enableInterrupt(instance->baseAddr, EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT);
}

/**
  * @brief   Non-Blocking I2C Transfer Function
  * @param * instance     - i2c instance pointer
  * @param * transferData - pointer to transfer data
  * @param * response     - callback function pointer / context
  * @retval  None
  */
void FI2C_Transfer ( fi2c_t * instance, fio_transfer_t * transferData, fio_callback_t callback )
{
  instance->xferIndex = 0;
  instance->transferData = transferData;
  instance->callback = callback;

  MAP_I2C_setSlaveAddress(instance->baseAddr, transferData->peerInfo.i2cSlaveAddr);

  if ( transferData->direction == FIO_TRANSFER_DIR_READ )
  {
    i2c_read(instance);
  }
  else if ( transferData->direction == FIO_TRANSFER_DIR_WRITE )
  {
    i2c_write(instance);
  }
}

/**
  * @brief   Deinitialize I2C module  
  * @param * instance  - i2c instance pointer
  * @retval  None
  */
void FI2C_Deinitialize ( fi2c_t * instance )
{
  /* Disable I2C Module */
  MAP_I2C_disableModule(instance->baseAddr);
}


/**
  * @brief   FI2C_Initialization Fuction 
  * @param   module    - i2c module base value
  * @param * config    - i2c configuration
  * @retval  instance pointer
  */
void FI2C_Initialize ( fi2c_t * instance, uint32_t baseAddr, const eUSCI_I2C_MasterConfig * config )
{
  instance->baseAddr = baseAddr;

  MAP_I2C_initMaster(baseAddr, config);

  MAP_I2C_enableModule(baseAddr);
}

void FI2C_IRQ_Handler ( fi2c_t * instance )
{
  uint_fast16_t status;
  status = MAP_I2C_getEnabledInterruptStatus(instance->baseAddr);
  MAP_I2C_clearInterruptFlag(instance->baseAddr, status);

  if ( instance->transferData->direction == FIO_TRANSFER_DIR_WRITE )
  {

  }
  else if ( instance->transferData->direction == FIO_TRANSFER_DIR_READ )
  {
    uint16_t   length = instance->transferData->length;
    uint16_t * xferIndex = &(instance->xferIndex);
    uint8_t  * rxArray = instance->transferData->rxBuf;

    /* If transmit interrupt 0 received, send another start to initiate receive */
    if ( status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0 )
    {
      MAP_I2C_disableInterrupt(instance->baseAddr, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
      MAP_I2C_enableInterrupt(instance->baseAddr, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
      MAP_I2C_masterReceiveStart(instance->baseAddr);
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

      if (instance->transferData->xferDoneCB.callbackFunction != NULL)
      {
        instance->transferData->xferDoneCB.callbackFunction(instance->transferData->xferDoneCB.context, FIO_ERROR_NONE);
      }
    }

    else if ( status & EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT )
    {

    }
  }
}




