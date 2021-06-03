import time
import signal
import sys
import PySimpleGUIQt as sg

import numpy as np

import os
import time
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
        self.bufferFlushCount = 96

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
            data_buffer = bytes()
            while 1:
                readData = self.read_raw_char()
                if readData == bytes(): #empty bytes
                    # only send if not empty
                    if data_buffer != bytes():
                        self.read_callback(data_buffer)
                    break
                data_buffer += readData
                if len(data_buffer) > self.bufferFlushCount:
                    self.read_callback(data_buffer[0:self.bufferFlushCount])
                    data_buffer = data_buffer[self.bufferFlushCount:]
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
        elif (packet_type == fp_type_t.FPT_CALIBRATE_RESPONSE):
            calib_response = fpr_calibrate_t(encoded)
            if (calib_response.calib == fe_calib_request_t.FE_CALIBRATE_SUCCESS):
                print("Sensors Calibrating")
            else:
                print("Calibration Canceled, Check Mode")
        elif (packet_type == fp_type_t.FPT_STATUS_RESPONSE):
            # status = fpr_status_t(encoded)
            # print(status.to_dict())
            None
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

    def send_att_params(self, att_Kp, att_Ki, att_Kd):
        kwargs = {
            'PID_pitch_P' : att_Kp,
            'PID_pitch_roll_I' : att_Ki,
            'PID_pitch_D' : att_Kd,
        }
        fcsParamInput = ft_fcs_att_control_params_t(**kwargs)

        kwargs = {'fcsAttParams': fcsParamInput}
        fcsParamPacket = fpc_attitude_params_t(**kwargs)
        self.write_packet(fcsParamPacket)

    def send_fcs_mode(self, mode):
        kwargs = {'mode' : mode}
        fcsModePacket = fpc_fcs_mode_t(**kwargs)
        self.write_packet(fcsModePacket)

    def send_calibration_command(self):
        kwargs = {'calib' : fe_calib_request_t.FE_CALIBRATE_SUCCESS}
        sensCalibCmd = fpc_calibrate_t(**kwargs)
        self.write_packet(sensCalibCmd)

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

    global albus

    connected = False
    ports = serial.tools.list_ports.comports()

    for i in range(len(ports)):
        if ports[i].product == "STM32 STLink":
            albus = Albus(ports[i].device, 115200, 1)
            connected = True
            print("Connected to Albus")
            break

    if connected == False:
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

        albus = Albus(serialPort, 115200, 1)

    albus.init_frame_encoder()

def handle_button_event(event):
    if event == 'Test-Query':
        serial_monitor_print('test-query sent')
        albus.send_test_query()

    if event == 'Radio-Stats':
        serial_monitor_print('radio-stats query sent')
        albus.send_radio_stats_query()

    if event == 'Idle':
        serial_monitor_print('Idle Mode Command')
        albus.send_fcs_mode(fe_flight_mode_t(1))

    if event == 'Calibrate':
        serial_monitor_print('Calibrate Command')
        albus.send_calibration_command();

    if event == 'Ready':
        serial_monitor_print('Ready Mode Command')
        albus.send_fcs_mode(fe_flight_mode_t(3))

    if event == 'Fly':
        serial_monitor_print('Fly Mode Command')
        albus.send_fcs_mode(fe_flight_mode_t(4))

    if event == 'Attitude Submit':
        serial_monitor_print('Attitude values set to: {}, {}, {}'.format(values['attitude-kp'],values['attitude-ki'],values['attitude-kd']))
        albus.send_att_params(values['attitude-kp'], values['attitude-ki'], values['attitude-kd'])
    if event == 'Altitude Submit':
        serial_monitor_print('Altitude values set to: {}, {}, {}'.format(values['altitude-hover'],values['altitude-kp'],values['altitude-kd']))
    if event == 'Yaw Submit':
        serial_monitor_print('Yaw values set to: {}, {}'.format(values['yaw-kp'],values['yaw-kd']))

def signal_handler(sig, frame):
    sys.exit(0)

def serial_monitor_print(text):
    monitor = window['serial-monitor']
    monitor.print(text)

signal.signal(signal.SIGINT, signal_handler)


# Serial Monitor
serial_monitor = sg.Multiline(size=(50, 10), key='serial-monitor')

# Spin Boxes
att_kp_spin = sg.Spin(key='attitude-kp',size=(5,1),values=np.arange(0.00,10.00,0.01), initial_value=3.60)
att_ki_spin = sg.Spin(key='attitude-ki',size=(5,1),values=np.arange(0.00,10.00,0.01), initial_value=0.20)
att_kd_spin = sg.Spin(key='attitude-kd',size=(5,1),values=np.arange(0.00,10.00,0.01), initial_value=0.18)
alt_kp_spin = sg.Spin(key='altitude-kp',size=(5,1),values=np.arange(0.00,10.00,0.01), initial_value=0.00)
alt_kd_spin = sg.Spin(key='altitude-kd',size=(5,1),values=np.arange(0.00,10.00,0.01), initial_value=0.00)
alt_hover_spin = sg.Spin(key='altitude-hover',size=(5,1),values=np.arange(0.00,2.00,0.01), initial_value=1.00)
yaw_kp_spin = sg.Spin(key='yaw-kp',size=(5,1),values=np.arange(0.00,10.00,0.01), initial_value=0.00)
yaw_kd_spin = sg.Spin(key='yaw-kd',size=(5,1),values=np.arange(0.00,10.00,0.01), initial_value=0.00)

layout = [ [sg.Text('Cornelius')],
           [sg.Button('Test-Query'),sg.Button('Radio-Stats')],
           [sg.Button('Idle',size=(9,1))],
           [sg.Button('Calibrate',size=(9,1))],
           [sg.Button('Ready',size=(9,1))],
           [sg.Button('Fly',size=(9,1))],
           [sg.Text('Attitude Control')],
           [sg.Text('Kp'), att_kp_spin, sg.Text('Ki'), att_ki_spin, sg.Text('Kd'), att_kd_spin],
           [sg.Button('Attitude Submit',size=(9,1))],
           [sg.Text('Altitude Control')], 
           [sg.Text('Hover'), alt_hover_spin, sg.Text('Kp'), alt_kp_spin, sg.Text('kd'), alt_kd_spin],
           [sg.Button('Altitude Submit',size=(9,1))],
           [sg.Text('Yaw Control')],
           [sg.Text('kp'), yaw_kp_spin, sg.Text('kd'), yaw_kd_spin],
           [sg.Button('Yaw Submit',size=(9,1))],
           [serial_monitor]
         ]

main()

# Create the window
window = sg.Window("Cornelius", layout)

# Create an event loop
while True:
    event, values = window.read()
    # End program if user closes window or
    # presses the OK button
    if event == sg.WIN_CLOSED:
        print("win close pressed")
        quit()
        break

    handle_button_event(event)
