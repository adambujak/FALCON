
#include "falcon_packet.h"

static uint8_t packet_size_lookup_table[FPT_CNT] = {
  [FPT_MODE_COMMAND] = 1,
  [FPT_MODE_QUERY] = 1,
  [FPT_MODE_RESPONSE] = 1,
  [FPT_FLIGHT_CONTROL_COMMAND] = 16,
  [FPT_STATUS_RESPONSE] = 57,
};

fp_type_t fp_get_packet_type(uint8_t packetID)
{
  return (fp_type_t) packetID;
}

uint8_t fp_get_packet_length(fp_type_t packetType)
{
  return packet_size_lookup_table[packetType] + PACKET_HEADER_SIZE;
}

