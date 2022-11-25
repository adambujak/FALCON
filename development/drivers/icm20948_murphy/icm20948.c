/****************************************************************************
*   ICM-20948 9-Axis MEMS Motion Tracking Device Driver
*
*   Copyright (C) 2020 Stephen Murphy - github.com/stephendpmurphy
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
****************************************************************************/

/*! @file icm20948.c
 * @brief Source file for the ICM20948 9-Axis MEMS device driver.
 */

#include <string.h>
#include "icm20948.h"
#include "icm20948_api.h"

#include "hedwig.h"

/*! @brief Structure holding reference to our interface functions
 * and the ICM20948 register values */
static icm20948_dev_t dev;

/*! @brief Current settings applied to the device */
static icm20948_settings_t settings;

/*!
 * @brief This API reads data via spi while also setting the Read bit on the address,
 * using the provided interface function
 *
 * @param[in] addr: Reg address to read from
 * @param[in] data: Pointer to the buffer we want to read data into
 * @param[in] len: Length of data to be read
 *
 * @return Returns the read status
 */
static icm20948_return_code_t _spi_read(uint8_t addr, uint8_t *data, uint32_t len)
{
  return dev.intf.read(addr, data, len);
}

/*!
 * @brief This API sends data via spi using the provided interface function
 *
 * @param[in] addr: Reg address to written from
 * @param[in] data: Pointer to the buffer we want to write data from
 * @param[in] len: Length of data to be written
 *
 * @return Returns the write status
 */
static icm20948_return_code_t _spi_write(uint8_t addr, uint8_t *data, uint32_t len)
{
  return dev.intf.write(addr, data, len);
}

static icm20948_return_code_t _set_bank(icm20948_reg_bank_sel_t bank_sel)
{
  icm20948_return_code_t ret = ICM20948_RET_OK;

  // If current bank is different, change it
  if ((dev.usr_bank.reg_bank_sel != bank_sel)) {
    dev.usr_bank.reg_bank_sel = bank_sel;
    uint8_t bank_sel_data = bank_sel << 4;
    ret = _spi_write(ICM20948_ADDR_REG_BANK_SEL, &bank_sel_data, 0x01);
  }
  return ret;
}

static icm20948_return_code_t _enable_gyro(icm20948_settings_t *settings)
{
  icm20948_return_code_t ret = ICM20948_RET_OK;

  ret |= _set_bank(ICM20948_USER_BANK_2);

  dev.usr_bank.bank2.bytes.GYRO_CONFIG_1.bits.GYRO_FS_SEL = settings->gyro.fs;
  dev.usr_bank.bank2.bytes.GYRO_CONFIG_1.bits.GYRO_FCHOICE = 1;
  dev.usr_bank.bank2.bytes.GYRO_CONFIG_1.bits.GYRO_DLPFCFG = 5;
  ret |= _spi_write(ICM20948_ADDR_GYRO_CONFIG_1, &dev.usr_bank.bank2.bytes.GYRO_CONFIG_1.byte, 0x01);

  dev.usr_bank.bank2.bytes.GYRO_SMPLRT_DIV = 0x0A;
  ret |= _spi_write(ICM20948_ADDR_GYRO_SMPLRT_DIV, &dev.usr_bank.bank2.bytes.GYRO_SMPLRT_DIV, 0x01);
  return ret;
}

static icm20948_return_code_t _disable_gyro(void)
{
  icm20948_return_code_t ret = ICM20948_RET_OK;

  ret |= _set_bank(ICM20948_USER_BANK_0);

  ret |= _spi_read(ICM20948_ADDR_PWR_MGMT_2, &dev.usr_bank.bank0.bytes.PWR_MGMT_2.byte, 0x01);

  dev.usr_bank.bank0.bytes.PWR_MGMT_2.bits.DISABLE_GYRO = 0b111;
  ret |= _spi_write(ICM20948_ADDR_PWR_MGMT_2, &dev.usr_bank.bank0.bytes.PWR_MGMT_2.byte, 0x01);
  return ret;
}

