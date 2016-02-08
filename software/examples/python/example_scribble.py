#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID
WIDTH = 128 # Columns (each 1 pixel wide)
HEIGHT = 8 # Rows (each 8 pixels high)

import math
import time
from PIL import Image, ImageDraw
from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_oled_128x64 import BrickletOLED128x64

def draw_image(oled, start_column, start_row, column_count, row_count, image):
    image_data = image.load()
    pages = []

    # Convert image pixels into 8bit pages
    for row in range(row_count):
        pages.append([])

        for column in range(column_count):
            page = 0

            for bit in range(8):
                if image_data[column, (row * 8) + bit]:
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

    # Draw rotating line
    image = Image.new("1", (WIDTH, HEIGHT * 8), 0)
    draw = ImageDraw.Draw(image)
    origin_x = WIDTH // 2
    origin_y = HEIGHT * 8 // 2
    length = HEIGHT * 8 // 2 - 2
    angle = 0

    print("Press ctrl+c to exit")

    try:
        while True:
            radians = math.pi * angle / 180.0
            x = (int)(origin_x + length * math.cos(radians))
            y = (int)(origin_y + length * math.sin(radians))

            draw.rectangle((0, 0, WIDTH, HEIGHT * 8), 0, 0)
            draw.line((origin_x, origin_y, x, y), 1, 1)

            draw_image(oled, 0, 0, WIDTH, HEIGHT, image)
            time.sleep(0.025)

            angle += 1
    except KeyboardInterrupt:
        pass

    ipcon.disconnect()
