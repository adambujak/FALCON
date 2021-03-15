//#include <Windows.h>
//#include <conio.h>

#include <stdio.h>
#include <string.h>

#include "driver/inv_mems_hw_config.h"
#include "invn/common/invn_types.h"

#include "bsp.h"
#include <FreeRTOS.h>
fln_i2c_handle_t *ICM_i2c_handle; 
void ICM_connect_i2c(fln_i2c_handle_t *handle)
{
    ICM_i2c_handle = handle;
}


int inv_serial_interface_write_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
    int result;

    result = bsp_i2c_write(ICM_i2c_handle, ACCEL_GYRO_CHIP_ADDR, (uint8_t)reg, (uint16_t)length, data);

    return result;
}

int inv_serial_interface_read_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
	int status;
    
    status = bsp_i2c_read(ICM_i2c_handle, ACCEL_GYRO_CHIP_ADDR, (uint8_t)reg, (uint16_t)length, data);

	return status;
}

/**
 *  @brief  Sleep function.
**/
void inv_sleep(unsigned long mSecs)
{
//    TickType_t xDelay;
//
//    unsigned long TICK_PERIOD_MS = 1000L / configTICK_RATE_HZ;
//    /* take the ceiling */
//    xDelay = (mSecs + TICK_PERIOD_MS - 1) / TICK_PERIOD_MS;
//
//    /* must add one tick to ensure a full duration of xDelay ticks */
//    vTaskDelay(xDelay + 1);
	hedwig_delay(mSecs);
}

void inv_sleep_100us(unsigned long nHowMany100MicroSecondsToSleep)
{
    (void)nHowMany100MicroSecondsToSleep;
    inv_sleep(1);
}

/**
 *  @brief  get system's internal tick count.
 *          Used for time reference.
 *  @return current tick count.
**/
long inv_get_tick_count(void)
{
    TickType_t xTicks = xTaskGetTickCount();
    return (long)xTicks;
}

