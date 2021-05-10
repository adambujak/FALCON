import time
import PySimpleGUIQt as sg

multiline = sg.Multiline(size=(30, 5), key='textbox')

layout = [  [sg.Text('Welcome to Cornelius')],
            [sg.Button('Ok'), sg.Button('Close Window')],
            [multiline]]

print(multiline)
# Create the window
window = sg.Window("Cornelius", layout)
print(window)

# Create an event loop
while True:
    event, values = window.read()
    # End program if user closes window or
    # presses the OK button
    if event == "OK" or event == sg.WIN_CLOSED:
        print("ok pressed")
        break

    window['textbox'].print(values['textbox'] + 'ashdflkjasdfl; asljfsa lkfjdsal kjf asddfjkl ;as\nhello')
