<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletOLED128x64.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletOLED128x64;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change to your UID
const WIDTH = 128; // Columns (each 1 pixel wide)
const HEIGHT = 8; // Rows (each 8 pixels high)

function drawMatrix($oled, $startColumn, $startRow, $columnCount, $rowCount, $pixels)
{
    $pages = array(array());

    // Convert pixel matrix into 8bit pages
    for ($row = 0; $row < $rowCount; $row++)
    {
        for ($column = 0; $column < $columnCount; $column++)
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

    // Merge page matrix into single page array
    $data = array();

    for ($i = 0, $row = 0; $row < $rowCount; $row++)
    {
        for ($column = 0; $column < $columnCount; $column++, $i++)
        {
            $data[$i] = $pages[$row][$column];
        }
    }

    // Set new window
    $oled->newWindow($startColumn, $startColumn + $columnCount - 1,
                     $startRow, $startRow + $rowCount - 1);

    // Write page data in 64 byte blocks
    for ($i = 0; $i < count($data); $i += 64)
    {
        $oled->write(array_pad(array_slice($data, $i, 64), 64, 0));
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

for ($y = 0; $y < HEIGHT * 8; $y++)
{
    for ($x = 0; $x < WIDTH; $x++)
    {
        $pixels[$y][$x] = ($y / 8) % 2 == ($x / 8) % 2;
    }
}

drawMatrix($oled, 0, 0, WIDTH, HEIGHT, $pixels);

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
