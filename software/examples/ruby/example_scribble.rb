#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'rgd'
require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_oled_128x64'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change to your UID
WIDTH = 128 # Columns (each 1 pixel wide)
HEIGHT = 8 # Rows (each 8 pixels high)

def draw_image(oled, start_column, start_row, column_count, row_count, image)
  pages = []

  # Convert image pixels into 8bit pages
  for row in 0..row_count - 1
    pages[row] = []

    for column in 0..column_count - 1
      pages[row][column] = 0

      for bit in 0..7
        index = image[column, (row * 8) + bit]
        rgba = image.rgba index

        if rgba[0] > 0
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
image = RGD::Image.create WIDTH, HEIGHT * 8
black = image.color_allocate 0, 0, 0
white = image.color_allocate 255, 255, 255
origin_x = WIDTH / 2
origin_y = HEIGHT * 8 / 2
length = HEIGHT * 8 / 2 - 2
angle = 0

puts 'Press key to exit'

Thread.new do
  $stdin.gets
  exit
end

while true
    radians = Math::PI * angle / 180.0
    x = (origin_x + length * Math.cos(radians)).to_i
    y = (origin_y + length * Math.sin(radians)).to_i

    image.filled_rectangle 0, 0, WIDTH, HEIGHT * 8, black
    image.line origin_x, origin_y, x, y, white

    draw_image oled, 0, 0, WIDTH, HEIGHT, image
    sleep 0.025

    angle += 1
end
