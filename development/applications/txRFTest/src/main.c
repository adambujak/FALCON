#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "rf_bsp.h"
#include "device_com.h"

int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    nrf_delay_ms(100);

    NRF_LOG_INFO("Started TX test app");
    NRF_LOG_FLUSH();
    device_com_init();
    NRF_LOG_INFO("Device com init");
    NRF_LOG_FLUSH();

    while(1)
    {   
        device_com_process();

        NRF_LOG_FLUSH();
        bsp_board_led_invert(BSP_BOARD_LED_0);
        nrf_delay_ms(500);
    }
}
