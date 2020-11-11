import pylink
import os
import sys

def __main__():
    print("Running falcon flash!")
    try:
        binaryFile = (sys.argv)[1]
        startAddress = int((sys.argv)[2], 16)
    except:
        print("File to flash or address not specified!")
        return

    jlinkSerialID = None
    try:
        jlinkSerialID = (sys.argv)[3]
    except:
        pass

    jlink = pylink.JLink()

    jlink.open(jlinkSerialID)
    jlink.connect('STM32F412ZG')

    jlink.flash_file(path=binaryFile, addr=startAddress, on_progress=None, power_on=False)
    jlink.reset()
    print("Flashed!")

__main__()
