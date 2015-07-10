/* oled-128x64-bricklet
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config.h: OLED 64x48 Bricklet specific configuration
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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib/drivers/board/sam3s/SAM3S.h"

#include "oled.h"

#define BRICKLET_FIRMWARE_VERSION_MAJOR 2
#define BRICKLET_FIRMWARE_VERSION_MINOR 0
#define BRICKLET_FIRMWARE_VERSION_REVISION 0

#define BRICKLET_HARDWARE_VERSION_MAJOR 1
#define BRICKLET_HARDWARE_VERSION_MINOR 0
#define BRICKLET_HARDWARE_VERSION_REVISION 0

#define BRICKLET_DEVICE_IDENTIFIER 263

#define BOARD_MCK 64000000

#define INVOCATION_IN_BRICKLET_CODE

#ifdef USE_I2C
#define PIN_NRESET (BS->pin1_ad)
#define PIN_SA0    (BS->pin2_da)
#define PIN_SCL    (BS->pin3_pwm)
#define PIN_SDA    (BS->pin4_io)
#endif

#ifdef USE_SPI
#define PIN_CS  (BS->pin1_ad)
#define PIN_DC  (BS->pin2_da)
#define PIN_CLK (BS->pin3_pwm)
#define PIN_SDI (BS->pin4_io)
#endif

typedef struct {
	uint8_t column_from;
	uint8_t column_to;
	uint8_t row_from;
	uint8_t row_to;

	uint8_t current_num;

	uint8_t contrast;

	uint8_t last_write_char_x;
	uint8_t last_write_char_y;
	bool invert;
} BrickContext;

#endif
