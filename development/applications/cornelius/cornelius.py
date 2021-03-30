import sys
import signal
import serial.tools.list_ports

from udevice import *

sys.path.append(os.path.abspath("../../libraries/falcon_packet"))
from falcon_packet import *
from ff_encoder import *

albus = None

class Albus(SerialDevice):
    def init_frame_encoder(self):
        self.encoder = FF_Encoder()

    def read_callback(self, byteData):
        print(byteData.decode('utf-8'), end="")

    def write_packet(self, packet):
        frame = self.encoder.pack_packets_into_frame([packet])
        print("writing!!!!!!!", len(frame))
        self.write_bytes(frame)
        time.sleep(0.001)

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

def signal_handler(sig, frame):
    global albus
    print("\nexit\n")
    albus.stop_read()
    del albus
    exit(0)

def main():
    signal.signal(signal.SIGINT, signal_handler)
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

    global albus
    albus = Albus(serialPort, 115200)

    albus.init_frame_encoder()

    while(1):
        time.sleep(1)
        albus.send_control(1.2, 1.5, 1.6, 1.8)


main()


