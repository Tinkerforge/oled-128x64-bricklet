<?php

if (!extension_loaded('gd')) {
    echo "Required gd extension is not available\n";
    exit;
}

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletOLED128x64.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletOLED128x64;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change to your UID
const WIDTH = 128; // Columns (each 1 pixel wide)
const HEIGHT = 8; // Rows (each 8 pixels high)

function drawImage($oled, $startColumn, $startRow, $columnCount, $rowCount, $image)
{
    $pages = array(array());

    // Convert image pixels into 8bit pages
    for ($row = 0; $row < $rowCount; $row++)
    {
        for ($column = 0; $column < $columnCount; $column++)
        {
            $pages[$row][$column] = 0;

            for ($bit = 0; $bit < 8; $bit++)
            {
                $index = imagecolorat($image, $column, ($row * 8) + $bit);
                $rgb = imagecolorsforindex($image, $index);

                if ($rgb['red'] > 0) // Assume a black/white image
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

// Draw rotating line
$image = imagecreate(WIDTH, HEIGHT * 8);
$black = imagecolorallocate($image, 0, 0, 0);
$white = imagecolorallocate($image, 255, 255, 255);
$originX = WIDTH / 2;
$originY = HEIGHT * 8 / 2;
$length = HEIGHT * 8 / 2 - 2;
$angle = 0;

echo "Press ctrl+c to exit\n";

while (true)
{
    $radians = M_PI * $angle / 180.0;
    $x = (int)($originX + $length * cos($radians));
    $y = (int)($originY + $length * sin($radians));

    imagefilledrectangle($image, 0, 0, WIDTH, HEIGHT * 8, $black);
    imageline($image, $originX, $originY, $x, $y, $white);

    drawImage($oled, 0, 0, WIDTH, HEIGHT, $image);
    usleep(25*1000);

    $angle++;
}

?>
