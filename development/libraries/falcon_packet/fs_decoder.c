#include "fs_decoder.h"
#include "checksum.h"

static void reset_frame(fs_decoder_t *decoder)
{
  decoder->state = FS_DECODER_STATE_FRAME_PARSE_HEADER;
  decoder->currentFrameIndex = 0;
  decoder->currentFrameSize = 0;
}

static void reset_decoder(fs_decoder_t *decoder)
{
  reset_frame(decoder);
  grinbuf_reset(&decoder->ringbuf);
}

void fs_decoder_init(fs_decoder_t *decoder, fs_decoder_config_t *config)
{
  decoder->callback = config->callback;
  grinbuf_init(&decoder->ringbuf, decoder->ringbuf_buffer, MAX_FRAME_SIZE * 2);

  reset_decoder(decoder);
}

void fs_decoder_decode(fs_decoder_t *decoder, uint8_t *buffer, uint32_t length)
{
  grinbuf_write(&decoder->ringbuf, buffer, length);

  while(true) {
    switch (decoder->state) {

      case FS_DECODER_STATE_FRAME_PARSE_HEADER: {
        uint8_t frameHeader[FRAME_HEADER_SIZE] = { 0 };
        uint32_t bytesRead = grinbuf_peek(&decoder->ringbuf, frameHeader, FRAME_HEADER_SIZE);
        
        if (bytesRead != FRAME_HEADER_SIZE) {
          /* Not enough data, try again when there's more */
          return;
        }
        
        if (frameHeader[0] != FRAME_DELIMITER1) {
          /* This ain't it chief, pop it off and retry next time */
          grinbuf_pop(&decoder->ringbuf, 1);
          break;
        }
        
        if (frameHeader[1] != FRAME_DELIMITER2) {
          /* This ain't it either chief, pop off first byte and retry next time */
          grinbuf_pop(&decoder->ringbuf, 1);
          break;
        }

        decoder->currentFrameSize = frameHeader[2];
        decoder->currentFrameIndex = FRAME_HEADER_SIZE;
        decoder->state = FS_DECODER_STATE_FRAME_PARSE_DATA;
        grinbuf_pop(&decoder->ringbuf, 3);
      }
      break;

      case FS_DECODER_STATE_FRAME_PARSE_DATA: {
        if (decoder->currentFrameIndex > (decoder->currentFrameSize - FRAME_FOOTER_SIZE)) {
          reset_frame(decoder);
          break;
        }
        
        uint32_t bytesAvailable = grinbuf_getBytesUsed(&decoder->ringbuf);
        uint32_t bytesToRead = decoder->currentFrameSize - FRAME_HEADER_SIZE;
        
        if (bytesAvailable < bytesToRead) {
          /* Not enough data, try again when there's more */
          return;
        }

        uint8_t frameData[MAX_FRAME_SIZE - FRAME_HEADER_SIZE - FRAME_FOOTER_SIZE] = { 0 };
        uint8_t frameDataSize = decoder->currentFrameSize - FRAME_HEADER_SIZE - FRAME_FOOTER_SIZE;

        uint32_t frameDataBytesRead = grinbuf_read(&decoder->ringbuf, frameData, frameDataSize);
        
        if (frameDataBytesRead != frameDataSize) {
          /* This should never happen */
          return;
        }

        uint8_t frameFooter[FRAME_FOOTER_SIZE] = { 0 };
        uint32_t frameFooterBytesRead = grinbuf_read(&decoder->ringbuf, frameFooter, FRAME_FOOTER_SIZE);

        if (frameFooterBytesRead != FRAME_FOOTER_SIZE) {
          /* This should never happen */
          return;
        }
        
        uint16_t decodedCRC = 0;
        decodedCRC |= (frameFooter[0] << 0);
        decodedCRC |= (frameFooter[1] << 8);

        uint16_t calculatedCRC = crc_modbus(frameData, frameDataSize);

        if (decodedCRC != calculatedCRC) {
          reset_frame(decoder);
          return;
        }
        fp_decoder_decode(frameData, frameDataSize, decoder->callback);
        reset_frame(decoder);
      }
      break;
      
      default: {
        decoder->state = FS_DECODER_STATE_FRAME_PARSE_DATA;
      }
      break;
    }
  }
}
