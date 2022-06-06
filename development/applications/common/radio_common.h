#ifndef RADIO_COMMON_H
#define RADIO_COMMON_H

#include <stdint.h>

#define HEDWIG_RADIO_ADDRESS     0xD7
#define ALBUS_RADIO_ADDRESS      0xE7

#define RADIO_ADDRESS_LENGTH     5

#define RADIO_WATCHDOG_PERIOD    2000 // 2 seconds

void radio_get_hedwig_address(uint8_t *buffer);
void radio_get_albus_address(uint8_t *buffer);

#endif // RADIO_COMMON_H
