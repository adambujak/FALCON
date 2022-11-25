/******************************************************************************
* @file     fiom_hal.c
* @brief    Falcon Input/Output Module
* @version  1.0
* @date     2020-04-05
* @author   Adam Bujak
******************************************************************************/

#include "fio_hal.h"
#include "fln_bsp.h"
#include <string.h>


#define INITIAL_FLAG_SET    0x3E
fio_callback_t nullCallback = { NULL, 0 };
static fio_hal_t fioModules[FIO_MODULE_CNT];
static uint8_t initialFlag = INITIAL_FLAG_SET;   // Random value since there was a bug when I set it to 0 TODO: find issue


/**
 * @brief   Returns hardware level base value for IO module
 * @param   module - IO Module Base Value
 * @retval  None
 */
static inline bio_module_t get_board_base_address(fio_module_t module)
{
  switch (module) {
  case FIO_MODULE_0:
    return BIO_AMODULE_0;

  case FIO_MODULE_1:
    return BIO_AMODULE_1;

  case FIO_MODULE_2:
    return BIO_AMODULE_2;

  case FIO_MODULE_3:
    return BIO_AMODULE_3;

  case FIO_MODULE_4:
    return BIO_BMODULE_0;

  case FIO_MODULE_5:
    return BIO_BMODULE_1;

  case FIO_MODULE_6:
    return BIO_BMODULE_2;

  case FIO_MODULE_7:
    return BIO_BMODULE_3;

  default:
    return 0;
  }
}

/**
 * @brief   Enables IRQ for specified IO module
 * @param   module - IO Module Base Value
 * @retval  None
 */
static inline void enable_iom_irq(fio_module_t module)
{
  switch (module) {
  case FIO_MODULE_0:
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
    return;

  case FIO_MODULE_1:
    MAP_Interrupt_enableInterrupt(INT_EUSCIA1);
    return;

  case FIO_MODULE_2:
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
    return;

  case FIO_MODULE_3:
    MAP_Interrupt_enableInterrupt(INT_EUSCIA3);
    return;

  case FIO_MODULE_4:
    MAP_Interrupt_enableInterrupt(INT_EUSCIB0);
    return;

  case FIO_MODULE_5:
    MAP_Interrupt_enableInterrupt(INT_EUSCIB1);
    return;

  case FIO_MODULE_6:
    MAP_Interrupt_enableInterrupt(INT_EUSCIB2);
    return;

  case FIO_MODULE_7:
    MAP_Interrupt_enableInterrupt(INT_EUSCIB3);
    return;

  default:
    return;
  }
}

/**
 * @brief   Disables IRQ for specified IO module
 * @param   module - IO Module Base Value
 * @retval  None
 */
static inline void disable_iom_irq(fio_module_t module)
{
  switch (module) {
  case FIO_MODULE_0:
    MAP_Interrupt_disableInterrupt(INT_EUSCIA0);
    return;

  case FIO_MODULE_1:
    MAP_Interrupt_disableInterrupt(INT_EUSCIA1);
    return;

  case FIO_MODULE_2:
    MAP_Interrupt_disableInterrupt(INT_EUSCIA2);
    return;

  case FIO_MODULE_3:
    MAP_Interrupt_disableInterrupt(INT_EUSCIA3);
    return;

  case FIO_MODULE_4:
    MAP_Interrupt_disableInterrupt(INT_EUSCIB0);
    return;

  case FIO_MODULE_5:
    MAP_Interrupt_disableInterrupt(INT_EUSCIB1);
    return;

  case FIO_MODULE_6:
    MAP_Interrupt_disableInterrupt(INT_EUSCIB2);
    return;

  case FIO_MODULE_7:
    MAP_Interrupt_disableInterrupt(INT_EUSCIB3);
    return;

  default:
    return;
  }
}

/**
 * @brief   Handler for when  transfer is completed
 * @param   context - context passed to peripheral - should be fio_hal instance
 * @param   status  - status of transfer
 * @retval  None
 */
static void transfer_complete_handler(void *context, uint8_t status)
{
  fio_hal_t *instance = (fio_hal_t *) (context);

  disable_iom_irq(instance->module);
  instance->isTransferring = false;
  if (instance->transferData->xferDoneCB.callbackFunction != NULL) {
    instance->transferData->xferDoneCB.callbackFunction(instance->transferData->xferDoneCB.context, status);
  }
}

/**
 * @brief   Deinitializes peripheral modules
 * @param   module - Peripheral module instance pointer
 * @param   mode   - FIO mode
 * @retval  None
 */
static inline void deinit_peripheral_module(fio_peripheral_t *module, fio_mode_t mode)
{
  switch (mode) {
  case FIO_MODE_I2C:
    FI2C_Deinitialize(&(module->i2cModule));
    break;
//      case FIO_MODE_SPI:
//        FSPI_Deinitialize(module);
//        break;
//      case FIO_MODE_UART:
//        FUART_Deinitialize(module);
//        break;
  }
}

/**
 * @brief   Check if module is initialized
 * @param   module - FIO module to check
 * @retval  error  - error status
 */
