Procedure

OS - Windows 7

1. Install Python 2.7.12 - use all default settings
2. Add C:\Python27 into PC Environmental System's PATH
3. Install pyserial 2.7 using all default paths
4. Install pygame 1.9.1 for python 2.7 using default settings
5. Connect the Invensense board through to the PC through a COM port
6. Find the specific COM port number either through the "Device Manager" if direct connect from UART to PC, or the BlueTooth Devices if using an CA-SDK BLE connection.
7. Use the python client that came with the software driver stack. The python client is different with CA-SDK, Motion Driver 5.1.3, Motion Driver 6.1.2, and eMD 20X48
8. on a command prompt, go to the Invensense pycube client folder. 
9. execute the python through command 
	5.1.3 = "python motion-driver-client.py <COM PORT NUM>"
	6.1.2 = "python eMPL=client.py <COM PORT NUM>"
	CA-SDK = "python eMA511-client.py <COM PORT NUM>"
	206X48 = "python motion-driver-client.py <COM PORT NUM>"
10. You should see the pycube up and running. 