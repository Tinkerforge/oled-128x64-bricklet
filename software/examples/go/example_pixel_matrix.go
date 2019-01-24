package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
	"github.com/Tinkerforge/go-api-bindings/oled_128x64_bricklet"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your OLED 128x64 Bricklet.

const WIDTH = 128
const HEIGHT = 64

func drawMatrix(oled oled_128x64_bricklet.OLED128x64Bricklet, pixels [HEIGHT][WIDTH]bool) {
	var pages [HEIGHT / 8][WIDTH]uint8
	for colIdx, col := range pages {
		for rowIdx := range col {
			for bit := 0; bit < 8; bit++ {
				if pixels[colIdx*8+bit][rowIdx] {
					pages[colIdx][rowIdx] |= 1 << uint(bit)
				}
			}
		}
	}

	oled.NewWindow(0, WIDTH-1, 0, (HEIGHT/8)-1)

	for row := 0; row < HEIGHT/8; row++ {
		for col := 0; col < WIDTH; col += 64 {
			var arr [64]uint8
			copy(arr[:], pages[row][col:col+64])
			oled.Write(arr)
		}
	}
}

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	oled, _ := oled_128x64_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	// Clear display
	oled.ClearDisplay()

	// Draw checkerboard pattern
	var pixels [HEIGHT][WIDTH]bool
	for rowIdx, row := range pixels {
		for colIdx := range row {
			pixels[rowIdx][colIdx] = (rowIdx/8)%2 == (colIdx/8)%2
		}
	}

	drawMatrix(oled, pixels)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()
}
