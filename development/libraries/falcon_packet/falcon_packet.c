
#include "falcon_packet.h"

static uint8_t packet_size_lookup_table[FPT_CNT] = {
  [FPT_MODE_COMMAND] = 1,
  [FPT_MODE_QUERY] = 0,
  [FPT_MODE_RESPONSE] = 1,
  [FPT_FLIGHT_CONTROL_COMMAND] = 16,
  [FPT_STATUS_RESPONSE] = 57,
  [FPT_TEST_QUERY] = 0,
  [FPT_TEST_RESPONSE] = 4,
  [FPT_RADIO_STATS_QUERY] = 0,
  [FPT_RADIO_STATS_RESPONSE] = 4,
};

fp_type_t fp_get_packet_type(uint8_t packetID)
{
  return (fp_type_t) packetID;
}

uint8_t fp_get_packet_length(fp_type_t packetType)
{
  return packet_size_lookup_table[packetType] + PACKET_HEADER_SIZE;
}

