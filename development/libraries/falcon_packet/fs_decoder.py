import struct
import libscrc
from falcon_packet import *

from enum import Enum
class DecoderParseState(Enum):
    HEADER = 0
    BODY = 1

class FP_Decoder():
    def __init__(self, callback):
        self.callback = callback

    def decode(self, encoded):
        packetType = fp_type_t.FPT_CNT

        while True:
            if len(encoded) < PACKET_HEADER_SIZE:
                return

            if (encoded[0] == PACKET_DELIMITER1 and encoded[1] == PACKET_DELIMITER2):
                packetType = fp_type_t(encoded[2])
                packetSize = get_packet_size(packetType)
                self.callback(encoded[0:packetSize], packetType)

                # drop used bytes
                encoded = encoded[PACKET_HEADER_SIZE+packetSize:]
            else:
                # drop 1 byte
                encoded = encoded[1:]

class FS_Decoder():
    def __init__(self, callback):
        self.encoded = bytearray()
        self.state = DecoderParseState.HEADER
        self.currentFrameSize = 0
        self.callback = callback
        self.decoder = FP_Decoder(callback)

    def _decode(self):
        while True:
            if self.state == DecoderParseState.HEADER:
                if len(self.encoded) < FRAME_HEADER_SIZE:
                    return

                if self.encoded[0] != FRAME_DELIMITER1:
                    self.encoded = self.encoded[1:]

                if self.encoded[1] != FRAME_DELIMITER2:
                    # only pop one byte off in case this is start of frame
                    self.encoded = self.encoded[1:]

                self.currentFrameSize = self.encoded[2]
                self.state = DecoderParseState.BODY
                self.encoded = self.encoded[FRAME_HEADER_SIZE:]

            if self.state == DecoderParseState.BODY:
                if len(self.encoded) < (self.currentFrameSize - FRAME_HEADER_SIZE):
                    return

                frameDataSize = self.currentFrameSize - FRAME_HEADER_SIZE - FRAME_FOOTER_SIZE
                frameData = self.encoded[0:frameDataSize]
                self.encoded = self.encoded[frameDataSize:]

                frameFooterSize = FRAME_FOOTER_SIZE
                frameFooter = self.encoded[0:frameFooterSize]
                self.encoded = self.encoded[frameFooterSize:]

                decodedCRC = 0
                decodedCRC |= (frameFooter[0] << 0)
                decodedCRC |= (frameFooter[1] << 8)

                calculatedCRC = libscrc.modbus(frameData)

                if decodedCRC != calculatedCRC:
                    self.reset()
                    return

                self.decoder.decode(frameData)
                self.reset()

    def decode(self, encoded):
        self.encoded = self.encoded + encoded
        self._decode()

    def reset(self):
        self.encoded = bytearray()
        self.state = DecoderParseState.HEADER
        self.currentFrameSize = 0


