import time
import signal
import sys
import PySimpleGUIQt as sg

def handle_button_event(event):
    if event == 'test-query':
        print ('test-query')

    if event == 'radio-stats':
        print ('radio')

    if event == 'idle':
        print ('idle')

    if event == 'calibrate':
        print ('calibrate')

    if event == 'ready':
        print ('ready')

    if event == 'fly':
        print ('fly')

    if event == 'yaw-submit':
        yaw_kp = window['yaw-kp']
        serial_monitor_print('yaw value:' + str(yaw_kp.get()))

def signal_handler(sig, frame):
    sys.exit(0)

def serial_monitor_print(text):
    monitor = window['serial-monitor']
    monitor.print(text)

signal.signal(signal.SIGINT, signal_handler)

multiline = sg.Multiline(size=(30, 5), key='serial-monitor')

layout = [ [sg.Text('Cornelius')],
           [sg.Button('test-query')],
           [sg.Button('radio-stats')],
           [sg.Button('idle')],
           [sg.Button('calibrate')],
           [sg.Button('ready')],
           [sg.Button('fly')],
           [sg.Text('attitude control')],
           [sg.Text('kp'), sg.Input(key='attitude-kp'), sg.Text('ki'), sg.Input(key='attitude-ki'), sg.Text('kd'), sg.Input(key='attitude-kd')],
           [sg.Button('attitude-submit')],
           [sg.Text('altitude control')],
           [sg.Text('hover'), sg.Input(key='altitude-hover'), sg.Text('kp'), sg.Input(key='altitude-kp'), sg.Text('kd'), sg.Input(key='altitude-kd')],
           [sg.Button('altitude-submit')],
           [sg.Text('yaw control')],
           [sg.Text('kp'), sg.Input(key='yaw-kp'), sg.Text('kd'), sg.Input(key='yaw-kd')],
           [sg.Button('yaw-submit')],
           [multiline]
         ]

# Create the window
window = sg.Window("Cornelius", layout)
print(window)

# Create an event loop
while True:
    event, values = window.read()
    # End program if user closes window or
    # presses the OK button
    if event == sg.WIN_CLOSED:
        print("win close pressed")
        break

    handle_button_event(event)

    serial_monitor_print("hello")

