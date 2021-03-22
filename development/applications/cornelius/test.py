import curses
import time
from threading import Thread, Lock
from math import *
from mokuskaui import *

class SerialMonitorWindow(ScrollWindow):
    def input_handler(self, key):
        if key == ord('r'):
            self.scroll_reset()

        if key == ord('k'):
            self.scroll_up()

        if key == ord('j'):
            self.scroll_down()

        if key == ord('g'):
            self.scroll_home()

class DeviceManagerUI(MokuskaUI):
    def input_handler(self, key):
        pass

    def start(self):
        serialMonitorWidth = int(self.width * 0.5)
        self.screen.addstr(1, self.width - serialMonitorWidth//2 - 9, "Serial Monitor")
        self.serialMonitorWindow = SerialMonitorWindow((self.width - serialMonitorWidth), 2, serialMonitorWidth-1, self.height-3)
        self.add_element(self.serialMonitorWindow)

    def serial_monitor_append(self, string):
        self.serialMonitorWindow.write_string(string)


ui = DeviceManagerUI()

count = 0
while(True):
    ui.serial_monitor_append("hello{}".format(count))
    count += 1
    time.sleep(.1)

