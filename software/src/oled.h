/* oled-128x64-bricklet
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * oled.h: Implementation of OLED 64x48 Bricklet messages
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef OLED_H
#define OLED_H

#include <stdint.h>

#include "bricklib/com/com_common.h"

#define USE_SPI

#define PIXEL_W 128
#define PIXEL_H 64
#define PIXEL_COL_START 0
#define MAX_CHARS_W 26
#define MAX_CHARS_H 8

#define CONTROL_CO (1 << 7)
#define CONTROL_DC (1 << 6)

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

void invocation(const ComType com, const uint8_t *data);
void constructor(void);
void destructor(void);
void tick(const uint8_t tick_type);

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) StandardMessage;


#define FID_WRITE 1
#define FID_NEW_WINDOW 2
#define FID_CLEAR_WINDOW 3
#define FID_SET_DISPLAY_CONFIGURATION 4
#define FID_GET_DISPLAY_CONFIGURATION 5
#define FID_WRITE_LINE 6

typedef struct {
	MessageHeader header;
	char data[64];
} __attribute__((__packed__)) Write;

typedef struct {
	MessageHeader header;
	uint8_t column_from;
	uint8_t column_to;
	uint8_t row_from;
	uint8_t row_to;
} __attribute__((__packed__)) NewWindow;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) ClearWindow;

typedef struct {
	MessageHeader header;
	uint8_t contrast;
	bool invert;
} __attribute__((__packed__)) SetDisplayConfiguration;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetDisplayConfiguration;

typedef struct {
	MessageHeader header;
	uint8_t contrast;
	bool invert;
} __attribute__((__packed__)) GetDisplayConfigurationReturn;

typedef struct {
	MessageHeader header;
	uint8_t line;
	uint8_t position;
	char text[13];
} __attribute__((__packed__)) WriteLine;

void write(const ComType com, const Write *data);
void new_window(const ComType com, const NewWindow *data);
void clear_window(const ComType com, const ClearWindow *data);
void set_display_configuration(const ComType com, const SetDisplayConfiguration *data);
void get_display_configuration(const ComType com, const GetDisplayConfiguration *data);
void write_line(const ComType com, const WriteLine *data);

void apply_window(void);
void write_char(uint8_t x, uint8_t y, char c);
void ssd1306_data(const uint8_t *data, const uint8_t length);
void ssd1306_command(const uint8_t *command, const uint8_t length);

#ifdef USE_SPI
void spibb_send_byte(const uint8_t value);
#endif

#ifdef USE_I2C
bool i2c_scl_value(void);
void i2c_scl_high(void);
void i2c_scl_low(void);
bool i2c_sda_value(void);
void i2c_sda_high(void);
void i2c_sda_low(void);
void i2c_sleep_halfclock(void);
void i2c_stop(void);
void i2c_start(void);
uint8_t i2c_recv_byte(bool ack);
bool i2c_send_byte(const uint8_t value);
#endif

#endif
