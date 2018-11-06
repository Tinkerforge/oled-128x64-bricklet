use std::{error::Error, io};

use tinkerforge::{ipconnection::IpConnection, oled_128x64_bricklet::*};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your OLED 128x64 Bricklet

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let oled_128x64_bricklet = OLED128x64Bricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    // Clear display
    oled_128x64_bricklet.clear_display();

    // Write "Hello World" starting from upper left corner of the screen
    oled_128x64_bricklet.write_line(0, 0, "Hello World".to_string());

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
