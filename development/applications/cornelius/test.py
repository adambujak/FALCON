import curses
import time
from threading import Thread, Lock
import serial

serialInstance = serial.Serial('/dev/tty.usbmodem14303', 115200, timeout=0.5)
serialInstance.flush()
serialInstance.reset_input_buffer()
serialInstance.reset_output_buffer()

print(bytes(2))
serialInstance.write(bytes(2))
readval = serialInstance.read(1)
print(readval)

serialInstance.write(bytes(2))
serialInstance.write(bytes(2))
print(readval)

serialInstance.write(bytes(2))
readval = serialInstance.read(1)
readval = serialInstance.read(1)
print(readval)

serialInstance.close()


