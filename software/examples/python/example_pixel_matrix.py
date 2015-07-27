#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID
SCREEN_WIDTH = 128
SCREEN_HEIGHT = 64

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_oled_128x64 import OLED128x64

def draw_matrix(pixels):
    row_index = 0
    row = []

    for i in range(8):
        for j in range(SCREEN_WIDTH):
            page = 0
            for k in range(8):
                if pixels[i*8 + k][j] == 1:
                    page |= 1 << k

            if len(row) <= row_index:
                row.append([]) 

            row[row_index].append(page)
            if len(row[row_index]) == SCREEN_HEIGHT:
                row_index += 1

        oled.new_window(0, SCREEN_WIDTH-1, 0, 7)

        for i in range(len(row)):
            oled.write(row[i])

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    oled = OLED128x64(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Clear display
    oled.clear_display()
    
    # Boolean matrix with all pixels turned off
    pixel_matrix = [[False]*SCREEN_WIDTH for i in range(SCREEN_HEIGHT)]

    # Draw check pattern
    for w in range(SCREEN_WIDTH):
        for h in range(SCREEN_HEIGHT):
            if w/5 % 2 == 0:
                pixel_matrix[h][w] = True
            if h/5 % 2 == 0:
                pixel_matrix[h][w] = True
        
    draw_matrix(pixel_matrix)

    raw_input('Press key to exit\n') # Use input() in Python 3
    ipcon.disconnect()