fio_error_t FIO_IsModuleInit(fio_module_t module)
{
  /* If FIO_Init has never been called, the ioModule structs have not been set to all 0,
   * therefore, we may get false positives.
   *
   * So if the initial flag is still set, the module is not initialized yet.
   */
  if (initialFlag == INITIAL_FLAG_SET) {
    return FIO_ERROR_NONE;
  }
  if (fioModules[module].isInitialized) {
    return FIO_ERROR_GENERAL;
  }
  return FIO_ERROR_NONE;
}

/**
 * @brief   I/O Transfer
 * @param   instance     - FIO instance pointer
 * @param   transferData - transfer data
 * @param   response     - callback response
 * @retval  error        - error status
 */
fio_error_t FIO_Transfer(fio_hal_t *instance, fio_transfer_t *transferData)
{
  if (instance == NULL || !(instance->isInitialized)) {
    return FIO_ERROR_INIT;
  }

  instance->transferData = transferData;
  instance->isTransferring = true;

  fio_callback_t tempcallback = { &transfer_complete_handler, instance };

  enable_iom_irq(instance->module);

  switch (instance->mode) {
  case (FIO_MODE_I2C):
    FI2C_Transfer(&(instance->peripheralModule.i2cModule), transferData, tempcallback);
    return FIO_ERROR_NONE;

  default:
    return FIO_ERROR_GENERAL;
  }
}

/**
 * @brief   FIO Initialization function
 * @param   module   - FIO module value
 * @param   config   - FIO config
 * @retval  instance - FIO HAL instance pointer
 */
fio_hal_t * FIO_Initialize(fio_module_t module, const fio_config_t *config)
{
  /* Only do this the first time FIO_Initialize is called */
  if (initialFlag == INITIAL_FLAG_SET) {
    initialFlag = 1;
    memset(fioModules, 0, sizeof(fioModules));
  }
  fio_hal_t *instance = &(fioModules[module]);

  if (instance->isInitialized) {
    return NULL;
  }

  if ((config->mode == FIO_MODE_I2C) && (FIO_I2C_MODULE_CHECK(module))) {
    /* Initialize io pins in bsp */
    initialize_io_pins(module, FIO_MODE_I2C);

    /* Get base io hardware address from bsp */
    bio_module_t baseAddr = get_board_base_address(module);

    /* Configure I2C Module */
    FI2C_Initialize(&(instance->peripheralModule.i2cModule), baseAddr, config->peripheralConfig.i2cConfig);

    instance->isInitialized = true;
    instance->module = module;
    instance->mode = FIO_MODE_I2C;
    instance->isTransferring = false;
    instance->transferData = NULL;

    return instance;
  }
//  else if ( config->type == FIOM_TYPE_SPI )
//  {
//    // Configure SPI
//  }
//  else if ( (config->type == FIOM_TYPE_I2C) && (module < FIO_MODULE_4) )
//  {
//    // Configure UART
//  }
  else{
    /* Invalid configuration */
    return NULL;
  }
}

/**
 * @brief    FIO Deinitialization function
 * @param  * instance - FIO HAL instance pointer
 * @retval   None
 */
void FIO_Deinitialize(fio_hal_t *instance)
{
  if (!(instance->isInitialized)) {
    return;
  }

  disable_iom_irq(instance->module);
  instance->isInitialized = false;
  instance->isTransferring = false;
  instance->module = FIO_MODULE_CNT;
  deinit_peripheral_module(&instance->peripheralModule, instance->mode);
//  deinit_pins
}

/**
 * @brief    Interrupt handler for io modules
 * @param  * instance - FIO HAL instance pointer
 * @retval   None
 */
static inline void IRQ_Handler(fio_hal_t *instance)
{
  switch (instance->mode) {
  case FIO_MODE_I2C:
    if (FIO_I2C_MODULE_CHECK(instance->module) && instance->isInitialized) {
      FI2C_IRQ_Handler(&(instance->peripheralModule.i2cModule));
    }
    break;
//    case FIO_MODE_SPI:
//          FSPI_IRQ_Handler(&(instance->peripheralModule.spiModule));
//    case FIO_MODE_UART:
//          FSPI_IRQ_Handler(&(instance->peripheralModule.uartModule));
  }
}

void EUSCIA0_IRQHandler(void)
{
  IRQ_Handler(&fioModules[0]);
}

void EUSCIA1_IRQHandler(void)
{
  IRQ_Handler(&fioModules[1]);
}

void EUSCIA2_IRQHandler(void)
{
  IRQ_Handler(&fioModules[2]);
}

void EUSCIA3_IRQHandler(void)
{
  IRQ_Handler(&fioModules[3]);
}

void EUSCIB0_IRQHandler(void)
{
  IRQ_Handler(&fioModules[4]);
}

void EUSCIB1_IRQHandler(void)
{
  IRQ_Handler(&fioModules[5]);
}

void EUSCIB2_IRQHandler(void)
{
  IRQ_Handler(&fioModules[6]);
}

void EUSCIB3_IRQHandler(void)
{
  IRQ_Handler(&fioModules[7]);
}
