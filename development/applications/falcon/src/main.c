/* DriverLib Defines */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Defines */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Slave Address for I2C Slave */
#define SLAVE_ADDRESS       0x48
#define NUM_OF_REC_BYTES    1

/* Variables */
const uint8_t TXData[] = {0x41,0x42};
static uint8_t RXData;

/* I2C Master Configuration Parameter */
// Baud rate selectable (100KBPS, 400KBPS)
const eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz (default)
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

int main(void)
{
    /* Disabling the Watchdog  */
    MAP_WDT_A_holdTimer();

    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);


    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);

    RXData=0;

    /* Initializing I2C Master to SMCLK at 100khz with no autostop */
    MAP_I2C_initMaster(EUSCI_B0_BASE, &i2cConfig);

    /* Specify slave address */
    MAP_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_B0_BASE);

    while (1)
    {
      RXData=0;

        /* Making sure the last transaction has been completely sent out */
        while (MAP_I2C_masterIsStopSent(EUSCI_B0_BASE));

        /* Send out EEPROM Mock Read Cmd (2 databytes) */
        MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, TXData[1]);  // Start + 1Byte
        MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, TXData[0]); // Poll for TXINT,Send 1Byte
        /*---------------------------------------------*/
        /* Now we need to initiate the read */
        /* Wait until 2nd Byte has been output to shift register */
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));

        // Send the restart condition, read one byte, send the stop condition right away
        EUSCI_B0->CTLW0 &= ~(EUSCI_B_CTLW0_TR);
        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;
        while(MAP_I2C_masterIsStartSent(EUSCI_B0_BASE));
        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0));
        RXData = EUSCI_B0->RXBUF;

        // Slave should send a single 'R' back
        if(RXData != 'R'){
            // Error- set P1.0 high
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            while(1);
        }
        __delay_cycles(300000); // ~100ms pause between transmissions
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
}

