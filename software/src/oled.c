/* oled-128x64-bricklet
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * oled.c: Implementation of OLED 64x48 Bricklet messages
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

#include "oled.h"

#include "brickletlib/bricklet_entry.h"
#include "bricklib/bricklet/bricklet_communication.h"
#include "bricklib/drivers/adc/adc.h"
#include "config.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/init.h"

#include "font.inc"

#define I2C_ADDRESS_SSD1306   0b111101
#define I2C_READ              1
#define I2C_WRITE             0
#define I2C_HALF_CLOCK_400KHZ 1250  // 2.5us per clock

const uint8_t init_conf[] = {
	1, SSD1306_DISPLAYOFF,
	2, SSD1306_SETDISPLAYCLOCKDIV,  0x80,
	2, SSD1306_SETMULTIPLEX,        0x3F,
	2, SSD1306_SETDISPLAYOFFSET,    0x00,
	1, SSD1306_SETSTARTLINE | 0x00,
	2, SSD1306_CHARGEPUMP,          0x14,
	2, SSD1306_MEMORYMODE,          0x00,
	1, SSD1306_SEGREMAP     | 0x01,
	1, SSD1306_COMSCANDEC,
	2, SSD1306_SETCOMPINS,          0x12,
	2, SSD1306_SETCONTRAST,         0x8F,
	2, SSD1306_SETPRECHARGE,        0xF1,
	2, SSD1306_SETVCOMDETECT,       0x40,
	1, SSD1306_DISPLAYALLON_RESUME,
	1, SSD1306_NORMALDISPLAY,
	1, SSD1306_DISPLAYON,
	3, SSD1306_COLUMNADDR,          PIXEL_COL_START, PIXEL_COL_START + PIXEL_W-1,
	3, SSD1306_PAGEADDR,            0, 7,
};


void invocation(const ComType com, const uint8_t *data) {
	switch(((MessageHeader*)data)->fid) {
		default: {
			case FID_WRITE: {
				write(com, (Write*)data);
				break;
			}

			case FID_NEW_WINDOW: {
				new_window(com, (NewWindow*)data);
				break;
			}

			case FID_CLEAR_WINDOW: {
				clear_window(com, (ClearWindow*)data);
				break;
			}

			case FID_SET_DISPLAY_CONFIGURATION: {
				set_display_configuration(com, (SetDisplayConfiguration*)data);
				break;
			}

			case FID_GET_DISPLAY_CONFIGURATION: {
				get_display_configuration(com, (GetDisplayConfiguration*)data);
				break;
			}

			case FID_WRITE_LINE: {
				write_line(com, (WriteLine*)data);
				break;
			}

			BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_NOT_SUPPORTED, com);
			break;
		}
	}
}

void constructor(void) {
	_Static_assert(sizeof(BrickContext) <= BRICKLET_CONTEXT_MAX_SIZE, "BrickContext too big");

#ifdef USE_I2C
	PIN_NRESET.type = PIO_OUTPUT_1;
	PIN_NRESET.attribute = PIO_DEFAULT;
    BA->PIO_Configure(&PIN_NRESET, 1);

    PIN_SA0.type = PIO_OUTPUT_1;
    PIN_SA0.attribute = PIO_DEFAULT;
    BA->PIO_Configure(&PIN_SA0, 1);

    PIN_SCL.type = PIO_INPUT;
    PIN_SCL.attribute = PIO_PULLUP;
    BA->PIO_Configure(&PIN_SCL, 1);

    PIN_SDA.type = PIO_INPUT;
    PIN_SDA.attribute = PIO_PULLUP;
    BA->PIO_Configure(&PIN_SDA, 1);
#endif

#ifdef USE_SPI
    PIN_CS.type = PIO_OUTPUT_1;
    PIN_CS.attribute = PIO_DEFAULT;
    BA->PIO_Configure(&PIN_CS, 1);

    PIN_DC.type = PIO_OUTPUT_1;
    PIN_DC.attribute = PIO_DEFAULT;
    BA->PIO_Configure(&PIN_DC, 1);

    PIN_CLK.type = PIO_OUTPUT_0;
    PIN_CLK.attribute = PIO_DEFAULT;
    BA->PIO_Configure(&PIN_CLK, 1);

    PIN_SDI.type = PIO_OUTPUT_0;
    PIN_SDI.attribute = PIO_DEFAULT;
    BA->PIO_Configure(&PIN_SDI, 1);
#endif

	BC->column_from       = 0;
	BC->column_to         = PIXEL_W-1;
	BC->row_from          = 0;
	BC->row_to            = PIXEL_H-1;
	BC->current_num       = 0;
	BC->contrast          = 0x8F;
	BC->invert            = false;
	BC->last_write_char_x = 254; // Initialize with unreachable value
	BC->last_write_char_y = 254;

#ifdef USE_I2C
    // Reset
    SLEEP_MS(1);
    PIN_NRESET.type = PIO_OUTPUT_0;
    BA->PIO_Configure(&PIN_NRESET, 1);
    SLEEP_MS(10);
	PIN_NRESET.type = PIO_OUTPUT_1;
    BA->PIO_Configure(&PIN_NRESET, 1);
#endif

#ifdef USE_SPI
    // Reset through NAND-gate between CS and SDI
    SLEEP_MS(1);
    PIN_SDI.type = PIO_OUTPUT_1;
    BA->PIO_Configure(&PIN_SDI, 1);
    SLEEP_MS(10);
    PIN_SDI.type = PIO_OUTPUT_0;
    BA->PIO_Configure(&PIN_SDI, 1);
#endif

    // Init sequence for 128x64 OLED module
    uint16_t i = 0;
    while(i < sizeof(init_conf)) {
    	const uint8_t length = init_conf[i];
    	ssd1306_command(init_conf + i + 1, length);
    	i += length + 1;
    }

    clear_window(COM_NONE, NULL);
}

void destructor(void) {

}

void write_char(uint8_t x, uint8_t y, char c) {
	// If the next char is in same row at next place we can reuse the window
	if((BC->last_write_char_x != x+1) || (BC->last_write_char_y != y)) {
		const uint8_t col[3] = {SSD1306_COLUMNADDR, PIXEL_COL_START + y*5, PIXEL_COL_START + PIXEL_W-1};
		const uint8_t row[3] = {SSD1306_PAGEADDR, x, x};
		ssd1306_command(col, 3);
		ssd1306_command(row, 3);
	}

	if(y == MAX_CHARS_W-1) {
		ssd1306_data(font + c*5, 3); // We can't draw the last char completely
	} else {
		ssd1306_data(font + c*5, 5);
	}
}

void tick(const uint8_t tick_type) {
	if(tick_type & TICK_TASK_TYPE_MESSAGE) {

	} else if(tick_type & TICK_TASK_TYPE_CALCULATION) {

	}
}

void apply_window(void) {
	const uint8_t col[3] = {SSD1306_COLUMNADDR, PIXEL_COL_START+BC->column_from, PIXEL_COL_START+BC->column_to};
	const uint8_t row[3] = {SSD1306_PAGEADDR, BC->row_from, BC->row_to};
	ssd1306_command(col, 3);
	ssd1306_command(row, 3);
}

void write(const ComType com, const Write *data) {
	BC->current_num++;

	const uint16_t bytes_up_to = BC->current_num * 64;
	const uint16_t bytes_in_window = (BC->column_to-BC->column_from+1)*(BC->row_to-BC->row_from+1);
	const int16_t diff = bytes_up_to - bytes_in_window;
	const uint8_t to_write = BETWEEN(0, 64 - diff, 64);;
	if(diff >= 0) {
		BC->current_num = 0;
	}

	ssd1306_data((const uint8_t*)data->data, to_write);

	BA->com_return_setter(com, data);
}

void new_window(const ComType com, const NewWindow *data) {
	BC->column_from = data->column_from;
	BC->column_to   = data->column_to;
	BC->row_from    = data->row_from;
	BC->row_to      = data->row_to;

	if(BC->column_to >= PIXEL_W) {
		BC->column_to = PIXEL_W-1;
	}
	if(BC->row_to >= PIXEL_H/8) {
		BC->row_to = PIXEL_H/8-1;
	}

	if(BC->column_from > BC->column_to) {
		BC->column_from = BC->column_to;
	}
	if(BC->row_from > BC->row_to) {
		BC->row_from = BC->row_to;
	}

	apply_window();

	BA->com_return_setter(com, data);
}

void clear_window(const ComType com, const ClearWindow *data) {
	const uint16_t bytes_in_window = (BC->column_to-BC->column_from+1)*(BC->row_to-BC->row_from+1);

    for(uint8_t page = 0; page < bytes_in_window/PIXEL_W; page++) {
    	ssd1306_data(NULL, PIXEL_W);
    }

    const uint8_t rest = bytes_in_window - PIXEL_W*(bytes_in_window/PIXEL_W);
    if(rest > 0) {
    	ssd1306_data(NULL, rest);
    }

	if(data != NULL) {
		BA->com_return_setter(com, data);
	}
}

void set_display_configuration(const ComType com, const SetDisplayConfiguration *data) {
	if(BC->contrast != data->contrast) {
		BC->contrast = data->contrast;
		const uint8_t contrast[2] = {SSD1306_SETCONTRAST, BC->contrast};
		ssd1306_command(contrast, 2);
	}

	if(BC->invert != data->invert) {
		BC->invert = data->invert;
		if(BC->invert) {
			const uint8_t value = SSD1306_INVERTDISPLAY;
			ssd1306_command(&value, 1);
		} else {
			const uint8_t value = SSD1306_NORMALDISPLAY;
			ssd1306_command(&value, 1);
		}
	}

	BA->com_return_setter(com, data);
}

void get_display_configuration(const ComType com, const GetDisplayConfiguration *data) {
	GetDisplayConfigurationReturn gdcr;

	gdcr.header        = data->header;
	gdcr.header.length = sizeof(GetDisplayConfigurationReturn);
	gdcr.contrast      = BC->contrast;
	gdcr.invert        = !!BC->invert;

	BA->send_blocking_with_timeout(&gdcr, sizeof(GetDisplayConfigurationReturn), com);
}

void write_line(const ComType com, const WriteLine *data) {
	if(data->line > MAX_CHARS_H-1 || data->position > MAX_CHARS_W-1) {
		BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	for(uint8_t i = 0; i < MAX_CHARS_W; i++) {
		const uint8_t pos = i + data->position;
		if(data->text[i] == '\0' || pos > MAX_CHARS_W-1) {
			break;
		}

		write_char(data->line, pos, data->text[i]);
	}

	// Set last write cache to invalid values after line write is finished
	BC->last_write_char_x = 254;
	BC->last_write_char_y = 254;

	// Reset to old window again
	apply_window();

	BA->com_return_setter(com, data);
}

void ssd1306_data(const uint8_t *data, const uint8_t length) {
	if(length == 0) {
		return;
	}

	PIN_DC.pio->PIO_SODR = PIN_DC.mask; // DC high = data
	PIN_CS.pio->PIO_CODR = PIN_CS.mask;

	// Allow NULL for easy clear display implementation
	if(data == NULL) {
		for(uint8_t i = 0; i < length; i++) {
			spibb_send_byte(0);
		}
	} else {
		for(uint8_t i = 0; i < length; i++) {
			spibb_send_byte(data[i]);
		}
	}

	PIN_SDI.pio->PIO_CODR = PIN_SDI.mask;
	PIN_CLK.pio->PIO_CODR = PIN_CLK.mask;
	SLEEP_US(10);
	PIN_CS.pio->PIO_SODR = PIN_CS.mask;

#ifdef USE_I2C
	uint8_t co = CONTROL_CO; // Single data byte
	if(length > 1 ) {
		co = 0;              // Data stream
	}
	i2c_start();
	i2c_send_byte((I2C_ADDRESS_SSD1306 << 1) | I2C_WRITE);
	i2c_send_byte(CONTROL_DC | co);

	// Allow NULL for easy clear display implementation
	if(data == NULL) {
		for(uint8_t i = 0; i < length; i++) {
			i2c_send_byte(0);
		}
	} else {
		for(uint8_t i = 0; i < length; i++) {
			i2c_send_byte(data[i]);
		}
	}
	i2c_stop();
#endif
}

void ssd1306_command(const uint8_t *command, const uint8_t length) {
	if(length == 0) {
		return;
	}

	PIN_DC.pio->PIO_CODR = PIN_DC.mask; // DC low = command
	PIN_CS.pio->PIO_CODR = PIN_CS.mask;

	for(uint8_t i = 0; i < length; i++) {
		spibb_send_byte(command[i]);
	}

	PIN_SDI.pio->PIO_CODR = PIN_SDI.mask;
	PIN_CLK.pio->PIO_CODR = PIN_CLK.mask;
	SLEEP_US(10);
	PIN_CS.pio->PIO_SODR = PIN_CS.mask;;

#ifdef USE_I2C
	i2c_start();
	i2c_send_byte((I2C_ADDRESS_SSD1306 << 1) | I2C_WRITE);
	if(length > 1) {
		i2c_send_byte(0);          // Command stream
	} else {
		i2c_send_byte(CONTROL_CO); // Single command byte
	}
	for(uint8_t i = 0; i < length; i++) {
		i2c_send_byte(command[i]);
	}
	i2c_stop();
#endif
}

#ifdef USE_SPI
void spibb_send_byte(const uint8_t value) {
	for(int8_t i = 7; i >= 0; i--) {
		PIN_CLK.pio->PIO_CODR = PIN_CLK.mask;
		if((value >> i) & 1) {
			PIN_SDI.pio->PIO_SODR = PIN_SDI.mask;
		} else {
			PIN_SDI.pio->PIO_CODR = PIN_SDI.mask;
		}

		SLEEP_US(1);
		PIN_CLK.pio->PIO_SODR = PIN_CLK.mask;
		SLEEP_US(1);
	}

	// Make sure clock and data is low when no data transfer is taking place.
	PIN_SDI.pio->PIO_CODR = PIN_SDI.mask;
	PIN_CLK.pio->PIO_CODR = PIN_CLK.mask;
}
#endif

#ifdef USE_I2C
bool i2c_scl_value(void) {
	return PIN_SCL.pio->PIO_PDSR & PIN_SCL.mask;
}

void i2c_scl_high(void) {
	PIN_SCL.pio->PIO_ODR = PIN_SCL.mask;
	while(!i2c_scl_value()); // allow slave to clock-stretch
}

void i2c_scl_low(void) {
	PIN_SCL.pio->PIO_OER = PIN_SCL.mask;
}

bool i2c_sda_value(void) {
	return PIN_SDA.pio->PIO_PDSR & PIN_SDA.mask;
}

void i2c_sda_high(void) {
	PIN_SDA.pio->PIO_ODR = PIN_SDA.mask;
}

void i2c_sda_low(void) {
	PIN_SDA.pio->PIO_OER = PIN_SDA.mask;
}

void i2c_sleep_halfclock(void) {
	SLEEP_NS(I2C_HALF_CLOCK_400KHZ);
}

void i2c_stop(void) {
	i2c_scl_low();
	i2c_sda_low();
	i2c_sleep_halfclock();
	i2c_scl_high();
	i2c_sleep_halfclock();
	i2c_sda_high();
	i2c_sleep_halfclock();
}

void i2c_start(void) {
	i2c_scl_high();
	i2c_sleep_halfclock();
	i2c_sda_low();
	i2c_sleep_halfclock();
}

uint8_t i2c_recv_byte(bool ack) {
	uint8_t value = 0;

	for(int8_t i = 7; i >= 0; i--) {
		i2c_scl_low();
		i2c_sda_high(); // allow slave to read
		i2c_sleep_halfclock();
		i2c_scl_high();
		if(i2c_sda_value()) {
			value |= (1 << i);
		}
		i2c_sleep_halfclock();
	}

	// ACK
	i2c_scl_low();
	if(ack) {
		i2c_sda_low();
	} else {
		i2c_sda_high();
	}
	i2c_sleep_halfclock();
	i2c_scl_high();
	i2c_sleep_halfclock();

	return value;
}

bool i2c_send_byte(const uint8_t value) {
	for(int8_t i = 7; i >= 0; i--) {
		i2c_scl_low();
		if((value >> i) & 1) {
			i2c_sda_high();
		} else {
			i2c_sda_low();
		}
		i2c_sleep_halfclock();
		i2c_scl_high();
		i2c_sleep_halfclock();
	}

	i2c_sda_high(); // Make sure SDA is always released

	// Wait for ACK
	bool ret = false;

	i2c_scl_low();
	i2c_sleep_halfclock();
	i2c_scl_high();
	if(!i2c_sda_value()) {
		ret = true;
	}

	i2c_sleep_halfclock();

	return ret;
}
#endif
