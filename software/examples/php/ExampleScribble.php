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
const WIDTH = 128;
const HEIGHT = 64;

function drawImage($oled, $image)
{
    $pages = array(array());

    for ($row = 0; $row < HEIGHT / 8; $row++)
    {
        for ($column = 0; $column < WIDTH; $column++)
        {
            $pages[$row][$column] = 0;

            for ($bit = 0; $bit < 8; $bit++)
            {
                $index = imagecolorat($image, $column, ($row * 8) + $bit);
                $rgb = imagecolorsforindex($image, $index);

                if ($rgb['red'] > 0)
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

// Draw rotating line
$image = imagecreate(WIDTH, HEIGHT);
$black = imagecolorallocate($image, 0, 0, 0);
$white = imagecolorallocate($image, 255, 255, 255);
$originX = WIDTH / 2;
$originY = HEIGHT / 2;
$length = HEIGHT / 2 - 2;
$angle = 0;

echo "Press ctrl+c to exit\n";

while (true)
{
    $radians = M_PI * $angle / 180.0;
    $x = (int)($originX + $length * cos($radians));
    $y = (int)($originY + $length * sin($radians));

    imagefilledrectangle($image, 0, 0, WIDTH, HEIGHT, $black);
    imageline($image, $originX, $originY, $x, $y, $white);

    drawImage($oled, $image);
    usleep(25*1000);

    $angle++;
}

?>
