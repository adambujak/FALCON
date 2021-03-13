#include "fp_decode.h"



static inline void decode_uint8(uint8_t *buffer, uint8_t *data)
{
  for (uint32_t i = 0; i < 1; i++) {
    *data |= buffer[i] << (i * 8);
  }
}

static inline void decode_int8(uint8_t *buffer, int8_t *data)
{
  for (uint32_t i = 0; i < 1; i++) {
    *data |= buffer[i] << (i * 8);
  }
}

static inline void decode_uint16(uint8_t *buffer, uint16_t *data)
{
  for (uint32_t i = 0; i < 2; i++) {
    *data |= buffer[i] << (i * 8);
  }
}

static inline void decode_int16(uint8_t *buffer, int16_t *data)
{
  for (uint32_t i = 0; i < 2; i++) {
    *data |= buffer[i] << (i * 8);
  }
}

static inline void decode_uint32(uint8_t *buffer, uint32_t *data)
{
  for (uint32_t i = 0; i < 4; i++) {
    *data |= buffer[i] << (i * 8);
  }
}

static inline void decode_int32(uint8_t *buffer, int32_t *data)
{
  for (uint32_t i = 0; i < 4; i++) {
    *data |= buffer[i] << (i * 8);
  }
}

static inline void decode_flo(uint8_t *buffer, float *data)
{
  uint32_t tempdata = 0;
  for (uint32_t i = 0; i < 4; i++) {
    tempdata |= buffer[i] << (i * 8);
  }
  *data = *((float *) &tempdata);
}

static inline void decode_fe_flight_control_mode(uint8_t *buffer, fe_flight_control_mode_t *data)
{
  uint32_t tempdata = 0;
  for (uint32_t i = 0; i < 1; i++) {
    tempdata |= buffer[i] << (i * 8);
  }
  *data = (fe_flight_control_mode_t) tempdata;
}

static inline void decode_ft_motor_pwm_control_data(uint8_t *buffer, ft_motor_pwm_control_data_t *data)
{
  decode_uint16(&buffer[0], &data->motor1);
  decode_uint16(&buffer[2], &data->motor2);
  decode_uint16(&buffer[4], &data->motor3);
  decode_uint16(&buffer[6], &data->motor4);
}

static inline void decode_ft_gps_coordinate_data(uint8_t *buffer, ft_gps_coordinate_data_t *data)
{
  decode_flo(&buffer[0], &data->longitude);
  decode_flo(&buffer[4], &data->latitude);
}

static inline void decode_ft_status_data(uint8_t *buffer, ft_status_data_t *data)
{
  decode_ft_gps_coordinate_data(&buffer[0], &data->gps);
  decode_ft_motor_pwm_control_data(&buffer[8], &data->motor);
  decode_uint8(&buffer[16], &data->someEight);
  decode_int8(&buffer[17], &data->someIEight);
  decode_int16(&buffer[18], &data->someSixteen);
  decode_uint32(&buffer[20], &data->someThirtyTwo);
  decode_int32(&buffer[24], &data->someIThirtyTwo);
}

static inline void decode_ft_orientation_reference(uint8_t *buffer, ft_orientation_reference_t *data)
{
  decode_flo(&buffer[0], &data->yaw);
  decode_flo(&buffer[4], &data->pitch);
  decode_flo(&buffer[8], &data->roll);
  decode_flo(&buffer[12], &data->z);
}

static inline void decode_ft_position_reference(uint8_t *buffer, ft_position_reference_t *data)
{
  decode_flo(&buffer[0], &data->x);
  decode_flo(&buffer[4], &data->y);
  decode_flo(&buffer[8], &data->z);
  decode_flo(&buffer[12], &data->yaw);
}

void fpc_flight_control_stick_decode(uint8_t *buffer, fpc_flight_control_stick_t *packet)
{
  decode_ft_orientation_reference(&buffer[0], &packet->orientationReferenceCmd);
}

void fpc_flight_control_position_decode(uint8_t *buffer, fpc_flight_control_position_t *packet)
{
  decode_ft_position_reference(&buffer[0], &packet->positionReferenceCMD);
}

void fpc_motor_speed_decode(uint8_t *buffer, fpc_motor_speed_t *packet)
{
  decode_ft_motor_pwm_control_data(&buffer[0], &packet->pwmData);
}

void fpq_motor_speed_decode(uint8_t *buffer, fpq_motor_speed_t *packet)
{
  decode_ft_motor_pwm_control_data(&buffer[0], &packet->pwmData);
}

void fpr_motor_speed_decode(uint8_t *buffer, fpr_motor_speed_t *packet)
{
  decode_ft_motor_pwm_control_data(&buffer[0], &packet->pwmData);
}

void fpc_set_destination_decode(uint8_t *buffer, fpc_set_destination_t *packet)
{
  decode_ft_gps_coordinate_data(&buffer[0], &packet->gpsData);
}

void fpr_status_decode(uint8_t *buffer, fpr_status_t *packet)
{
  decode_ft_gps_coordinate_data(&buffer[0], &packet->gpsData);
  decode_ft_status_data(&buffer[8], &packet->status);
}

