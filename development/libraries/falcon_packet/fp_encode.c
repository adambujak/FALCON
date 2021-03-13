#include "fp_encode.h"

static uint8_t encode_header(uint8_t *buffer, fp_type_t packetType)
{
  buffer[0] = PACKET_DELIMITER1;
  buffer[1] = PACKET_DELIMITER2;
  buffer[2] = (uint8_t)packetType;
  return PACKET_HEADER_SIZE;
}

static inline uint8_t encode_uint8(uint8_t *buffer, uint8_t *value)
{
  for (uint32_t i = 0; i < 1; i++) {
    buffer[i] = ((*value) >> (i * 8)) & 0xFF;
  }
  return 1;
}

static inline uint8_t encode_int8(uint8_t *buffer, int8_t *value)
{
  for (uint32_t i = 0; i < 1; i++) {
    buffer[i] = ((*value) >> (i * 8)) & 0xFF;
  }
  return 1;
}

static inline uint8_t encode_uint16(uint8_t *buffer, uint16_t *value)
{
  for (uint32_t i = 0; i < 2; i++) {
    buffer[i] = ((*value) >> (i * 8)) & 0xFF;
  }
  return 2;
}

static inline uint8_t encode_int16(uint8_t *buffer, int16_t *value)
{
  for (uint32_t i = 0; i < 2; i++) {
    buffer[i] = ((*value) >> (i * 8)) & 0xFF;
  }
  return 2;
}

static inline uint8_t encode_uint32(uint8_t *buffer, uint32_t *value)
{
  for (uint32_t i = 0; i < 4; i++) {
    buffer[i] = ((*value) >> (i * 8)) & 0xFF;
  }
  return 4;
}

static inline uint8_t encode_int32(uint8_t *buffer, int32_t *value)
{
  for (uint32_t i = 0; i < 4; i++) {
    buffer[i] = ((*value) >> (i * 8)) & 0xFF;
  }
  return 4;
}

static inline uint8_t encode_flo(uint8_t *buffer, float *value)
{
  uint32_t *tempValue = (uint32_t *) value;
  for (uint32_t i = 0; i < 4; i++) {
    buffer[i] = ((*tempValue) >> (i * 8)) & 0xFF;
  }
  return 4;
}

static inline uint8_t encode_fe_flight_control_mode(uint8_t *buffer, fe_flight_control_mode_t *value)
{
  uint32_t tempValue = (uint32_t) *value;
  for (uint32_t i = 0; i < 1; i++) {
     buffer[i] = ((tempValue) >> (i * 8)) & 0xFF;
  }
  return 1;
}

static inline uint8_t encode_ft_motor_pwm_control_data(uint8_t *buffer, ft_motor_pwm_control_data_t *value)
{
  encode_uint16(&buffer[0], &value->motor1);
  encode_uint16(&buffer[2], &value->motor2);
  encode_uint16(&buffer[4], &value->motor3);
  encode_uint16(&buffer[6], &value->motor4);
  return 8;
}

static inline uint8_t encode_ft_gps_coordinate_data(uint8_t *buffer, ft_gps_coordinate_data_t *value)
{
  encode_flo(&buffer[0], &value->longitude);
  encode_flo(&buffer[4], &value->latitude);
  return 8;
}

static inline uint8_t encode_ft_status_data(uint8_t *buffer, ft_status_data_t *value)
{
  encode_ft_gps_coordinate_data(&buffer[0], &value->gps);
  encode_ft_motor_pwm_control_data(&buffer[8], &value->motor);
  encode_uint8(&buffer[16], &value->someEight);
  encode_int8(&buffer[17], &value->someIEight);
  encode_int16(&buffer[18], &value->someSixteen);
  encode_uint32(&buffer[20], &value->someThirtyTwo);
  encode_int32(&buffer[24], &value->someIThirtyTwo);
  return 28;
}

static inline uint8_t encode_ft_orientation_reference(uint8_t *buffer, ft_orientation_reference_t *value)
{
  encode_flo(&buffer[0], &value->yaw);
  encode_flo(&buffer[4], &value->pitch);
  encode_flo(&buffer[8], &value->roll);
  encode_flo(&buffer[12], &value->z);
  return 16;
}

static inline uint8_t encode_ft_position_reference(uint8_t *buffer, ft_position_reference_t *value)
{
  encode_flo(&buffer[0], &value->x);
  encode_flo(&buffer[4], &value->y);
  encode_flo(&buffer[8], &value->z);
  encode_flo(&buffer[12], &value->yaw);
  return 16;
}

uint8_t fpc_flight_control_stick_encode(uint8_t *buffer, fpc_flight_control_stick_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_FLIGHT_CONTROL_STICK_COMMAND);
  encode_ft_orientation_reference(&buffer[3], &packet->orientationReferenceCmd);
  return size;
}

uint8_t fpc_flight_control_position_encode(uint8_t *buffer, fpc_flight_control_position_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_FLIGHT_CONTROL_POSITION_COMMAND);
  encode_ft_position_reference(&buffer[3], &packet->positionReferenceCMD);
  return size;
}

uint8_t fpc_motor_speed_encode(uint8_t *buffer, fpc_motor_speed_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_MOTOR_SPEED_COMMAND);
  encode_ft_motor_pwm_control_data(&buffer[3], &packet->pwmData);
  return size;
}

uint8_t fpq_motor_speed_encode(uint8_t *buffer, fpq_motor_speed_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_MOTOR_SPEED_QUERY);
  encode_ft_motor_pwm_control_data(&buffer[3], &packet->pwmData);
  return size;
}

uint8_t fpr_motor_speed_encode(uint8_t *buffer, fpr_motor_speed_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_MOTOR_SPEED_RESPONSE);
  encode_ft_motor_pwm_control_data(&buffer[3], &packet->pwmData);
  return size;
}

uint8_t fpc_set_destination_encode(uint8_t *buffer, fpc_set_destination_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_SET_DESTINATION_COMMAND);
  encode_ft_gps_coordinate_data(&buffer[3], &packet->gpsData);
  return size;
}

uint8_t fpr_status_encode(uint8_t *buffer, fpr_status_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_STATUS_RESPONSE);
  encode_ft_gps_coordinate_data(&buffer[3], &packet->gpsData);
  encode_ft_status_data(&buffer[11], &packet->status);
  return size;
}

uint8_t fp_encode_packet(uint8_t *buffer, void *packet, fp_type_t packetType)
{
  switch(packetType) {
    case FPT_FLIGHT_CONTROL_STICK_COMMAND:
      return fpc_flight_control_stick_encode(buffer, (fpc_flight_control_stick_t *) packet);

    case FPT_FLIGHT_CONTROL_POSITION_COMMAND:
      return fpc_flight_control_position_encode(buffer, (fpc_flight_control_position_t *) packet);

    case FPT_MOTOR_SPEED_COMMAND:
      return fpc_motor_speed_encode(buffer, (fpc_motor_speed_t *) packet);

    case FPT_MOTOR_SPEED_QUERY:
      return fpq_motor_speed_encode(buffer, (fpq_motor_speed_t *) packet);

    case FPT_MOTOR_SPEED_RESPONSE:
      return fpr_motor_speed_encode(buffer, (fpr_motor_speed_t *) packet);

    case FPT_SET_DESTINATION_COMMAND:
      return fpc_set_destination_encode(buffer, (fpc_set_destination_t *) packet);

    case FPT_STATUS_RESPONSE:
      return fpr_status_encode(buffer, (fpr_status_t *) packet);

    default:
       return 0;
  }
}