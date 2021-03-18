from __future__ import division  #You don't need this in Python3
import curses
import time
from threading import Thread, Lock
from math import *

class ScrollWindow:
    def __init__(self, x, y, width, height):
        self.height = height
        self.width = width

        # subtract two for border
        self.rowCount = height - 2;

        self.window = curses.newwin(height, width, y, x)
        self.window.box()
        self.window.border()
        self.strings = []
        self.stringIndex = 0

    def write_string(self, string):
        # TODO: add splitting up of strings
        self.strings += [string]

    def scroll_up(self):
        pass#self.stringIndex += 1

    def scroll_down(self):
        pass#if self.stringIndex > 0:
        #    self.stringIndex -= 1

    def refresh(self):
        stringCount = len(self.strings)
        startIndex = 0
        endIndex = stringCount

        # if we have more text than the scrollwindow size,
        # only draw the most recent strings unless stringIndex is non-zero
        if stringCount > self.rowCount:

            if self.stringIndex == 0:
                startIndex = stringCount - self.rowCount

            else:
                startIndex = stringCount - (self.rowCount + self.stringIndex)
                endIndex = stringCount - self.stringIndex

        strings = self.strings[startIndex:endIndex]
        for index, string in enumerate(strings):
            f.write("s: {}, e: {}, c: {}, s: {}\n".format(startIndex, endIndex, index, string))
            self.window.addstr(index+1, 1, string)

        self.window.refresh()

    def input_handler(self, key):
        if key == ord('r'):
            self.stringIndex = 0

        if key == ord('k'):
            self.scroll_up()

        if key == ord('j'):
            self.scroll_down()


class GUI:
    def __init__(self):
        self.screen = curses.initscr()
        curses.noecho()
        curses.cbreak()
        curses.start_color()
        self.screen.keypad(1)

        curses.init_pair(1,curses.COLOR_BLACK, curses.COLOR_WHITE)
        highlightText = curses.color_pair(1)
        normalText = curses.A_NORMAL

        self.screen.border(0)
        curses.curs_set(0)

        self.serial_monitor_window = ScrollWindow(120, 0, 80, 10)

        self.draw_thread_instance = Thread(target=self.draw_thread, args=[])
        self.draw_thread_instance.start()

        self.input_thread_instance = Thread(target=self.input_thread, args=[])
        self.input_thread_instance.start()

    def refresh(self):
        self.screen.refresh()
        self.serial_monitor_window.refresh()

    def input_handler(self, key):
        self.serial_monitor_window.input_handler(key)

    def draw_thread(self):
        count = 0
        try:
            while(1):
                self.refresh()
                self.serial_monitor_window.write_string("hello"+str(count))
                count += 1
                time.sleep(0.5)
        except:
            curses.endwin()
            print("ERROR in draw thread")
            f.close()
            exit()

    def input_thread(self):
        try:
            while(1):
                key = self.screen.getch()
                self.input_handler(key)
                time.sleep(0.01)
        except:
            curses.endwin()
            print("ERROR in input thread")
            exit()


gui = GUI()
f = open('log.txt', 'w')
f.write("hello\n")


strings = [ "a", "b", "c", "d", "e", "f", "g", "h", "i", "l", "m", "n" ] #list of strings

#pages = int( ceil( row_num / max_row ) )
#position = 1
#page = 1
#for i in range( 1, max_row + 1 ):
#    if row_num == 0:
#        box.addstr( 1, 1, "There aren't strings", highlightText )
#    else:
#        if (i == position):
#            box.addstr( i, 2, str( i ) + " - " + strings[ i - 1 ], highlightText )
#        else:
#            box.addstr( i, 2, str( i ) + " - " + strings[ i - 1 ], normalText )
#        if i == row_num:
#            break
#

# key getting

#while key != 27:
#    #window
#    box.erase()
#    screen.border( 0 )
#    box.border( 0 )
#
#    for i in range( 1 + ( max_row * (page - 1) ), max_row + 1 + ( max_row * ( page - 1 ) ) ):
#        if ( i + ( max_row * ( page - 1 ) ) == position + ( max_row * ( page - 1 ) ) ):
#            box.addstr( i - ( max_row * ( page - 1 ) ), 2, str( i ) + " - " + strings[ i - 1 ], highlightText )
#        else:
#            box.addstr( i - ( max_row * ( page - 1 ) ), 2, str( i ) + " - " + strings[ i - 1 ], normalText )
#        if i == row_num:
#            break
#
#    screen.refresh()
#    box.refresh()
#    key = screen.getch()

