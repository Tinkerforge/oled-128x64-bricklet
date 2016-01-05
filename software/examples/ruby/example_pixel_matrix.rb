#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_oled_128x64'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change to your UID
SCREEN_WIDTH = 128
SCREEN_HEIGHT = 64

def draw_matrix(oled, pixels)
  column = []
  column_write = []

  for i in 0..SCREEN_HEIGHT/8 - 1
    column[i] = []

    for j in 0..SCREEN_WIDTH-1
      page = 0

      for k in 0..7
        if pixels[i*8 + k][j] == true
          page |= 1 << k
        end
      end

      column[i][j] = page
    end
  end

  oled.new_window 0, SCREEN_WIDTH-1, 0, 7

  for i in 0..SCREEN_HEIGHT/8 - 1
    l = 0
    for j in 0..(SCREEN_WIDTH/2) - 1
      column_write[l] = column[i][j]
      l = l + 1
    end

    oled.write column_write

    l = 0
    for k in SCREEN_WIDTH/2..(SCREEN_WIDTH/2) - 1
      column_write[l] = column[i][k]
      l = l + 1
    end

    oled.write column_write
  end
end

ipcon = IPConnection.new # Create IP connection
oled = BrickletOLED128x64.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Clear display
oled.clear_display

# Draw checkerboard pattern
pixel_matrix = []

for h in 0..SCREEN_HEIGHT-1
  pixel_matrix[i] = []

  for w in 0..SCREEN_WIDTH-1
    pixel_matrix[h][w] = (h / 8) % 2 == (w / 8) % 2
  end
end

draw_matrix oled, pixel_matrix

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
