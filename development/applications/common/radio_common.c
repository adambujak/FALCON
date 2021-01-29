#include "radio_common.h"

#include <string.h>

void radio_get_hedwig_address(uint8_t *buffer)
{
  memset(buffer, HEDWIG_RADIO_ADDRESS, RADIO_ADDRESS_LENGTH);
}

void radio_get_albus_address(uint8_t *buffer)
{
  memset(buffer, ALBUS_RADIO_ADDRESS, RADIO_ADDRESS_LENGTH);
}
