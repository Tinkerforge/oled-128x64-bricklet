import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletOLED128x64;

public class ExamplePixelMatrix {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your OLED 128x64 Bricklet
	private static final String UID = "XYZ";
	private static final short SCREEN_WIDTH = 128;
	private static final short SCREEN_HEIGHT = 64;

	static void drawMatrix(BrickletOLED128x64 oled, boolean[][] pixels) throws Exception {
		short[][] column = new short[SCREEN_HEIGHT/8][SCREEN_WIDTH];
		short[] columnWrite = new short[64];
		short page = 0;
		short i, j, k, l;

		for (i = 0; i < SCREEN_HEIGHT/8; i++) {
			for (j = 0; j < SCREEN_WIDTH; j++) {
				page = 0;

				for (k = 0; k < 8; k++) {
					if (pixels[(i*8)+k][j]) {
						page |= (short)(1 << k);
					}
				}

				column[i][j] = page;
			}
		}

		oled.newWindow((short)0, (short)(SCREEN_WIDTH-1), (short)0, (short)7);

		for (i = 0; i < SCREEN_HEIGHT/8; i++) {
			l = 0;
			for (j = 0; j < SCREEN_WIDTH/2; j++) {
				columnWrite[l] = column[i][j];
				l++;
			}

			oled.write(columnWrite);

			l = 0;
			for (k = SCREEN_WIDTH/2; k < SCREEN_WIDTH; k++) {
				columnWrite[l] = column[i][k];
				l++;
			}

			oled.write(columnWrite);
		}
	}

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletOLED128x64 oled = new BrickletOLED128x64(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Clear display
		oled.clearDisplay();

		// Draw checkerboard pattern
		boolean[][] pixelMatrix = new boolean[SCREEN_HEIGHT][SCREEN_WIDTH];

		for (short h = 0; h < SCREEN_HEIGHT; h++) {
			for (short w = 0; w < SCREEN_WIDTH; w++) {
				pixelMatrix[h][w] = (h / 8) % 2 == (w / 8) % 2;
			}
		}

		drawMatrix(oled, pixelMatrix);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
