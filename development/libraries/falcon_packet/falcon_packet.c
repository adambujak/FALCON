
#include "falcon_packet.h"

static uint8_t packet_size_lookup_table[FPT_CNT] = {
  [FPT_FLIGHT_CONTROL_STICK_COMMAND] = 16,
  [FPT_FLIGHT_CONTROL_POSITION_COMMAND] = 16,
  [FPT_MOTOR_SPEED_COMMAND] = 8,
  [FPT_MOTOR_SPEED_QUERY] = 8,
  [FPT_MOTOR_SPEED_RESPONSE] = 8,
  [FPT_SET_DESTINATION_COMMAND] = 8,
  [FPT_STATUS_RESPONSE] = 36,
};

fp_type_t fp_get_packet_type(uint8_t packetID)
{
  return (fp_type_t) packetID;
}

uint8_t fp_get_packet_length(fp_type_t packetType)
{
  return packet_size_lookup_table[packetType];
}

