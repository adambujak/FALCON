#ifndef FP_DECODE_H
#define FP_DECODE_H


#include "falcon_packet.h"

void fpc_mode_decode(uint8_t *buffer, fpc_mode_t *packet);
void fpq_mode_decode(uint8_t *buffer, fpq_mode_t *packet);
void fpr_mode_decode(uint8_t *buffer, fpr_mode_t *packet);
void fpc_flight_control_decode(uint8_t *buffer, fpc_flight_control_t *packet);
void fpr_status_decode(uint8_t *buffer, fpr_status_t *packet);
void fpq_test_decode(uint8_t *buffer, fpq_test_t *packet);
void fpr_test_decode(uint8_t *buffer, fpr_test_t *packet);
void fpq_radio_stats_decode(uint8_t *buffer, fpq_radio_stats_t *packet);
void fpr_radio_stats_decode(uint8_t *buffer, fpr_radio_stats_t *packet);
#endif /* FP_DECODE_H */