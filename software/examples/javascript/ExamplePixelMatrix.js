var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your OLED 128x64 Bricklet
var SCREEN_WIDTH = 128;
var SCREEN_HEIGHT = 64;

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var oled = new Tinkerforge.BrickletOLED128x64(UID, ipcon); // Create device object

function drawMatrix(oled, pixels) {
    column = [];
    column_write = [];

    for (var i = 0; i < SCREEN_HEIGHT/8; i++) {
        column[i] = [];
    }

    for (var i = 0; i < SCREEN_HEIGHT/8; i++) {
        for (var j = 0; j < SCREEN_WIDTH; j++) {
            page = 0;

            for (var k = 0; k < 8; k++) {
                if (pixels[i*8 + k][j]) {
                    page |= 1 << k;
                }
            }

            column[i][j] = page;
        }
    }

    oled.newWindow(0, SCREEN_WIDTH-1, 0, 7)

    for (var i = 0; i < SCREEN_HEIGHT/8; i++) {
        l = 0;
        for (j = 0; j < SCREEN_WIDTH/2; j++) {
            column_write[l] = column[i][j];
            l++;
        }

        oled.write(column_write);

        l = 0;
        for (k = SCREEN_WIDTH/2; k < SCREEN_WIDTH; k++) {
            column_write[l] = column[i][k];
            l++;
        }

        oled.write(column_write);
    }
}

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Clear display
        oled.clearDisplay();

        // Draw checkerboard pattern
        var pixelMatrix = [];
        for (var h = 0; h < SCREEN_HEIGHT; h++) {
            pixelMatrix[h] = [];
            for (var w = 0; w < SCREEN_WIDTH; w++) {
                pixelMatrix[h][w] = Math.floor(h / 8) % 2 == Math.floor(w / 8) % 2;
            }
        }

        drawMatrix(oled, pixelMatrix);
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
