//
///* DriverLib Defines */
//#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
//
///* Standard Defines */
//#include <stdint.h>
//#include <stdbool.h>
//#include <string.h>
//
///* Slave Address for I2C Slave */
//#define SLAVE_ADDRESS       0x52
//#define NUM_OF_REC_BYTES    2
//
///* Variables */
//const uint8_t TXData[] = {0x32};//, 0x1F, 0x80};
//static uint8_t RXData[NUM_OF_REC_BYTES];
//static volatile uint32_t xferIndex;
//static volatile bool stopSent;
//unsigned int i=0, timeout=0;
//static volatile uint8_t eeprom = 0;
//
///* I2C Master Configuration Parameter */
//const eUSCI_I2C_MasterConfig i2cConfig =
//{
//        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
//        3000000,                                // SMCLK = 3MHz
//        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 100khz
//        0,                                      // No byte counter threshold
//        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
//};
//
//int main(void)
//{
//  
//
//    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
//     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
//     */
//    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
//            GPIO_PIN6 + GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
//    stopSent = false;
//    memset(RXData, 0x00, NUM_OF_REC_BYTES);
//
//    /* Initializing I2C Master to SMCLK at 100khz with no autostop */
//    MAP_I2C_initMaster(EUSCI_B0_BASE, &i2cConfig);
//
//    /* Set clock low timeout, interrupt triggered if slave hold SCL>28ms */
//    MAP_I2C_setTimeout(EUSCI_B0_BASE, EUSCI_B_I2C_TIMEOUT_28_MS);
//
//    /* Specify slave address */
//    MAP_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);
//
//    /* Enable I2C Module to start operations */
//    MAP_I2C_enableModule(EUSCI_B0_BASE);
//    MAP_Interrupt_enableInterrupt(INT_EUSCIB0);
//    MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
//    MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT);
//
//    /* Making sure the last transaction has been completely sent out */
//    while (MAP_I2C_masterIsStopSent(EUSCI_B0_BASE));
//
//    MAP_Interrupt_enableSleepOnIsrExit();
//
//    /* Send start and the first byte of the transmit buffer. */
//    MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, 0x31);
//    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, 0x00);
//    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, 0x00);
//    MAP_I2C_masterSendMultiByteStop(EUSCI_B0_BASE);
//
//    MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, 0x31);
//    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, 0xDE);
//    MAP_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, 0xC0);
//    MAP_I2C_masterSendMultiByteStop(EUSCI_B0_BASE);
//
////    /* Sent the first byte, now we need to initiate the read */
//    MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, 0x32);
//    xferIndex = 0;
//    MAP_I2C_masterReceiveStart(EUSCI_B0_BASE);
//
//    while( !eeprom );
//    MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, 0x93);
//    xferIndex = 0;
//    MAP_I2C_masterReceiveStart(EUSCI_B0_BASE);
//    while(1)
//    {
//        MAP_PCM_gotoLPM0InterruptSafe();
//
//        /* Blink P1.0 LED if timeout occurred */
//        while(timeout)
//        {
//            MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
//            __delay_cycles(300000);
//        }
//    }
//}
//
///*******************************************************************************
// * eUSCIB0 ISR. The repeated start and transmit/receive operations happen
// * within this ISR.
// *******************************************************************************/
//void EUSCIB0_IRQHandler(void)
//{
//    uint_fast16_t status;
//
//    status = MAP_I2C_getEnabledInterruptStatus(EUSCI_B0_BASE);
//    MAP_I2C_clearInterruptFlag(EUSCI_B0_BASE, status);
//
//    /* Check for clock low timeout and increment timeout counter */
//    if (status & EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT)
//    {
//        timeout++;
//    }
//
//    /* Receives bytes into the receive buffer. If we have received all bytes,
//     * send a STOP condition */
//    if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT0)
//    {
//        if (xferIndex == NUM_OF_REC_BYTES - 2)
//        {
//            MAP_I2C_disableInterrupt(EUSCI_B0_BASE,
//                    EUSCI_B_I2C_RECEIVE_INTERRUPT0);
//            MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_STOP_INTERRUPT);
//
//            /*
//             * Switch order so that stop is being set during reception of last
//             * byte read byte so that next byte can be read.
//             */
//            MAP_I2C_masterReceiveMultiByteStop(EUSCI_B0_BASE);
//            RXData[xferIndex++] = MAP_I2C_masterReceiveMultiByteNext(
//                    EUSCI_B0_BASE);
//        } else
//        {
//            RXData[xferIndex++] = MAP_I2C_masterReceiveMultiByteNext(
//            EUSCI_B0_BASE);
//        }
//    }
//    else if (status & EUSCI_B_I2C_STOP_INTERRUPT)
//    {
//
//        MAP_Interrupt_disableSleepOnIsrExit();
//        MAP_I2C_disableInterrupt(EUSCI_B0_BASE,
//                                 EUSCI_B_I2C_STOP_INTERRUPT);
//        eeprom++;
//    }
//}
