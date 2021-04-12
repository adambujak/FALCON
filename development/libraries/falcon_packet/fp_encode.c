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

static inline uint8_t encode_fe_falcon_mode(uint8_t *buffer, fe_falcon_mode_t *value)
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

static inline uint8_t encode_ft_fcs_state_estimate(uint8_t *buffer, ft_fcs_state_estimate_t *value)
{
  encode_flo(&buffer[0], &value->x);
  encode_flo(&buffer[4], &value->y);
  encode_flo(&buffer[8], &value->z);
  encode_flo(&buffer[12], &value->dx);
  encode_flo(&buffer[16], &value->dy);
  encode_flo(&buffer[20], &value->dz);
  encode_flo(&buffer[24], &value->yaw);
  encode_flo(&buffer[28], &value->pitch);
  encode_flo(&buffer[32], &value->roll);
  encode_flo(&buffer[36], &value->p);
  encode_flo(&buffer[40], &value->q);
  encode_flo(&buffer[44], &value->r);
  return 48;
}

static inline uint8_t encode_ft_status_data(uint8_t *buffer, ft_status_data_t *value)
{
  encode_fe_falcon_mode(&buffer[0], &value->mode);
  encode_ft_motor_pwm_control_data(&buffer[1], &value->motor);
  encode_ft_fcs_state_estimate(&buffer[9], &value->states);
  return 57;
}

static inline uint8_t encode_ft_fcs_control_input(uint8_t *buffer, ft_fcs_control_input_t *value)
{
  encode_flo(&buffer[0], &value->yaw);
  encode_flo(&buffer[4], &value->pitch);
  encode_flo(&buffer[8], &value->roll);
  encode_flo(&buffer[12], &value->alt);
  return 16;
}

uint8_t fpc_mode_encode(uint8_t *buffer, fpc_mode_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_MODE_COMMAND);
  encode_fe_falcon_mode(&buffer[3], &packet->mode);
  return size;
}

uint8_t fpq_mode_encode(uint8_t *buffer, fpq_mode_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_MODE_QUERY);
  encode_fe_falcon_mode(&buffer[3], &packet->mode);
  return size;
}

uint8_t fpr_mode_encode(uint8_t *buffer, fpr_mode_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_MODE_RESPONSE);
  encode_fe_falcon_mode(&buffer[3], &packet->mode);
  return size;
}

uint8_t fpc_flight_control_encode(uint8_t *buffer, fpc_flight_control_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_FLIGHT_CONTROL_COMMAND);
  encode_ft_fcs_control_input(&buffer[3], &packet->fcsControlCmd);
  return size;
}

uint8_t fpr_status_encode(uint8_t *buffer, fpr_status_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_STATUS_RESPONSE);
  encode_ft_status_data(&buffer[3], &packet->status);
  return size;
}

uint8_t fpq_test_encode(uint8_t *buffer, fpq_test_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_TEST_QUERY);
  encode_uint32(&buffer[3], &packet->cookie);
  return size;
}

uint8_t fpr_test_encode(uint8_t *buffer, fpr_test_t *packet)
{
  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;
  encode_header(buffer, FPT_TEST_RESPONSE);
  encode_uint32(&buffer[3], &packet->cookie);
  return size;
}

uint8_t fp_encode_packet(uint8_t *buffer, void *packet, fp_type_t packetType)
{
  switch(packetType) {
    case FPT_MODE_COMMAND:
      return fpc_mode_encode(buffer, (fpc_mode_t *) packet);

    case FPT_MODE_QUERY:
      return fpq_mode_encode(buffer, (fpq_mode_t *) packet);

    case FPT_MODE_RESPONSE:
      return fpr_mode_encode(buffer, (fpr_mode_t *) packet);

    case FPT_FLIGHT_CONTROL_COMMAND:
      return fpc_flight_control_encode(buffer, (fpc_flight_control_t *) packet);

    case FPT_STATUS_RESPONSE:
      return fpr_status_encode(buffer, (fpr_status_t *) packet);

    case FPT_TEST_QUERY:
      return fpq_test_encode(buffer, (fpq_test_t *) packet);

    case FPT_TEST_RESPONSE:
      return fpr_test_encode(buffer, (fpr_test_t *) packet);

    default:
       return 0;
  }
}