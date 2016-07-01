#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your OLED 128x64 Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_oled_128x64 import BrickletOLED128x64

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    oled = BrickletOLED128x64(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Clear display
    oled.clear_display()

    # Write "Hello World" starting from upper left corner of the screen
    oled.write_line(0, 0, "Hello World")

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
