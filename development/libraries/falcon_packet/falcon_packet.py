import struct
from enum import IntEnum

MAX_FRAME_SIZE = 96
MAX_PACKET_SIZE = 94
FRAME_DELIMITER1 = 81
FRAME_DELIMITER2 = 152
PACKET_DELIMITER1 = 167
PACKET_DELIMITER2 = 185
PACKET_HEADER_SIZE = 3
FRAME_HEADER_SIZE = 3
FRAME_FOOTER_SIZE = 2

class fp_type_t(IntEnum):
    FPT_MODE_COMMAND = 0
    FPT_MODE_QUERY = 1
    FPT_MODE_RESPONSE = 2
    FPT_FLIGHT_CONTROL_COMMAND = 3
    FPT_STATUS_RESPONSE = 4
    FPT_CNT = 5

packet_size_lookup_dict = {
    fp_type_t.FPT_MODE_COMMAND: 1,
    fp_type_t.FPT_MODE_QUERY: 1,
    fp_type_t.FPT_MODE_RESPONSE: 1,
    fp_type_t.FPT_FLIGHT_CONTROL_COMMAND: 16,
    fp_type_t.FPT_STATUS_RESPONSE: 57,
};

def get_packet_size(packetType):
   return packet_size_lookup_dict[packetType] + PACKET_HEADER_SIZE

def encode_header(dest, packetType, offset):
    struct.pack_into('<B', dest, offset, PACKET_DELIMITER1)
    struct.pack_into('<B', dest, offset + 1, PACKET_DELIMITER2)
    struct.pack_into('<B', dest, offset + 2, packetType)

class fe_falcon_mode_t(IntEnum):
    FE_FLIGHT_MODE_IDLE = 1
    FE_FLIGHT_MODE_CALIBRATING = 2
    FE_FLIGHT_MODE_FCS_READY = 3
    FE_FLIGHT_MODE_FLY = 4

class ft_motor_pwm_control_data_t:
    def __init__(self, encoded=None, offset=0, **kwargs):
        if encoded:
            self.decode(encoded, offset)
        else:
            self.motor1 = kwargs.get("motor1", 0)
            self.motor2 = kwargs.get("motor2", 0)
            self.motor3 = kwargs.get("motor3", 0)
            self.motor4 = kwargs.get("motor4", 0)

    def encode(self, dest, offset=0):
        struct.pack_into('<H', dest, offset + 0, self.motor1)
        struct.pack_into('<H', dest, offset + 2, self.motor2)
        struct.pack_into('<H', dest, offset + 4, self.motor3)
        struct.pack_into('<H', dest, offset + 6, self.motor4)


class ft_fcs_state_estimate_t:
    def __init__(self, encoded=None, offset=0, **kwargs):
        if encoded:
            self.decode(encoded, offset)
        else:
            self.x = kwargs.get("x", 0)
            self.y = kwargs.get("y", 0)
            self.z = kwargs.get("z", 0)
            self.dx = kwargs.get("dx", 0)
            self.dy = kwargs.get("dy", 0)
            self.dz = kwargs.get("dz", 0)
            self.yaw = kwargs.get("yaw", 0)
            self.pitch = kwargs.get("pitch", 0)
            self.roll = kwargs.get("roll", 0)
            self.p = kwargs.get("p", 0)
            self.q = kwargs.get("q", 0)
            self.r = kwargs.get("r", 0)

    def encode(self, dest, offset=0):
        struct.pack_into('<f', dest, offset + 0, self.x)
        struct.pack_into('<f', dest, offset + 4, self.y)
        struct.pack_into('<f', dest, offset + 8, self.z)
        struct.pack_into('<f', dest, offset + 12, self.dx)
        struct.pack_into('<f', dest, offset + 16, self.dy)
        struct.pack_into('<f', dest, offset + 20, self.dz)
        struct.pack_into('<f', dest, offset + 24, self.yaw)
        struct.pack_into('<f', dest, offset + 28, self.pitch)
        struct.pack_into('<f', dest, offset + 32, self.roll)
        struct.pack_into('<f', dest, offset + 36, self.p)
        struct.pack_into('<f', dest, offset + 40, self.q)
        struct.pack_into('<f', dest, offset + 44, self.r)


