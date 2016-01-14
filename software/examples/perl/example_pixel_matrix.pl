#!/usr/bin/perl

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletOLED128x64;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change to your UID
use constant WIDTH => 128;
use constant HEIGHT => 64;

sub draw_matrix
{
    my ($oled, $pixels_ref) = @_;
    my @pixels = @{$pixels_ref};
    my @pages = ();

    foreach my $row (0..&HEIGHT / 8 - 1) {
        $pages[$row] = ();

        foreach my $column (0..&WIDTH - 1) {
            $pages[$row][$column] = 0;

            foreach my $bit (0..7) {
                if ($pixels[($row * 8) + $bit][$column]) {
                    $pages[$row][$column] |= 1 << $bit;
                }
            }
        }
    }

    $oled->new_window(0, &WIDTH - 1, 0, &HEIGHT / 8 - 1);

    foreach my $row (0..&HEIGHT / 8 - 1) {
        for (my $column = 0; $column < &WIDTH; $column += 64) {
            my @section = @{$pages[$row]}[$column..$column + 64];

            $oled->write(\@section);
        }
    }
}

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $oled = Tinkerforge::BrickletOLED128x64->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Clear display
$oled->clear_display();

# Draw checkerboard pattern
my @pixels = ();

foreach my $row (0..&HEIGHT - 1) {
    $pixels[$row] = ();

    foreach my $column (0..&WIDTH - 1) {
        $pixels[$row][$column] = (($row / 8) % 2) == (($column / 8) % 2);
    }
}

draw_matrix($oled, \@pixels);

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
