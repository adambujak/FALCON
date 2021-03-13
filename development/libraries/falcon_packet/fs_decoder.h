/******************************************************************************
 * @file     fs_decoder.h
 * @brief    Falcon Frame Stream Decoder
 * @version  1.0
 * @date     2021-03-04
 * @author   Adam Bujak
 ******************************************************************************/
#ifndef FS_DECODER_H
#define FS_DECODER_H

#include "falcon_packet.h"
#include "fp_decoder.h"
#include "grinbuf.h"
#include <stdbool.h>

typedef enum {
  FS_DECODER_STATE_FRAME_PARSE_HEADER,
  FS_DECODER_STATE_FRAME_PARSE_DATA,
  FS_DECODER_STATE_CNT
} fs_decoder_state_t;

typedef fp_decoder_cb_t fs_decoder_cb_t;

typedef struct {
  fs_decoder_cb_t callback;
} fs_decoder_config_t;

typedef struct {
  fs_decoder_cb_t callback;
  fs_decoder_state_t state;
  uint32_t currentFrameIndex;
  uint32_t currentFrameSize;
  grinbuf_t ringbuf;
  uint8_t ringbuf_buffer[MAX_FRAME_SIZE * 2];
} fs_decoder_t;

void fs_decoder_init(fs_decoder_t *decoder, fs_decoder_config_t *config);
void fs_decoder_decode(fs_decoder_t *decoder, uint8_t *buffer, uint32_t length);

#endif // FS_DECODER_H
