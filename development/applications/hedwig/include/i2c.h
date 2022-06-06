#ifndef I2C_H
#define I2C_H

#include <stdint.h>

int i2c_imu_write(uint8_t slave_addr, uint8_t reg_addr,
                  const uint8_t *data, uint16_t length);
int i2c_imu_read(uint8_t slave_addr, uint8_t reg_addr,
                 uint8_t *data, uint16_t length);
int i2c_baro_write(uint8_t slave_addr, uint8_t reg_addr,
                   const uint8_t *data, uint16_t length);
int i2c_baro_read(uint8_t slave_addr, uint8_t reg_addr,
                  uint8_t *data, uint16_t length);
int i2c_init(void);


#endif // I2C_H
