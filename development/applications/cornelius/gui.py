import time
import signal
import sys
import PySimpleGUIQt as sg

import numpy as np

import cornelius

def handle_button_event(event):
    if event == 'Test-Query':
        serial_monitor_print('test-query sent')
        cornelius.albus.send_test_query()

    if event == 'Radio-Stats':
        serial_monitor_print('radio-stats query sent')
        cornelius.albus.send_radio_stats_query()

    if event == 'Idle':
        serial_monitor_print('Idle Mode Command')
        cornelius.albus.send_fcs_mode(1)

    if event == 'Calibrate':
        serial_monitor_print('Calibrate Command')
        cornelius.albus.send_calibration_command();

    if event == 'Ready':
        serial_monitor_print('Ready Mode Command')
        cornelius.albus.send_fcs_mode(3)

    if event == 'Fly':
        serial_monitor_print('Fly Mode Command')
        cornelius.albus.send_fcs_mode(4)

    if event == 'Attitude Submit':
        serial_monitor_print('Attitude values set to: {}, {}, {}'.format(float(values['attitude-kp']),float(values['attitude-ki']),float(values['attitude-kd'])))
        cornelius.albus.send_att_params(float(values['attitude-kp']), float(values['attitude-ki']), float(values['attitude-kd']))
    if event == 'Altitude Submit':
        serial_monitor_print('Altitude values set to: {}, {}, {}'.format(float(values['altitude-hover']),float(values['altitude-kp']),float(values['altitude-kd'])))
        cornelius.albus.send_alt_params(float(values['altitude-kp']), float(values['altitude-kd']), float(values['altitude-hover']))
    if event == 'Yaw Submit':
        serial_monitor_print('Yaw values set to: {}, {}'.format(float(values['yaw-kp']),float(values['yaw-kd'])))
        cornelius.albus.send_yaw_params(float(values['yaw-kp']), float(values['yaw-kd']))

def signal_handler(sig, frame):
    sys.exit(0)

def serial_monitor_print(text):
    monitor = window['serial-monitor']
    monitor.print(text)

signal.signal(signal.SIGINT, signal_handler)


# Serial Monitor
serial_monitor = sg.Multiline(size=(50, 10), key='serial-monitor')

# Spin Boxes
att_kp_input = sg.Input(key='attitude-kp',size=(5,1), default_text='3.60')
att_ki_input = sg.Input(key='attitude-ki',size=(5,1), default_text='0.20')
att_kd_input = sg.Input(key='attitude-kd',size=(5,1), default_text='0.18')
alt_kp_input = sg.Input(key='altitude-kp',size=(5,1), default_text='0.00')
alt_kd_input = sg.Input(key='altitude-kd',size=(5,1), default_text='0.00')
alt_kh_input = sg.Input(key='altitude-hover',size=(5,1), default_text='1.00')
yaw_kp_input = sg.Input(key='yaw-kp',size=(5,1), default_text='0.00')
yaw_kd_input = sg.Input(key='yaw-kd',size=(5,1), default_text='0.00')

layout = [ [sg.Text('Cornelius')],
           [sg.Button('Test-Query'),sg.Button('Radio-Stats')],
           [sg.Button('Idle',size=(9,1))],
           [sg.Button('Calibrate',size=(9,1))],
           [sg.Button('Ready',size=(9,1))],
           [sg.Button('Fly',size=(9,1))],
           [sg.Text('Attitude Control')],
           [sg.Text('Kp'), att_kp_input, sg.Text('Ki'), att_ki_input, sg.Text('Kd'), att_kd_input],
           [sg.Button('Attitude Submit',size=(9,1))],
           [sg.Text('Altitude Control')],
           [sg.Text('Hover'), alt_kh_input, sg.Text('Kp'), alt_kp_input, sg.Text('Kd'), alt_kd_input],
           [sg.Button('Altitude Submit',size=(9,1))],
           [sg.Text('Yaw Control')],
           [sg.Text('Kp'), yaw_kp_input, sg.Text('Kd'), yaw_kd_input],
           [sg.Button('Yaw Submit',size=(9,1))],
           [serial_monitor]
         ]

def update_alt_values(kh, kp, kd):
    alt_kh_input.Update(value=kh)
    alt_kp_input.Update(value=kp)
    alt_kd_input.Update(value=kd)

def update_att_values(kp, ki, kd):    
    att_kp_input.Update(value=kp)
    att_ki_input.Update(value=ki)
    att_kd_input.Update(value=kd)

def update_yaw_values(kp, kd):
    yaw_kp_input.Update(value=kp)
    yaw_kd_input.Update(value=kd)

cornelius.main(update_att_values, update_alt_values, update_yaw_values)

# Create the window
window = sg.Window("Cornelius", layout)

# Create an event loop
while True:
    event, values = window.read()
    # End program if user closes window or
    # presses the OK button
    if event == sg.WIN_CLOSED:
        print("win close pressed")
        cornelius.quit()
        break

    handle_button_event(event)
