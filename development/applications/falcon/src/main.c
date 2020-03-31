#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include <stdint.h>
#include <stdbool.h>

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
