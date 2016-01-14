<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletOLED128x64.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletOLED128x64;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change to your UID
const WIDTH = 128;
const HEIGHT = 64;

function drawMatrix($oled, $pixels)
{
    $pages = array(array());

    for ($row = 0; $row < HEIGHT / 8; $row++)
    {
        for ($column = 0; $column < WIDTH; $column++)
        {
            $pages[$row][$column] = 0;

            for ($bit = 0; $bit < 8; $bit++)
            {
                if ($pixels[($row * 8) + $bit][$column])
                {
                    $pages[$row][$column] |= 1 << $bit;
                }
            }
        }
    }

    $oled->newWindow(0, WIDTH - 1, 0, HEIGHT / 8 - 1);

    for ($row = 0; $row < HEIGHT / 8; $row++)
    {
        for ($column = 0; $column < WIDTH; $column += 64)
        {
            $oled->write(array_slice($pages[$row], $column, 64));
        }
    }
}

$ipcon = new IPConnection(); // Create IP connection
$oled = new BrickletOLED128x64(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Clear display
$oled->clearDisplay();

// Draw checkerboard pattern
$pixels = array(array());

for ($row = 0; $row < HEIGHT; $row++)
{
    for ($column = 0; $column < WIDTH; $column++)
    {
        $pixels[$row][$column] = ($row / 8) % 2 == ($column / 8) % 2;
    }
}

drawMatrix($oled, $pixels);

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
