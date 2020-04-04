#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include <stdint.h>
#include <stdbool.h>
#include "i2c_master.h"
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

int main ( void )
{
  volatile uint32_t ii;

  /* Halting the Watchdog */
  MAP_WDT_A_holdTimer();

  /* Configuring RGB LED pins as output */
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);



  fi2c_t * i2cInstance;
  i2cInstance = FI2C_Initialize(FIO_MODULE_0, &fi2c_default_config);


  fi2c_reg_t txData[] = {0xFF, 0x00, 0x00};
  fi2c_reg_t rxData[] = {0xFF, 0xFF, 0xFF, 0xFF};

  i2cInstance->blockingWrite(i2cInstance, SLAVE_ADDR, 0x60, txData, 2);


  txData[0] = 0x0;
  txData[1] = 0x0;
  i2cInstance->blockingWrite(i2cInstance, SLAVE_ADDR, 0x31,  txData, 2);


  txData[0] = 0xC0;
  txData[1] = 0xDE;
  i2cInstance->blockingWrite(i2cInstance, SLAVE_ADDR, 0x31, txData, 2);



  i2cInstance->blockingRead(i2cInstance, SLAVE_ADDR, 0x32, rxData, 2);



  txData[0] = 0x00;
  txData[1] = 0x02;
  i2cInstance->blockingWrite(i2cInstance, SLAVE_ADDR, 0x35, txData, 2);


  i2cInstance->blockingRead(i2cInstance, SLAVE_ADDR, 0x32, rxData, 2);

  i2cInstance->blockingRead(i2cInstance, SLAVE_ADDR, 0x93, rxData, 2);

  txData[0] = 0x00;
  txData[1] = 0x00;
  i2cInstance->blockingWrite(i2cInstance, SLAVE_ADDR, 0x60, txData, 2);

  txData[0] = 0x81;
  txData[1] = 0xFF;
  i2cInstance->blockingWrite(i2cInstance, SLAVE_ADDR, 0x30, txData, 2);

  i2cInstance->blockingRead(i2cInstance, SLAVE_ADDR, 0x30, txData, 2);

  i2cInstance->blockingRead(i2cInstance, SLAVE_ADDR, 0x05, rxData, 2);
  while ( 1 )
  {
    for ( uint8_t r = 0; r < 2; r++ )
    {
      for ( uint8_t g = 0; g < 2; g++ )
        {
          for ( uint8_t b = 0; b < 2; b++ )
          {
            setpin (GPIO_PORT_P2, GPIO_PIN0, r);
            setpin (GPIO_PORT_P2, GPIO_PIN1, g);
            setpin (GPIO_PORT_P2, GPIO_PIN2, b);
            /* Delay Loop */
            for ( ii = 0; ii < 250000; ii++ )
            {
            }

          }
        }
    }
  }
}
