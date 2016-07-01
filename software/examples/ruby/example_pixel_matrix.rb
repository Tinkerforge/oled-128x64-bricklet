#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_oled_128x64'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your OLED 128x64 Bricklet
WIDTH = 128 # Columns (each 1 pixel wide)
HEIGHT = 8 # Rows (each 8 pixels high)

def draw_matrix(oled, start_column, start_row, column_count, row_count, pixels)
  pages = []

  # Convert pixel matrix into 8bit pages
  for row in 0..row_count - 1
    pages[row] = []

    for column in 0..column_count - 1
      pages[row][column] = 0

      for bit in 0..7
        if pixels[row * 8 + bit][column]
          pages[row][column] |= 1 << bit
        end
      end
    end
  end

  # Merge page matrix into single page array
  data = []

  for row in 0..row_count - 1
    for column in 0..column_count - 1
      data.push pages[row][column]
    end
  end

  # Set new window
  oled.new_window start_column, start_column + column_count - 1, \
                  start_row, start_row + row_count - 1

  # Write page data in 64 byte blocks
  for i in (0..data.length - 1).step(64)
    block = data[i, 64]
    oled.write block.fill(0, block.length, 64 - block.length)
  end
end

ipcon = IPConnection.new # Create IP connection
oled = BrickletOLED128x64.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Clear display
oled.clear_display

# Draw checkerboard pattern
pixels = []

for row in 0..HEIGHT * 8 - 1
  pixels[row] = []

  for column in 0..WIDTH - 1
    pixels[row][column] = (row / 8) % 2 == (column / 8) % 2
  end
end

draw_matrix oled, 0, 0, WIDTH, HEIGHT, pixels

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
