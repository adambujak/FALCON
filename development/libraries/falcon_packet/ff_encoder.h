/******************************************************************************
 * @file     ff_encoder.h
 * @brief    Falcon Frame Encoder
 * @version  1.0
 * @date     2021-03-04
 * @author   Adam Bujak
 ******************************************************************************/
#ifndef FP_ENCODER_H
#define FP_ENCODER_H

#include "falcon_packet.h"
#include <stdint.h>

typedef struct {
  uint8_t *destBuffer;
  uint8_t writeIndex;
} ff_encoder_t;

void ff_encoder_init(ff_encoder_t *encoder);
void ff_encoder_set_buffer(ff_encoder_t *encoder, uint8_t *buffer);
int ff_encoder_append_packet(ff_encoder_t *encoder, void *packet, fp_type_t packetType);
int ff_encoder_append_data(ff_encoder_t *encoder, uint8_t *data, uint32_t length);
int ff_encoder_append_footer(ff_encoder_t *encoder);
uint32_t ff_encoder_get_remaining_bytes(ff_encoder_t *encoder);

#endif // FP_ENCODER_H
