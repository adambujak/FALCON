#ifndef FP_ENCODE_H
#define FP_ENCODE_H


#include "falcon_packet.h"

uint8_t fp_encode_packet(uint8_t *buffer, void *packet, fp_type_t packetType);
uint8_t fpc_mode_encode(uint8_t *buffer, fpc_mode_t *packet);
uint8_t fpq_mode_encode(uint8_t *buffer, fpq_mode_t *packet);
uint8_t fpr_mode_encode(uint8_t *buffer, fpr_mode_t *packet);
uint8_t fpc_flight_control_encode(uint8_t *buffer, fpc_flight_control_t *packet);
uint8_t fpr_status_encode(uint8_t *buffer, fpr_status_t *packet);
uint8_t fpq_test_encode(uint8_t *buffer, fpq_test_t *packet);
uint8_t fpr_test_encode(uint8_t *buffer, fpr_test_t *packet);
uint8_t fpq_radio_stats_encode(uint8_t *buffer, fpq_radio_stats_t *packet);
uint8_t fpr_radio_stats_encode(uint8_t *buffer, fpr_radio_stats_t *packet);
#endif /* FP_ENCODE_H */