using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change to your UID
	private static int SCREEN_WIDTH = 128;
	private static int SCREEN_HEIGHT = 64;

	private static void DrawMatrix(BrickletOLED128x64 oled, bool[][] pixels)
	{
		byte[][] column = new byte[SCREEN_HEIGHT/8][];
		byte[] columnWrite = new byte[64];

		for (int i = 0; i < SCREEN_HEIGHT/8; i++)
		{
			column[i] = new byte[SCREEN_WIDTH];
		}

		for (int i = 0; i < SCREEN_HEIGHT/8; i++)
		{
			for (int j = 0; j < SCREEN_WIDTH; j++)
			{
				byte page = 0;

				for (int k = 0; k < 8; k++)
				{
					if (pixels[(i*8)+k][j] == true)
					{
						page |= (byte)(1 << k);
					}
				}
				column[i][j] = page;
			}
		}
		oled.NewWindow(0, (byte)(SCREEN_WIDTH-1), 0, 7);

		int l = 0;
		for (int i = 0; i < SCREEN_HEIGHT/8; i++)
		{
			l = 0;
			for (int j = 0; j < SCREEN_WIDTH/2; j++)
			{
				columnWrite[l] = column[i][j];
				l++;
			}
			oled.Write(columnWrite);
			l = 0;
			for (int k = SCREEN_WIDTH/2; k < SCREEN_WIDTH; k++)
			{
				columnWrite[l] = column[i][k];
				l++;
			}
			oled.Write(columnWrite);
		}
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletOLED128x64 oled = new BrickletOLED128x64(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Clear display
		oled.ClearDisplay();

		// Pixel matrix with all pixels turned off
		bool[][] pixelMatrix = new bool[SCREEN_HEIGHT][];

		for (int i = 0; i < SCREEN_HEIGHT; i++)
		{
			pixelMatrix[i] = new bool[SCREEN_WIDTH];

			for (int j = 0; j < SCREEN_WIDTH; j++)
			{
				pixelMatrix[i][j] = false;
			}
		}

		// Draw check pattern
		for (int w = 0; w < SCREEN_WIDTH; w++)
		{
			for (int h = 0; h < SCREEN_HEIGHT; h++)
			{
				if (w/5 % 2 == 0)
				{
					pixelMatrix[h][w] = true;
				}
				if (h/5 % 2 == 0)
				{
					pixelMatrix[h][w] = true;
				}
			}
		}
		DrawMatrix(oled, pixelMatrix);

		System.Console.WriteLine("Press enter to exit");
		System.Console.ReadLine();
		ipcon.Disconnect();
	}
}
