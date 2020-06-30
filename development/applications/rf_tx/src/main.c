#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "boards.h"
#include "rf_bsp.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "frf.h"



int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    nrf_delay_ms(100);

   
    


    // frf_init(&rfRxModule, rfRxConfig);

    // frf_start(&rfRxModule, 2, 4);

    // frf_tx_address(&rfRxModule, rx_address);
    // frf_rx_address(&rfRxModule, tx_address);




    while(1)
    {
        if(frf_dataReady(&rfRxModule))
        {
            frf_getData(&rfRxModule, data_array);
            NRF_LOG_INFO("> ");
            NRF_LOG_INFO("%d ",data_array[0]);
            NRF_LOG_INFO("%d ",data_array[1]);
            NRF_LOG_INFO("%d ",data_array[2]);
            NRF_LOG_INFO("%d\r\n",data_array[3]);
        }
        tx_buffer[3]++;
        nrf_delay_ms(100);
        frf_send(&rfTxModule, tx_buffer, 4);

        /* Wait for transmission to end */
        while(frf_isSending(&rfTxModule));

        uint8_t temp = frf_lastMessageStatus(&rfTxModule);

        if(temp == FRF_TRANSMISSON_OK)
        {
            NRF_LOG_INFO("Transmission success!\r\n");
            NRF_LOG_FLUSH();
        }
        else if(temp == FRF_MESSAGE_LOST)
        {
            NRF_LOG_INFO("Transmission failed!\r\n");
            NRF_LOG_FLUSH();
        }

        if (rxIRQFiredFlag == 1)
        {
            NRF_LOG_INFO("Clear Interrupts");
            frf_clearInterrupts(&rfRxModule);
            rxIRQFiredFlag = 0;
        }

        NRF_LOG_FLUSH();
        bsp_board_led_invert(BSP_BOARD_LED_0);
        nrf_delay_ms(500);
    }
}