static icm20948_return_code_t _enable_accel(icm20948_settings_t *settings)
{
  icm20948_return_code_t ret = ICM20948_RET_OK;

  ret |= _set_bank(ICM20948_USER_BANK_2);

  dev.usr_bank.bank2.bytes.ACCEL_CONFIG.bits.ACCEL_FS_SEL = settings->accel.fs;
  dev.usr_bank.bank2.bytes.ACCEL_CONFIG.bits.ACCEL_FCHOICE = 0;
  dev.usr_bank.bank2.bytes.ACCEL_CONFIG.bits.ACCEL_DLPFCFG = 0;
  ret |= _spi_write(ICM20948_ADDR_ACCEL_CONFIG, &dev.usr_bank.bank2.bytes.ACCEL_CONFIG.byte, 0x01);

  // Set the sample rate
  dev.usr_bank.bank2.bytes.ACCEL_SMPLRT_DIV_1.bits.ACCEL_SMPLRT_DIV = 0;
  ret |= _spi_write(ICM20948_ADDR_ACCEL_SMPLRT_DIV_1, &dev.usr_bank.bank2.bytes.ACCEL_SMPLRT_DIV_1.byte, 0x01);

  dev.usr_bank.bank2.bytes.ACCEL_SMPLRT_DIV_2 = 0x0A;
  ret |= _spi_write(ICM20948_ADDR_ACCEL_SMPLRT_DIV_2, &dev.usr_bank.bank2.bytes.ACCEL_SMPLRT_DIV_2, 0x01);
  return ret;
}

static icm20948_return_code_t _disable_accel(void)
{
  icm20948_return_code_t ret = ICM20948_RET_OK;

  ret |= _set_bank(ICM20948_USER_BANK_0);

  ret |= _spi_read(ICM20948_ADDR_PWR_MGMT_2, &dev.usr_bank.bank0.bytes.PWR_MGMT_2.byte, 0x01);

  dev.usr_bank.bank0.bytes.PWR_MGMT_2.bits.DISABLE_ACCEL = 0b111;
  ret |= _spi_write(ICM20948_ADDR_PWR_MGMT_2, &dev.usr_bank.bank0.bytes.PWR_MGMT_2.byte, 0x01);
  return ret;
}

/*!
 * @brief This API initializes the ICM20948 comms interface, and then does a read from the device
 * to verify working comms
 */
icm20948_return_code_t icm20948_init(icm20948_read_fptr_t r, icm20948_write_fptr_t w, icm20948_delay_us_fptr_t delay)
{
  icm20948_return_code_t ret = ICM20948_RET_OK;

  // Verify that the function pointers given to us are not invalid
  if ((r == NULL) || (w == NULL) || (delay == NULL)) {
    // One of the functions given to us was a NULL pointer, return with a
    // NULL PTR return code
    ret = ICM20948_RET_NULL_PTR;
  }

  // Store the interface functions passed in to us to be used to
  // communicate with the IC.
  dev.intf.read = r;
  dev.intf.write = w;
  dev.intf.delay_us = delay;

  // Select user register bank 0
  dev.usr_bank.reg_bank_sel = ICM20948_USER_BANK_0;

  if (ret == ICM20948_RET_OK) {
    // Write to the reg bank select to select bank 0
    ret = _spi_write(ICM20948_ADDR_REG_BANK_SEL, 0, 0x01);
  }

  if (ret == ICM20948_RET_OK) {
    // Ensure the local WHO_AM_I value is zeroed out before reading it from the chip
    dev.usr_bank.bank0.bytes.WHO_AM_I = 0x00;

    // If the bank was selected, read the WHO_AM_I register
    ret = _spi_read(ICM20948_ADDR_WHO_AM_I, &dev.usr_bank.bank0.bytes.WHO_AM_I, 0x01);

    if (ret == ICM20948_RET_OK) {
      if (dev.usr_bank.bank0.bytes.WHO_AM_I != ICM20948_WHO_AM_I_DEFAULT) {
        // The WHO_AM_I ID was incorrect.
        ret = ICM20948_RET_GEN_FAIL;
      }
    }
  }

  if (ret == ICM20948_RET_OK) {
    // Set the clock to best available
    dev.usr_bank.bank0.bytes.PWR_MGMT_1.bits.CLKSEL = 1;
    dev.usr_bank.bank0.bytes.PWR_MGMT_1.bits.SLEEP = 0;
    dev.usr_bank.bank0.bytes.PWR_MGMT_1.bits.DEVICE_RESET = 0;
    ret = _spi_write(ICM20948_ADDR_PWR_MGMT_1, &dev.usr_bank.bank0.bytes.PWR_MGMT_1.byte, 0x01);
  }

  // Return our init status
  return ret;
}

