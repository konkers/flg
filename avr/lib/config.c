/*
 * Copyright 2009 Erik Gilling
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/eeprom.h>

#include <config.h>

#include <crc8.h>

struct config_data config;

void config_init(void)
{
	uint8_t crc;
	uint8_t i;

	eeprom_read_block(&config, (const void*)0x10, sizeof(config));

	crc = crc8_start();
	for (i = 0 ; i < (sizeof(config) - 1); i++) {
		crc = crc8_calc(crc, ((uint8_t *)&config)[i]);
	}
	crc = crc8_end(crc);

	if (config.crc != crc || config.ver != CONFIG_VER) {
		config_load_defaults();
		config_write();
	}
}

void config_load_defaults(void)
{
	config.ver = CONFIG_VER;
	config.addr = CONFIG_DEFAULT_ADDR;
}

void config_write(void)
{
	uint8_t crc;
	uint8_t i;

	crc = crc8_start();
	for (i = 0 ; i < (sizeof(config) - 1); i++) {
		crc = crc8_calc(crc, ((uint8_t *)&config)[i]);
	}
	crc = crc8_end(crc);

	config.crc = crc;
	eeprom_write_block(&config, (void*)0x10, sizeof(config));
}
