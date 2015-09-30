#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_oled_128x64.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change to your UID
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

void draw_matrix(OLED128x64 *oled, bool (*pixels)[SCREEN_WIDTH]) {
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	uint8_t page = 0;
	uint8_t column_write[64];
	uint8_t column[SCREEN_HEIGHT/8][SCREEN_WIDTH];

	for (i = 0; i < SCREEN_HEIGHT/8; i++) {
		for (j = 0; j < SCREEN_WIDTH; j++) {
			page = 0;

			for (k = 0; k < 8; k++) {
				if (pixels[(i*8) + k][j] == true) {
					page |= (1 << k);
				}
			}
			column[i][j] = page;
		}
	}

	oled_128x64_new_window(oled, 0, SCREEN_WIDTH-1, 0, 7);

	for (i = 0; i < SCREEN_HEIGHT/8; i++) {
		l = 0;
		for (j = 0; j < SCREEN_WIDTH/2; j++) {
			column_write[l] = column[i][j];
			l++;
		}

		oled_128x64_write(oled, column_write);

		l = 0;
		for (k = SCREEN_WIDTH/2; k < SCREEN_WIDTH; k++) {
			column_write[l] = column[i][k];
			l++;
		}

		oled_128x64_write(oled, column_write);
	}
}

int main(void) {
	// Create IP connection
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	OLED128x64 oled;
	oled_128x64_create(&oled, UID, &ipcon);

	int i = 0;
	int j = 0;
	int w = 0;
	int h = 0;
	bool pixel_matrix[SCREEN_HEIGHT][SCREEN_WIDTH];

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		return 1;
	}
	// Don't use device before ipcon is connected

	// Clear display
	oled_128x64_clear_display(&oled);

	// Pixel matrix with all pixels turned off
	for (i = 0; i < SCREEN_HEIGHT; i++) {
		for (j = 0; j < SCREEN_WIDTH; j++) {
			pixel_matrix[i][j] = false;
		}
	}

	// Draw check pattern
	for (w = 0; w < SCREEN_WIDTH; w++) {
		for (h = 0; h < SCREEN_HEIGHT; h++) {
			if ((w/5) % 2 == 0) {
				pixel_matrix[h][w] = true;
			}

			if ((h/5) % 2 == 0) {
				pixel_matrix[h][w] = true;
			}
		}
	}

	draw_matrix(&oled, pixel_matrix);

	printf("Press key to exit\n");
	getchar();
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
	return 0;
}