/*!
 * @brief This API applys the developers settings for configuring the ICM20948 components
 */
icm20948_return_code_t icm20948_applySettings(icm20948_settings_t *newSettings)
{
  icm20948_return_code_t ret = ICM20948_RET_OK;

  // Copy over the new settings
  memcpy(&settings, newSettings, sizeof(settings));

  // Apply the new settings
  if (settings.gyro.en == ICM20948_MOD_ENABLED) {
    ret |= _enable_gyro(&settings);
  }
  else {
    ret |= _disable_gyro();
  }

  if (settings.accel.en == ICM20948_MOD_ENABLED) {
    ret |= _enable_accel(&settings);
  }
  else {
    ret |= _disable_accel();
  }

  return ret;
}

/*!
 * @brief This API retrieves the current gyro data from the device
 */
icm20948_return_code_t icm20948_getGyroData(icm20948_gyro_t *gyro)
{
  icm20948_return_code_t ret = ICM20948_RET_OK;

  // Check if the Gyro is enabled
  if (settings.gyro.en != ICM20948_MOD_ENABLED) {
    ret = ICM20948_RET_INV_CONFIG;
  }

  ret |= _set_bank(ICM20948_USER_BANK_0);

  ret |= _spi_read(ICM20948_ADDR_GYRO_XOUT_H, &dev.usr_bank.bank0.bytes.GYRO_XOUT_H, 0x06);

  gyro->x = ((int16_t) dev.usr_bank.bank0.bytes.GYRO_XOUT_H << 8) | dev.usr_bank.bank0.bytes.GYRO_XOUT_L;
  gyro->y = ((int16_t) dev.usr_bank.bank0.bytes.GYRO_YOUT_H << 8) | dev.usr_bank.bank0.bytes.GYRO_YOUT_L;
  gyro->z = ((int16_t) dev.usr_bank.bank0.bytes.GYRO_ZOUT_H << 8) | dev.usr_bank.bank0.bytes.GYRO_ZOUT_L;

  return ret;
}

/*!
 * @brief This API retrieves the current accel data from the device
 */
icm20948_return_code_t icm20948_getAccelData(icm20948_accel_t *accel)
{
  icm20948_return_code_t ret = ICM20948_RET_OK;

  // Check if the Accelerometer is enabled
  if (settings.accel.en != ICM20948_MOD_ENABLED) {
    ret = ICM20948_RET_INV_CONFIG;
  }

  ret |= _set_bank(ICM20948_USER_BANK_0);

  ret |= _spi_read(ICM20948_ADDR_ACCEL_XOUT_H, &dev.usr_bank.bank0.bytes.ACCEL_XOUT_H, 0x06);

  accel->x = ((int16_t) dev.usr_bank.bank0.bytes.ACCEL_XOUT_H << 8) | dev.usr_bank.bank0.bytes.ACCEL_XOUT_L;
  accel->y = ((int16_t) dev.usr_bank.bank0.bytes.ACCEL_YOUT_H << 8) | dev.usr_bank.bank0.bytes.ACCEL_YOUT_L;
  accel->z = ((int16_t) dev.usr_bank.bank0.bytes.ACCEL_ZOUT_H << 8) | dev.usr_bank.bank0.bytes.ACCEL_ZOUT_L;

  return ret;
}
