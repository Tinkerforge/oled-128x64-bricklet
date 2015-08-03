#!/bin/sh
# connects to localhost:4223 by default, use --host and --port to change it

# change to your UID
uid=XYZ

# clear display
./tinkerforge call oled-64x48-bricklet $uid clear-display

# write "Hello World" starting from upper left corner of the screen
./tinkerforge call oled-64x48-bricklet $uid write-line 0 0 "Hello World"
