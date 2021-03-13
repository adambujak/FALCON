#ifndef FP_ENCODE_H
#define FP_ENCODE_H


#include "falcon_packet.h"

uint8_t fp_encode_packet(uint8_t *buffer, void *packet, fp_type_t packetType);
uint8_t fpc_flight_control_stick_encode(uint8_t *buffer, fpc_flight_control_stick_t *packet);
uint8_t fpc_flight_control_position_encode(uint8_t *buffer, fpc_flight_control_position_t *packet);
uint8_t fpc_motor_speed_encode(uint8_t *buffer, fpc_motor_speed_t *packet);
uint8_t fpq_motor_speed_encode(uint8_t *buffer, fpq_motor_speed_t *packet);
uint8_t fpr_motor_speed_encode(uint8_t *buffer, fpr_motor_speed_t *packet);
uint8_t fpc_set_destination_encode(uint8_t *buffer, fpc_set_destination_t *packet);
uint8_t fpr_status_encode(uint8_t *buffer, fpr_status_t *packet);
#endif /* FP_ENCODE_H */