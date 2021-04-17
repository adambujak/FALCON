import sys
import os
import time
import signal
import serial
import serial.tools.list_ports
from threading import Thread, Lock, Event

sys.path.append(os.path.abspath("../../libraries/falcon_packet"))
from falcon_packet import *
from ff_encoder import *
from fs_decoder import *

albus = None

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
        maxsize = 31
        while len(byteData) > maxsize:
            self.serialInstance.write(byteData[0:maxsize])
            byteData = byteData[maxsize:]
            time.sleep(0.001)

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
        self.decoder = FS_Decoder(self.decoder_callback)

    def decoder_callback(self, encoded, packet_type):
        if (packet_type == fp_type_t.FPT_TEST_RESPONSE):
            test_response = fpr_test_t(encoded)
            print("received: fpr_test_t ", test_response.to_dict())
        elif (packet_type == fp_type_t.FPT_RADIO_STATS_RESPONSE):
            radio_stats = fpr_radio_stats_t(encoded)
            print("received: radio stats ", radio_stats.to_dict())
        else:
            print("decoder callback:", packet_type)

    def read_callback(self, byteData):
        self.decoder.decode(byteData)

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

    def send_mode(self, mode):
        # fcsModeInput = fe_falcon_mode_t(mode)

        kwargs = {'mode' : mode}
        fcsModePacket = fpc_fcs_mode_t(**kwargs)
        self.write_packet(fcsModePacket)

    def send_test_query(self):
        kwargs = {}
        test_query = fpq_test_t(**kwargs)
        self.write_packet(test_query)

    def send_radio_stats_query(self):
        kwargs = {}
        radio_query = fpq_radio_stats_t(**kwargs)
        self.write_packet(radio_query)


def quit():
    global albus
    albus.stop_read()
    del albus
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

    while(1):
        user_input = input("enter command: ")
        if user_input == "s":
            print("sending test query")
            albus.send_test_query()
        elif user_input == "r":
            print("sending radio stats query")
            albus.send_radio_stats_query()
        elif user_input == "m":            
            mode_input = fe_flight_mode_t(int(input("enter mode: ")))
            albus.send_mode(mode_input)
        elif user_input == "q":
            print("quit")
            quit()


main()
