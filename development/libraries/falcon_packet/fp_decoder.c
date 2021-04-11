#include "fp_decoder.h"

typedef enum {
  FP_DECODER_STATE_PACKET_PARSE_HEADER,
  FP_DECODER_STATE_PACKET_PARSE_DATA,
  FP_DECODER_STATE_CNT
} fp_decoder_state_t;

void fp_decoder_decode(uint8_t *buffer, uint32_t length, fp_decoder_cb_t callback)
{
  fp_decoder_state_t state = FP_DECODER_STATE_PACKET_PARSE_HEADER;
  fp_type_t packetType = FPT_CNT;
  uint8_t index = 0;

  while(true) {

    if (index >= length) {
      return;
    }

    switch (state) {

      case FP_DECODER_STATE_PACKET_PARSE_HEADER: {

        if (buffer[index] == PACKET_DELIMITER1 && buffer[index + 1] == PACKET_DELIMITER2) {
          packetType = buffer[index + 2];
          index += PACKET_HEADER_SIZE;
          state = FP_DECODER_STATE_PACKET_PARSE_DATA;
        }
        else {
          index++;
        }
      }
      break;

      case FP_DECODER_STATE_PACKET_PARSE_DATA: {

        uint8_t packetSize = fp_get_packet_length(packetType);
        callback(&buffer[index], packetType);

        index += packetSize;
        state = FP_DECODER_STATE_PACKET_PARSE_HEADER;
      }
      break;

      case FP_DECODER_STATE_CNT: {
        /* Something has gone wrong */
        return;
      }
    }
  }
}
