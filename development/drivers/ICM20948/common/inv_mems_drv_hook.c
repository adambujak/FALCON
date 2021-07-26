//#include <Windows.h>
//#include <conio.h>

#include <stdio.h>
#include <string.h>

#include "driver/inv_mems_hw_config.h"
#include "invn/common/invn_types.h"

#include "i2c.h"
#include "hedwig.h"

int inv_serial_interface_write_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
    int result;

    result = i2c_imu_write(ACCEL_GYRO_CHIP_ADDR, (uint8_t)reg, data, (uint16_t)length);

    return result;
}

int inv_serial_interface_read_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
	int status;

    status = i2c_imu_read(ACCEL_GYRO_CHIP_ADDR, (uint8_t)reg, data, (uint16_t)length);

	return status;
}

/**
 *  @brief  Sleep function.
**/
void inv_sleep(unsigned long mSecs)
{
	rtos_delay_ms(mSecs);
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

