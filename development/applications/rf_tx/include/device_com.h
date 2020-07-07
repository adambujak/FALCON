/******************************************************************************
 * @file     device_com.h
 * @brief    Device Com Layer Header File
 * @version  1.0
 * @date     2020-06-16
 * @author   Adam Bujak - transcribed by Devin Bell
 ******************************************************************************/
#include <stdint.h>

void device_com_init(void);

uint32_t device_com_getStatus(void);

uint32_t device_com_read(uint8_t *data);

uint32_t device_com_write(uint8_t *data, uint16_t length);

uint32_t device_com_test(void);

uint32_t device_com_test_read(uint8_t *read_buf);

uint32_t device_com_test_write(uint8_t *data, uint16_t length);


