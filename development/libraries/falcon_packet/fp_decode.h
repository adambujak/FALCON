#ifndef FP_DECODE_H
#define FP_DECODE_H


#include "falcon_packet.h"

void fpc_flight_control_stick_decode(uint8_t *buffer, fpc_flight_control_stick_t *packet);
void fpc_flight_control_position_decode(uint8_t *buffer, fpc_flight_control_position_t *packet);
void fpc_motor_speed_decode(uint8_t *buffer, fpc_motor_speed_t *packet);
void fpq_motor_speed_decode(uint8_t *buffer, fpq_motor_speed_t *packet);
void fpr_motor_speed_decode(uint8_t *buffer, fpr_motor_speed_t *packet);
void fpc_set_destination_decode(uint8_t *buffer, fpc_set_destination_t *packet);
void fpr_status_decode(uint8_t *buffer, fpr_status_t *packet);
#endif /* FP_DECODE_H */