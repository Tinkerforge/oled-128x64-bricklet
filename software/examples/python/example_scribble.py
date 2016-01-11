#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID
WIDTH = 128
HEIGHT = 64

import math
import time
from PIL import Image, ImageDraw
from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_oled_128x64 import BrickletOLED128x64

def draw_image(oled, image):
    image_data = image.load()
    pages = []

    # Convert pixels into pages
    for row in range(HEIGHT // 8):
        pages.append([])

        for column in range(WIDTH):
            page = 0

            for bit in range(8):
                if image_data[column, (row * 8) + bit]:
                    page |= 1 << bit

            pages[row].append(page)

    # Write all pages
    oled.new_window(0, WIDTH - 1, 0, HEIGHT // 8 - 1)

    for row in range(HEIGHT // 8):
        for column in range(0, WIDTH, 64):
            oled.write(pages[row][column:column + 64])

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    oled = BrickletOLED128x64(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Clear display
    oled.clear_display()

    # Draw rotating line
    image = Image.new('1', (WIDTH, HEIGHT), 0)
    draw = ImageDraw.Draw(image)
    origin_x = WIDTH // 2
    origin_y = HEIGHT // 2
    length = HEIGHT // 2 - 2
    angle = 0

    print("Press ctrl+c to exit")

    try:
        while True:
            radians = math.pi * angle / 180.0
            x = (int)(origin_x + length * math.cos(radians))
            y = (int)(origin_y + length * math.sin(radians))

            draw.rectangle((0, 0, WIDTH, HEIGHT), 0, 0)
            draw.line((origin_x, origin_y, x, y), 1, 1)

            draw_image(oled, image)
            time.sleep(0.025)

            angle += 1
    except KeyboardInterrupt:
        pass

    ipcon.disconnect()
