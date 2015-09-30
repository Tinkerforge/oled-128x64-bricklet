#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change to your UID

# Clear display
tinkerforge call oled-128x64-bricklet $uid clear-display

# Write "Hello World" starting from upper left corner of the screen
tinkerforge call oled-128x64-bricklet $uid write-line 0 0 "Hello World"
