#include "ff_encoder.h"
#include "fp_decode.h"
#include "fp_encode.h"
#include "checksum.h"

#include <stdlib.h>
#include <string.h>

#define FRAME_SIZE_BYTE_INDEX (FRAME_HEADER_SIZE-1)

void ff_encoder_init(ff_encoder_t *encoder)
{
  encoder->destBuffer = NULL;
  encoder->writeIndex = 0;
}

void ff_encoder_set_buffer(ff_encoder_t *encoder, uint8_t *buffer)
{
  encoder->destBuffer = buffer;
  buffer[0] = FRAME_DELIMITER1;
  buffer[1] = FRAME_DELIMITER2;
  buffer[2] = 0;
  encoder->writeIndex = FRAME_HEADER_SIZE;
}

int ff_encoder_append_packet(ff_encoder_t *encoder, void *packet, fp_type_t packetType)
{
  uint8_t packetLength = fp_get_packet_length(packetType) + PACKET_HEADER_SIZE;

  if ((encoder->writeIndex + packetLength) > (MAX_FRAME_SIZE - FRAME_FOOTER_SIZE)) {
    return -1;
  }

  uint8_t bytesWritten = fp_encode_packet(encoder->destBuffer + encoder->writeIndex, packet, packetType);
  if (bytesWritten == packetLength) {
    encoder->writeIndex += bytesWritten;
    return bytesWritten;
  }
  return -1;
}

int ff_encoder_append_data(ff_encoder_t *encoder, uint8_t *data, uint32_t length)
{
  if (length > MAX_PACKET_SIZE) {
    return -1;
  }
  if ((encoder->writeIndex + length) > (MAX_FRAME_SIZE - FRAME_FOOTER_SIZE)) {
    return -1;
  }

  memcpy(encoder->destBuffer + encoder->writeIndex, data, length);
  encoder->writeIndex += length;
  return length;
}

int ff_encoder_append_footer(ff_encoder_t *encoder)
{
  uint16_t crc = crc_modbus(&encoder->destBuffer[FRAME_HEADER_SIZE], encoder->writeIndex - FRAME_HEADER_SIZE);
  encoder->destBuffer[encoder->writeIndex] = (crc >> 0) & 0xFF;
  encoder->writeIndex++;
  encoder->destBuffer[encoder->writeIndex] = (crc >> 8) & 0xFF;
  encoder->writeIndex++;

  /* Write size byte */
  encoder->destBuffer[FRAME_SIZE_BYTE_INDEX] = encoder->writeIndex;
  return encoder->writeIndex;
}

uint32_t ff_encoder_get_remaining_bytes(ff_encoder_t *encoder)
{
  return MAX_FRAME_SIZE - encoder->writeIndex - FRAME_FOOTER_SIZE;
}

