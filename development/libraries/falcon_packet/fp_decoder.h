/******************************************************************************
 * @file     fp_decoder.h
 * @brief    Falcon Packet Decoder
 * @version  1.0
 * @date     2021-03-04
 * @author   Adam Bujak
 ******************************************************************************/
#ifndef FP_DECODER_H
#define FP_DECODER_H

#include "falcon_packet.h"
#include <stdbool.h>

typedef void (*fp_decoder_cb_t)(uint8_t *buffer, fp_type_t type);

void fp_decoder_decode(uint8_t *buffer, uint32_t length, fp_decoder_cb_t callback);

#endif // FP_DECODER_H
