import sys
import os
import signal
import serial.tools.list_ports

import remi.gui as gui
from remi.gui import *
from remi import start, App

from udevice import *

sys.path.append(os.path.abspath("../../libraries/falcon_packet"))
from falcon_packet import *
from ff_encoder import *

global_gui_instance = None
albus = None

class Albus(SerialDevice):
    def init_frame_encoder(self):
        self.encoder = FF_Encoder()

    def read_callback(self, byteData):
        global global_gui_instance
        try:
            global_gui_instance.serial_monitor_append(string)
        except:
            print(byteData)

    def write_packet(self, packet):
        frame = self.encoder.pack_packets_into_frame([packet])
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

    def send_test_query(self, cookie):
        kwargs = {'cookie': cookie}
        test_query = fpq_test_t(**kwargs)
        self.write_packet(test_query)


class Cornelius(App):
    def __init__(self, *args):
        super(Cornelius, self).__init__(*args)

    def main(self):
        global global_gui_instance
        global_gui_instance = self

        self.main_container = Container(width=706, height=445, margin='0px auto', style="position: relative")

        self.serial_monitor_container = VBox(width=630, height=277, style='position: absolute; left: 40px; top: 150px; background-color: #b6b6b6')
        self.serial_monitor = TextInput(single_line=False, width=630, height=277)
        self.serial_monitor_container.append(self.serial_monitor, 'Serial Monitor')

        self.send_command_button = gui.Button('Send Command', width=200, height=30)
        self.send_command_button.onclick.do(self.on_send_button_pressed)

        self.main_container.append(self.serial_monitor_container,'Serial Monitor Container')
        self.main_container.append(self.send_command_button,'Send Command Button')

        return self.main_container

    def serial_monitor_append(self, text):
        if len(text) == 0:
            return
        try:
            all_text = self.serial_monitor.get_text() + text
            self.serial_monitor.set_text(all_text)
            if (text == '\n'):
                js_cmd = "document.getElementById('{}').scrollTop={}".format(self.serial_monitor.identifier, 9999)
                self.execute_javascript(js_cmd);
        except Exception as e:
            print('no serial', e)

    def on_send_button_pressed(self, widget):
        global albus
        albus.send_control(1.2, 1.8, 1.9, 4.5)

    def on_close(self):
        super(Cornelius, self).on_close()
        quit()

def quit():
    global albus
    global global_gui_instance
    print("\nexit\n")
    albus.stop_read()
    del albus
    del global_gui_instance
    exit(0)

def main():
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

#    start(Cornelius)
    while(1):
        time.sleep(1)
        albus.send_test_query(214)


main()


