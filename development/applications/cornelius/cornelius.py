import sys
import os
import signal
import serial.tools.list_ports

import remi.gui as gui
from remi.gui import *
from remi import start, App

# from udevice import *

sys.path.append(os.path.abspath("/Users/dbell44/Workspace/FALCON/development/libraries/falcon_packet"))
from falcon_packet import *
from ff_encoder import *

global_gui_instance = None
albus = None
import os
import time
import serial
from threading import Thread, Lock, Event

class SerialDevice:
    def __init__(self, serialPort, baudRate=115200, readSleepTimeMs=1):
        self.serialMutex = Lock()
        self.readSleepTime = readSleepTimeMs / 1000;
        self.serialInstance = serial.Serial(serialPort, baudRate, timeout=0.5)
        self.stopReadEvent = Event()
        self.stopReadEvent.clear()
        self.readThreadInstance = Thread(target=self.read_thread, args=[])
        self.readThreadInstance.start()

    def __del__(self):
        try:
            self.readThreadInstance.join()
        except:
            pass
        try:
            self.stop_read()
        except:
            pass

    def stop_read(self):
        self.stopReadEvent.set()

    def read_stopped(self):
        return self.stopReadEvent.is_set()

    def read_thread(self):
        while 1:
            if self.read_stopped():
                break
            buffer = bytes()
            while 1:
                readData = self.read_raw_char()
                if readData == bytes(): #empty bytes
                    self.read_callback(buffer)                    
                    break
                buffer += readData
            time.sleep(self.readSleepTime)

    def write_bytes(self, byteData):
        self.serialMutex.acquire()
        self.serialInstance.write(byteData)
        self.serialMutex.release()

    def write_string(self, string):
        self.write_bytes(string.encode('ascii'))

    def read_line(self):
        self.serialMutex.acquire()
        retval = None
        try:
            retval = self.serialInstance.readline().decode('ascii')
        except UnicodeDecodeError:
            pass
        self.serialMutex.release()
        return retval

    def read_raw_line(self):
        self.serialMutex.acquire()
        retval = self.serialInstance.readline()
        self.serialMutex.release()
        return retval

    def read_raw_char(self):
        self.serialMutex.acquire()
        readval = self.serialInstance.read(1)
        self.serialMutex.release()
        return readval

    def read_callback(self):
        # User should overwrite this in their own class
        pass

class Albus(SerialDevice):
    def init_frame_encoder(self):
        self.encoder = FF_Encoder()

    def read_callback(self, byteData):
        # global global_gui_instance
        # try:
        #     # global_gui_instance.serial_monitor_append(string)
        # except:
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
    albus = Albus(serialPort, 115200, 1)
    albus.init_frame_encoder()

#    start(Cornelius)
    while(1):
        time.sleep(0.001)
        albus.send_test_query(214)
        print("adams gay")


main()


