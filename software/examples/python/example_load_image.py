#!/usr/bin/env python
# -*- coding: utf-8 -*-  

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID
SCREEN_WIDTH = 128
SCREEN_HEIGHT = 64

from PIL import Image
from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_oled_128x64 import BrickletOLED128x64, OLED128x64

def draw_matrix(pixels):
    column_index = 0
    column = []

    for i in range(SCREEN_HEIGHT//8):
        for j in range(SCREEN_WIDTH):
            page = 0

            for k in range(8):
                if pixels[i*8 + k][j] == True:
                    page |= 1 << k

            if len(column) <= column_index:
                column.append([]) 

            column[column_index].append(page)

            if len(column[column_index]) == SCREEN_HEIGHT:
                column_index += 1

        oled.new_window(0, SCREEN_WIDTH-1, 0, 7)

        for i in range(len(column)):
            oled.write(column[i])

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    oled = OLED128x64(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    image = Image.open("./tf_logo_128x64.png")
    width, height = image.size
    #data = [image.getpixel((x, y)) for x in range(width) for y in range(height)]

    # Clear display and setup window to draw from
    oled.clear_display()
    oled.new_window(0, 127, 0, 7)
    
    # Boolean matrix with all pixels turned off
    pixel_matrix = [[False]*SCREEN_WIDTH for i in range(SCREEN_HEIGHT)]
    
    for w in range(width):
        for h in range(height):
            if image.getpixel((w, h)) == 255:
                pixel_matrix[h][w] = True

    draw_matrix(pixel_matrix)

    raw_input('Press key to exit\n') # Use input() in Python 3
    ipcon.disconnect()
