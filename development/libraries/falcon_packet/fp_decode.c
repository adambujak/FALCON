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

static inline void decode_fe_falcon_mode(uint8_t *buffer, fe_falcon_mode_t *data)
{
  uint32_t tempdata = 0;
  for (uint32_t i = 0; i < 1; i++) {
    tempdata |= buffer[i] << (i * 8);
  }
  *data = (fe_falcon_mode_t) tempdata;
}

static inline void decode_ft_motor_pwm_control_data(uint8_t *buffer, ft_motor_pwm_control_data_t *data)
{
  decode_uint16(&buffer[0], &data->motor1);
  decode_uint16(&buffer[2], &data->motor2);
  decode_uint16(&buffer[4], &data->motor3);
  decode_uint16(&buffer[6], &data->motor4);
}

static inline void decode_ft_fcs_state_estimate(uint8_t *buffer, ft_fcs_state_estimate_t *data)
{
  decode_flo(&buffer[0], &data->x);
  decode_flo(&buffer[4], &data->y);
  decode_flo(&buffer[8], &data->z);
  decode_flo(&buffer[12], &data->dx);
  decode_flo(&buffer[16], &data->dy);
  decode_flo(&buffer[20], &data->dz);
  decode_flo(&buffer[24], &data->yaw);
  decode_flo(&buffer[28], &data->pitch);
  decode_flo(&buffer[32], &data->roll);
  decode_flo(&buffer[36], &data->p);
  decode_flo(&buffer[40], &data->q);
  decode_flo(&buffer[44], &data->r);
}

static inline void decode_ft_status_data(uint8_t *buffer, ft_status_data_t *data)
{
  decode_fe_falcon_mode(&buffer[0], &data->mode);
  decode_ft_motor_pwm_control_data(&buffer[1], &data->motor);
  decode_ft_fcs_state_estimate(&buffer[9], &data->states);
}

static inline void decode_ft_fcs_control_input(uint8_t *buffer, ft_fcs_control_input_t *data)
{
  decode_flo(&buffer[0], &data->yaw);
  decode_flo(&buffer[4], &data->pitch);
  decode_flo(&buffer[8], &data->roll);
  decode_flo(&buffer[12], &data->alt);
}

void fpc_mode_decode(uint8_t *buffer, fpc_mode_t *packet)
{
  decode_fe_falcon_mode(&buffer[3], &packet->mode);
}

void fpq_mode_decode(uint8_t *buffer, fpq_mode_t *packet)
{
  decode_fe_falcon_mode(&buffer[3], &packet->mode);
}

void fpr_mode_decode(uint8_t *buffer, fpr_mode_t *packet)
{
  decode_fe_falcon_mode(&buffer[3], &packet->mode);
}

void fpc_flight_control_decode(uint8_t *buffer, fpc_flight_control_t *packet)
{
  decode_ft_fcs_control_input(&buffer[3], &packet->fcsControlCmd);
}

void fpr_status_decode(uint8_t *buffer, fpr_status_t *packet)
{
  decode_ft_status_data(&buffer[3], &packet->status);
}

