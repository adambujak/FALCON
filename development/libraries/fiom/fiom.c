/******************************************************************************
 * @file     fiom.c 
 * @brief    Falcon Input/Output Module 
 * @version  1.0
 * @date     2020-04-05
 * @author   Adam Bujak
 ******************************************************************************/

#include "fiom.h"


/**
  * @brief   Non blocking io transfer
  * @param * instance     - FIOM instance pointer
  * @param   transferData - transfer data
  * @retval  error    - error status
  */
static fio_error_t non_blocking_transfer ( fiom_t * instance, fio_transfer_t * transferData )
{
  return FIO_Transfer(instance->halInstance, transferData);
}

/**
  * @brief   Blocking io transfer
  * @param * instance     - FIOM instance pointer
  * @param   transferData - transfer data
  * @retval  error    - error status
  */
static fio_error_t blocking_transfer ( fiom_t * instance, fio_transfer_t * transferData )
{
  fio_error_t ret = FIO_Transfer(instance->halInstance, transferData);

  /* If error exists, return error code, else wait for transfer to complete */
  if ( ret )
  {
    return ret;
  }
  while (instance->halInstance->isTransferring);
  return ret;
}

/**
  * @brief   FIOM Initialization function
  * @param * instance - FIOM instance pointer
  * @param   config   - FIO config
  * @retval  error    - error status
  */
fio_error_t FIOM_Initialize   ( fiom_t * instance, fio_module_t module, const fio_config_t * config )
{
  if ( FIO_IsModuleInit(module) )
  {
    return FIO_ERROR_MISC;
  }

  fio_hal_t * hal = FIO_Initialize(module, config);
  if ( hal == NULL )
  {
    return FIO_ERROR_MISC;
  }

  instance->halInstance = hal;

  instance->nonBlockingTransfer = &non_blocking_transfer;
  instance->blockingTransfer    = &blocking_transfer;

  return FIO_ERROR_NONE;
}

/**
  * @brief   FIOM Deinitialization function
  * @param * instance - FIOM instance pointer
  * @retval  None
  */
void FIOM_Deinitialize ( fiom_t * instance )
{
  FIO_Deinitialize(instance->halInstance);
  instance->halInstance = NULL;
}
