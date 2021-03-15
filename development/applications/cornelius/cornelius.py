import sys
import os
sys.path.append(os.path.abspath("../../libraries/falcon_packet"))
from falcon_packet import *

kwargs = {
	'yaw': 1.9,
	'pitch': 2.4,
	'roll': 3.6,
	'alt': 4.8
}

fcsControlInput = ft_fcs_control_input_t(**kwargs)

kwargs = {'fcsControlCmd': fcsControlInput}

fcsControlPacket = fpc_flight_control_t(**kwargs)
data = bytearray(get_packet_size(fp_type_t.FPT_FLIGHT_CONTROL_COMMAND) + PACKET_HEADER_SIZE)
fcsControlPacket.encode(data)
print(''.join(("0x"+format(x, '02x') + ", ") for x in data))
