#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your OLED 128x64 Bricklet
WIDTH = 128 # Columns (each 1 pixel wide)
HEIGHT = 8 # Rows (each 8 pixels high)

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_oled_128x64 import BrickletOLED128x64

def draw_matrix(oled, start_column, start_row, column_count, row_count, pixels):
    pages = []

    # Convert pixel matrix into 8bit pages
    for row in range(row_count):
        pages.append([])

        for column in range(column_count):
            page = 0

            for bit in range(8):
                if pixels[(row * 8) + bit][column]:
                    page |= 1 << bit

            pages[row].append(page)

    # Merge page matrix into single page array
    data = []

    for row in range(row_count):
        for column in range(column_count):
            data.append(pages[row][column])

    # Set new window
    oled.new_window(start_column, start_column + column_count - 1,
                    start_row, start_row + row_count - 1)

    # Write page data in 64 byte blocks
    for i in range(0, len(data), 64):
        block = data[i:i + 64]
        oled.write(block + [0] * (64 - len(block)))

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    oled = BrickletOLED128x64(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Clear display
    oled.clear_display()

    # Draw checkerboard pattern
    pixels = []

    for row in range(HEIGHT * 8):
        pixels.append([])

        for column in range(WIDTH):
            pixels[row].append((row // 8) % 2 == (column // 8) % 2)

    draw_matrix(oled, 0, 0, WIDTH, HEIGHT, pixels)

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
