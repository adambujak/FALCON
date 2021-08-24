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

    def set_callbacks(self, update_att_values, update_alt_values, update_yaw_values):
        self.update_att_values = update_att_values
        self.update_alt_values = update_alt_values
        self.update_yaw_values = update_yaw_values

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
            None
        elif (packet_type == fp_type_t.FPT_ATTITUDE_PARAMS_COMMAND):
            att_params = fpc_attitude_params_t(encoded)
            self.update_att_values(round(att_params.fcsAttParams.PID_pitch_P,3), round(att_params.fcsAttParams.PID_pitch_roll_I,3), round(att_params.fcsAttParams.PID_pitch_D,3))
        elif (packet_type == fp_type_t.FPT_ALT_PARAMS_COMMAND):
            alt_params = fpc_alt_params_t(encoded)
            self.update_alt_values(round(alt_params.fcsAltParams.Alt_Hover_Const,3), round(alt_params.fcsAltParams.PID_alt_P,3), round(alt_params.fcsAltParams.PID_alt_D,3))
        elif (packet_type == fp_type_t.FPT_YAW_PARAMS_COMMAND):
            yaw_params =fpc_yaw_params_t(encoded)
            self.update_yaw_values(round(yaw_params.fcsYawParams.PID_yaw_P,3), round(yaw_params.fcsYawParams.PID_yaw_D,3))
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

    def send_alt_params(self, alt_Kp, alt_Kd, alt_Hover):
        kwargs = {
            'PID_alt_P' : alt_Kp,
            'PID_alt_D' : alt_Kd,
            'Alt_Hover_Const' : alt_Hover,
        }
        fcsParamInput = ft_fcs_alt_control_params_t(**kwargs)

        kwargs = {'fcsAltParams': fcsParamInput}
        fcsParamPacket = fpc_alt_params_t(**kwargs)
        self.write_packet(fcsParamPacket)

    def send_yaw_params(self, yaw_Kp, yaw_Kd):
        kwargs = {
            'PID_yaw_P' : yaw_Kp,
            'PID_yaw_D' : yaw_Kd,
        }
        fcsParamInput = ft_fcs_yaw_control_params_t(**kwargs)

        kwargs = {'fcsYawParams': fcsParamInput}
        fcsParamPacket = fpc_yaw_params_t(**kwargs)
        self.write_packet(fcsParamPacket)

    def send_fcs_mode(self, mode):
        _mode = fe_flight_mode_t(mode)
        kwargs = {'mode' : _mode}
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

def main(update_att_values, update_alt_values, update_yaw_values):

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

    albus.set_callbacks(update_att_values, update_alt_values, update_yaw_values)

    albus.init_frame_encoder()
