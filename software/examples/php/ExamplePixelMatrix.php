<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletOLED128x64.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletOLED128x64;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change to your UID
const SCREEN_WIDTH = 128;
const SCREEN_HEIGHT = 64;

function drawMatrix($oled, $pixels)
{
	$column = array(array());

	for ($i = 0; $i < SCREEN_HEIGHT/8; $i++)
	{
		for ($j = 0; $j < SCREEN_WIDTH; $j++)
		{
			$page = 0;

			for ($k = 0; $k < 8; $k++)
			{
				if ($pixels[($i*8) + $k][$j] == true)
				{
					$page |= 1 << $k;
				}
			}

			$column[$i][$j] = $page;
		}
	}
	$oled->newWindow(0, SCREEN_WIDTH-1, 0, 7);

	for ($i = 0; $i < SCREEN_HEIGHT/8; $i++)
	{
		$l = 0;
		for ($j = 0; $j < SCREEN_WIDTH/2; $j++) {
			$column_write[$l] = $column[$i][$j];
			$l++;
		}
		$oled->write($column_write);
		$l = 0;
		for ($k = SCREEN_WIDTH/2; $k < SCREEN_WIDTH; $k++) {
			$column_write[$l] = $column[$i][$k];
			$l++;
		}
		$oled->write($column_write);
	}
}

$ipcon = new IPConnection(); // Create IP connection
$oled = new BrickletOLED128x64(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Clear display
$oled->clearDisplay();

// Pixel matrix with all pixels turned off
$pixel_matrix = array(array());

for ($i = 0; $i < SCREEN_HEIGHT; $i++)
{
	for ($j = 0; $j < SCREEN_WIDTH; $j++)
	{
		$pixel_matrix[$i][$j] = false;
	}
}

# Draw check pattern
for ($w = 0; $w < SCREEN_WIDTH; $w++)
{
	for ($h = 0; $h < SCREEN_HEIGHT; $h++)
	{
		if ($w/5 % 2 == 0)
		{
			$pixel_matrix[$h][$w] = true;
		}
		if ($h/5 % 2 == 0)
		{
			$pixel_matrix[$h][$w] = true;
		}
	}
}

drawMatrix($oled,$pixel_matrix);

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
