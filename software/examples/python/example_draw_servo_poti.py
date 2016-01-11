#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
# This example uses a Servo Brick, a Rotary Poti Bricklet and
# a OLED 128x64 Bricklet.
#
# The position of the Rotary Poti Bricklet is drawn on the OLED
# display (as text, as a bar graph and as a dial indicator).
# At the same time the servo moves to an angle that is equivalent
# of the position of the Rotary Poti Bricklet.
#

HOST = "localhost"
PORT = 4223
SCREEN_WIDTH = 128
SCREEN_HEIGHT = 64

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_oled_128x64 import BrickletOLED128x64
from tinkerforge.bricklet_rotary_poti import BrickletRotaryPoti
from tinkerforge.brick_servo import BrickServo

from PIL import Image, ImageDraw, ImageFont

import sys
import math
import time

def draw_matrix(pixels):
    column_index = 0
    column = []

    for i in range(SCREEN_HEIGHT//8 - 1): # We use last 8 pixels for text
        for j in range(SCREEN_WIDTH):
            page = 0
            for k in range(8):
                if pixels[i*8 + k][j] == 1:
                    page |= 1 << k

            if len(column) <= column_index:
                column.append([])

            column[column_index].append(page)
            if len(column[column_index]) == SCREEN_HEIGHT:
                column_index += 1

        oled.new_window(0, SCREEN_WIDTH-1, 0, 6)

    for i in range(len(column)):
        oled.write(column[i])

def line_at_angle(startx, starty, angle, length):
   radian_angle = math.pi * angle / 180.0
   x = startx + length * math.cos(radian_angle)
   y = starty + length * math.sin(radian_angle)
   return (startx, starty, x, y)

if __name__ == "__main__":
    # Create Brick/Bricklet objects
    ipcon = IPConnection()
    oled = BrickletOLED128x64('x5U', ipcon)
    poti = BrickletRotaryPoti('8Co', ipcon)
    servo = BrickServo('6e8MF1', ipcon)

    # Connect to brickd
    ipcon.connect(HOST, PORT)

    # Configure Servo so that it rotates 180°
    servo.set_pulse_width(6, 650, 2350)
    servo.enable(6)

    # Clear display
    oled.clear_display()

    # Draw text once at beginning, the window in draw_matrix does not overwrite
    # the last line of the display
    oled.write_line(7, 5, "tinkerforge.com")

    # We just use an endless loop here, press ctrl + c to abort the script
    while True:
        # With position 110 poti is at 90°
        angle = poti.get_position()*90//110
        if angle > 90:
            angle = 90
        elif angle < -90:
            angle = -90

        # Set servo position according to angle
        servo.set_position(6, angle*100)

        # Create angle text
        angle_str = str(angle) + u'°'
        if angle >= 0:
            angle_str = ' ' + angle_str

        # Draw servo position line
        img = Image.new('1', (128, 64), 0)
        draw = ImageDraw.Draw(img)
        draw.line(line_at_angle(32, 32, angle - 90, 32), 1, 6)

        # Draw bar graph
        draw.line((90, 4, 90 + angle*30//90, 4), 1, 6)

        # Draw angle text
        font = ImageFont.truetype("./share/fonts/truetype/dejavu/DejaVuSans.ttf", 25)
        draw.text((70, 22), angle_str, font=font, fill=1)

        # Move data from PIL image into matrix of bools
        data = img.load()
        pixel_matrix = [[False]*SCREEN_WIDTH for i in range(SCREEN_HEIGHT)]
        for x in range(SCREEN_WIDTH):
            for y in range(SCREEN_HEIGHT):
                pixel_matrix[y][x] = data[x, y] == 1

        # Draw everything to display
        draw_matrix(pixel_matrix)

        # Framerate of ~40 FPS
        time.sleep(0.04)

    ipcon.disconnect()
