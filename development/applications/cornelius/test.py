import os
import sys
sys.path.append(os.path.abspath("../../libraries/falcon_packet"))
from falcon_packet import *
from ff_encoder import *
from fs_decoder import *

def decode_callback(encoded, packet_type):
    if (packet_type == fp_type_t.FPT_TEST_QUERY):
        test_query = fpq_test_t(encoded)
        print("test_query, ", test_query)

    if (packet_type == fp_type_t.FPT_FLIGHT_CONTROL_COMMAND):
        flight_command = fpc_flight_control_t(encoded)
        print("flight_command, ", flight_command)
        print("yaw, ",   flight_command.fcsControlCmd.yaw)
        print("pitch, ", flight_command.fcsControlCmd.pitch)
        print("roll, ",  flight_command.fcsControlCmd.roll)
        print("alt, ",   flight_command.fcsControlCmd.alt)


kwargs = {}
test_query = fpq_test_t(**kwargs)
print(test_query.encode())

kwargs = {'yaw': 1.2, 'pitch':2.4, 'roll':3.5, 'alt':4.5}

controlInputData = ft_fcs_control_input_t(**kwargs)

kwargs = {'fcsControlCmd': controlInputData}
flightControl = fpc_flight_control_t(**kwargs)
print(flightControl.fcsControlCmd.yaw)
print(flightControl.encode())
decoded = fpc_flight_control_t(encoded=flightControl.encode())
print(decoded.encode())
print(decoded.fcsControlCmd.yaw)
print(decoded.fcsControlCmd.pitch)
print(decoded.fcsControlCmd.roll)
print(decoded.fcsControlCmd.alt)
encoder = FF_Encoder()
decoder = FS_Decoder(decode_callback)
frame = encoder.pack_packets_into_frame([test_query, flightControl])
print(frame)
decoder.decode(frame)
