import struct
import libscrc
from falcon_packet import *

class FF_Encoder():
    def __init__(self):
        pass

    def write_frame_header(self, dest, size):
        struct.pack_into('<B', dest, 0, FRAME_DELIMITER1)
        struct.pack_into('<B', dest, 1, FRAME_DELIMITER2)
        struct.pack_into('<B', dest, 2, size)
        return FRAME_HEADER_SIZE

    def write_frame_footer(self, dest, offset, crc16):
        struct.pack_into('<B', dest, offset, crc16 & 0xFF)
        struct.pack_into('<B', dest, offset + 1, (crc16 >> 8) & 0xFF)
        return FRAME_FOOTER_SIZE

    def append_frame_packet(self, dest, offset, packet):
        encodedPacket = packet.encode()
        packetLength = len(encodedPacket)
        assert((offset + packetLength) < (MAX_FRAME_SIZE - FRAME_FOOTER_SIZE))
        dest[offset:offset+packetLength] = encodedPacket
        return packetLength

    def pack_packets_into_frame(self, packets):
        retbuf = bytearray(MAX_FRAME_SIZE)

        writeIndex = FRAME_HEADER_SIZE
        for packet in packets:
            writeIndex += self.append_frame_packet(retbuf, writeIndex, packet)


        crc16 = libscrc.modbus(retbuf[FRAME_HEADER_SIZE:writeIndex])
        writeIndex += self.write_frame_footer(retbuf, writeIndex, crc16)

        self.write_frame_header(retbuf, writeIndex)
        return retbuf

