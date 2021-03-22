import sys
import serial.tools.list_ports
import pdb

from threading import Thread, Lock
from udevice import *

sys.path.append(os.path.abspath("../../libraries/falcon_packet"))
from falcon_packet import *
from ff_encoder import *

class Albus(SerialDevice):
    def init_frame_encoder(self):
        self.encoder = FF_Encoder()

    def read_callback(self, byteData):
        print(byteData.decode('ascii'), end="")

    def write_packet(self, packet):
        frame = self.encoder.pack_packets_into_frame([packet])
        self.write_bytes(frame)

    def send_control(self, yaw, pitch, roll, alt):
        kwargs = {
            'yaw': yaw,
            'pitch': pitch,
            'roll': roll,
            'alt': alt
        }
        fcsControlInput = ft_fcs_control_input_t(**kwargs)

        kwargs = {'fcsControlCmd': fcsControlInput}
        fcsControlPacket = fpc_flight_control_t(**kwargs)
        self.write_packet(fcsControlPacket)

ports = serial.tools.list_ports.comports()

# Delete bluetooth port
for i in range(len(ports)):
    if "Bluetooth" in ports[i].name:
        ports.pop(i)
        break


if len(ports) == 1:
    selectedPort = 0
    print("Using port: {}".format(ports[0].name))

else:
    for i in range(len(ports)):
        print("{}: {}".format(i+1, ports[i].name))

    print("Which serial port do you want to use?")
    selectedPort = int(input()) - 1

serialPort = ports[selectedPort].device

albus = Albus(serialPort)
albus.init_frame_encoder()
albus.send_control(1.2, 1.5, 1.6, 1.8)

