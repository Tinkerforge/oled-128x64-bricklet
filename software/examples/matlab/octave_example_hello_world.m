function octave_example_hello_world()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change to your UID

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    oled = java_new("com.tinkerforge.BrickletOLED128x64", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Clear display
    oled.clearDisplay();

    % Write "Hello World" starting from upper left corner of the screen
    oled.writeLine(0, 0, "Hello World");

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end
