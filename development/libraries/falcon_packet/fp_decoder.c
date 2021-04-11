#include "fp_decoder.h"

void fp_decoder_decode(uint8_t *buffer, uint32_t length, fp_decoder_cb_t callback)
{  fp_type_t packetType = FPT_CNT;
  uint8_t index = 0;

  while(true) {

    if (index >= length) {
      return;
    }

    if (buffer[index] == PACKET_DELIMITER1 && buffer[index + 1] == PACKET_DELIMITER2) {
      packetType = buffer[index + 2];
      uint8_t packetSize = fp_get_packet_length(packetType);
      callback(&buffer[index], packetType);

      index += packetSize + PACKET_HEADER_SIZE;
    }
    else {
      index++;
    }
  }
}
