function matlab_example_hello_world()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletOLED128x64;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your OLED 128x64 Bricklet

    ipcon = IPConnection(); % Create IP connection
    oled = handle(BrickletOLED128x64(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Clear display
    oled.clearDisplay();

    % Write "Hello World" starting from upper left corner of the screen
    oled.writeLine(0, 0, 'Hello World');

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end