class ft_status_data_t:
    def __init__(self, encoded=None, offset=0, **kwargs):
        if encoded:
            self.decode(encoded, offset)
        else:
            self.mode = kwargs.get("mode", 0)
            self.motor = kwargs.get("motor", 0)
            self.states = kwargs.get("states", 0)

    def encode(self, dest, offset=0):
        self.mode.encode(dest, offset + 0)
        self.motor.encode(dest, offset + 1)
        self.states.encode(dest, offset + 9)


class ft_fcs_control_input_t:
    def __init__(self, encoded=None, offset=0, **kwargs):
        if encoded:
            self.decode(encoded, offset)
        else:
            self.yaw = kwargs.get("yaw", 0)
            self.pitch = kwargs.get("pitch", 0)
            self.roll = kwargs.get("roll", 0)
            self.alt = kwargs.get("alt", 0)

    def encode(self, dest, offset=0):
        struct.pack_into('<f', dest, offset + 0, self.yaw)
        struct.pack_into('<f', dest, offset + 4, self.pitch)
        struct.pack_into('<f', dest, offset + 8, self.roll)
        struct.pack_into('<f', dest, offset + 12, self.alt)


class fpc_mode_t:
    def __init__(self, encoded=None, offset=0, **kwargs):
        if encoded:
            self.decode(encoded, offset)
        else:
            self.mode = kwargs.get("mode", 0)

    def encode(self, offset=0):
        dest = bytearray(get_packet_size(fp_type_t.FPT_MODE_COMMAND) + 3)

        encode_header(dest, fp_type_t.FPT_MODE_COMMAND, 0)
        self.mode.encode(dest, offset + 3)
        return dest


class fpq_mode_t:
    def __init__(self, encoded=None, offset=0, **kwargs):
        if encoded:
            self.decode(encoded, offset)
        else:
            self.mode = kwargs.get("mode", 0)

    def encode(self, offset=0):
        dest = bytearray(get_packet_size(fp_type_t.FPT_MODE_QUERY) + 3)

        encode_header(dest, fp_type_t.FPT_MODE_QUERY, 0)
        self.mode.encode(dest, offset + 3)
        return dest


class fpr_mode_t:
    def __init__(self, encoded=None, offset=0, **kwargs):
        if encoded:
            self.decode(encoded, offset)
        else:
            self.mode = kwargs.get("mode", 0)

    def encode(self, offset=0):
        dest = bytearray(get_packet_size(fp_type_t.FPT_MODE_RESPONSE) + 3)

        encode_header(dest, fp_type_t.FPT_MODE_RESPONSE, 0)
        self.mode.encode(dest, offset + 3)
        return dest


class fpc_flight_control_t:
    def __init__(self, encoded=None, offset=0, **kwargs):
        if encoded:
            self.decode(encoded, offset)
        else:
            self.fcsControlCmd = kwargs.get("fcsControlCmd", 0)

    def encode(self, offset=0):
        dest = bytearray(get_packet_size(fp_type_t.FPT_FLIGHT_CONTROL_COMMAND) + 3)

        encode_header(dest, fp_type_t.FPT_FLIGHT_CONTROL_COMMAND, 0)
        self.fcsControlCmd.encode(dest, offset + 3)
        return dest


class fpr_status_t:
    def __init__(self, encoded=None, offset=0, **kwargs):
        if encoded:
            self.decode(encoded, offset)
        else:
            self.status = kwargs.get("status", 0)

    def encode(self, offset=0):
        dest = bytearray(get_packet_size(fp_type_t.FPT_STATUS_RESPONSE) + 3)

        encode_header(dest, fp_type_t.FPT_STATUS_RESPONSE, 0)
        self.status.encode(dest, offset + 3)
        return dest


