#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include <stdint.h>
#include <stdbool.h>
#include "fiom.h"
#include "fln_bsp.h"

#define SYS_CLOCK    300000
#define SYS_MS_TO_TICK(ms)  ( ms*SYS_CLOCK/1000 )

#define SLAVE_ADDR 0x52


void setpin ( uint_fast8_t port, uint_fast16_t pin, uint8_t state )
{
  if ( state == 0 )
  {
    MAP_GPIO_setOutputLowOnPin(port, pin);
  }
  else
  {
    MAP_GPIO_setOutputHighOnPin(port, pin);
  }
}

fiom_t fioModuleInstance;
fiom_t * fiom = &fioModuleInstance;
fio_transfer_t transferData;

fio_reg_t txData[] = {0xFF, 0x00, 0x00};
fio_reg_t rxData[] = {0xFF, 0xFF, 0xFF, 0xFF};

void transferDone ( void * context, uint8_t status )
{
  if (context != NULL)
  {

  }
}

fio_error_t i2c_write ( fio_addr_t slave, fio_reg_t reg, fio_reg_t *txData, fio_len_t length )
{
  transferData.writeReg  = reg;
  transferData.txBuf     = txData;
  transferData.length    = length;
  transferData.direction = FIO_TRANSFER_DIR_WRITE;
  transferData.timeout   = SYS_MS_TO_TICK(25);
  transferData.peerInfo.i2cSlaveAddr = slave;
  transferData.xferDoneCB.callbackFunction = &transferDone;
  transferData.xferDoneCB.context          = fiom;

  return fiom->nonBlockingTransfer(fiom, &transferData);
}
fio_error_t i2c_read ( fio_addr_t slave, fio_reg_t reg, fio_reg_t *rxData, fio_len_t length )
{
  transferData.readReg   = reg;
  transferData.rxBuf     = rxData;
  transferData.length    = length;
  transferData.direction = FIO_TRANSFER_DIR_READ;
  transferData.timeout   = SYS_MS_TO_TICK(25);
  transferData.peerInfo.i2cSlaveAddr = slave;

  transferData.xferDoneCB.callbackFunction = &transferDone;
  transferData.xferDoneCB.context          = fiom;

  return fiom->nonBlockingTransfer(fiom, &transferData);
}
fio_error_t i2c_readb ( fio_addr_t slave, fio_reg_t reg, fio_reg_t *rxData, fio_len_t length )
{
  transferData.readReg   = reg;
  transferData.rxBuf     = rxData;
  transferData.length    = length;
  transferData.direction = FIO_TRANSFER_DIR_READ;
  transferData.timeout   = SYS_MS_TO_TICK(25);
  transferData.peerInfo.i2cSlaveAddr = slave;

  transferData.xferDoneCB.callbackFunction = &transferDone;
  transferData.xferDoneCB.context          = fiom;

  return fiom->blockingTransfer(fiom, &transferData);
}

int main ( void )
{
  volatile uint32_t ii;

  /* Halting the Watchdog */
  MAP_WDT_A_holdTimer();

  uint32_t a = SYS_MS_TO_TICK(25);

  /* Configuring RGB LED pins as output */
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7);

  MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);


  fio_error_t error;

  error = FIOM_Initialize(fiom, FIO_MODULE_4, &fio_default_i2c_config);

  if ( error )
  {
    while(1);
  }


//  error = i2c_read(SLAVE_ADDR, 0x32, rxData, 2);
  error = i2c_write(SLAVE_ADDR, 0x60, txData, 4);
  while(1);

//
//
////  i2c_read(SLAVE_ADDR, 0x00, rxData, 2);
//
//  success = i2c_write(SLAVE_ADDR, 0x60, txData, 2);
//
//
//  txData[0] = 0x0;
//  txData[1] = 0x0;
//  success = i2c_write(SLAVE_ADDR, 0x31,  txData, 2);
//
//
//  txData[0] = 0xC0;
//  txData[1] = 0xDE;
//  success = i2c_write(SLAVE_ADDR, 0x31, txData, 2);
//
//
//
////  MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
//
//  success = i2c_read(SLAVE_ADDR, 0x32, rxData, 2);
//
//
//
//  txData[0] = 0x00;
//  txData[1] = 0x02;
//  success = i2c_write(SLAVE_ADDR, 0x35, txData, 2);
//
//
//  success = i2c_read(SLAVE_ADDR, 0x32, rxData, 2);
//
//  success = i2c_read(SLAVE_ADDR, 0x93, rxData, 2);
//
//  txData[0] = 0x00;
//  txData[1] = 0x00;
//  success = i2c_write(SLAVE_ADDR, 0x60, txData, 2);
//
//  txData[0] = 0x81;
//  txData[1] = 0xFF;
//  success = i2c_write(SLAVE_ADDR, 0x30, txData, 2);
//
//  success = i2c_read(SLAVE_ADDR, 0x30, txData, 2);
//
//  while (1)
//  {
//    success = i2c_read(SLAVE_ADDR, 0x00, rxData, 2);
//    success = i2c_read(SLAVE_ADDR, 0x05, rxData+2, 2);
//  }

//  while ( 1 )
//  {
//    for ( uint8_t r = 0; r < 2; r++ )
//    {
//      for ( uint8_t g = 0; g < 2; g++ )
//        {
//          for ( uint8_t b = 0; b < 2; b++ )
//          {
//            setpin (GPIO_PORT_P2, GPIO_PIN0, r);
//            setpin (GPIO_PORT_P2, GPIO_PIN1, g);
//            setpin (GPIO_PORT_P2, GPIO_PIN2, b);
//            /* Delay Loop */
//            for ( ii = 0; ii < 250000; ii++ )
//            {
//            }
//
//          }
//        }
//    }
//  }
}
