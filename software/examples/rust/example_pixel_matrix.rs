use std::{error::Error, io};

use tinkerforge::{ip_connection::IpConnection, oled_128x64_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your OLED 128x64 Bricklet
const WIDTH: usize = 128;
const HEIGHT: usize = 64;

fn draw_matrix(oled: &Oled128x64Bricklet, pixels: [[bool; WIDTH]; HEIGHT]) {
    let mut pages = [[0u8; WIDTH]; HEIGHT / 8];
    for (col_idx, col) in pages.iter_mut().enumerate() {
        for (row_idx, byte) in col.iter_mut().enumerate() {
            for bit in 0..8 {
                if pixels[col_idx * 8 + bit][row_idx] {
                    *byte |= 1 << bit;
                }
            }
        }
    }

    oled.new_window(0, (WIDTH - 1) as u8, 0, (HEIGHT / 8 - 1) as u8);

    for row in 0..HEIGHT / 8 {
        for col in (0..128).step_by(64) {
            let mut arr = [0u8; 64];
            arr.copy_from_slice(&pages[row][col..col + 64]);
            oled.write(arr);
        }
    }
}

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let oled = Oled128x64Bricklet::new(UID, &ipcon); // Create device object

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd
                                          // Don't use device before ipcon is connected

    // Clear display
    oled.clear_display();

    // Draw checkerboard pattern
    let mut pixels = [[false; WIDTH]; HEIGHT];
    for (row_idx, row) in pixels.iter_mut().enumerate() {
        for (col_idx, pixel) in row.iter_mut().enumerate() {
            *pixel = (row_idx / 8) % 2 == (col_idx / 8) % 2;
        }
    }

    draw_matrix(&oled, pixels);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
