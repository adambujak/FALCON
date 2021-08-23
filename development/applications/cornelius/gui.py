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
        serial_monitor_print('Attitude values set to: {}, {}, {}'.format(values['attitude-kp'],values['attitude-ki'],values['attitude-kd']))
        cornelius.albus.send_att_params(values['attitude-kp'], values['attitude-ki'], values['attitude-kd'])
    if event == 'Altitude Submit':
        serial_monitor_print('Altitude values set to: {}, {}, {}'.format(values['altitude-hover'],values['altitude-kp'],values['altitude-kd']))
        cornelius.albus.send_alt_params(values['altitude-kp'], values['altitude-kd'], values['altitude-hover'])
    if event == 'Yaw Submit':
        serial_monitor_print('Yaw values set to: {}, {}'.format(values['yaw-kp'],values['yaw-kd']))
        cornelius.albus.send_yaw_params(values['yaw-kp'], values['yaw-kd'])

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

cornelius.main()

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
