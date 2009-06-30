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

#ifndef __libavr_config_h__
#define __libavr_config_h__

/* increment with every change to struct config_data */
#define CONFIG_VER		0
#define CONFIG_DEFAULT_ADDR	0

struct config_data {
	uint8_t ver;
	uint8_t addr;
	uint8_t crc;
};

extern struct config_data config;

void config_init(void);
void config_load_defaults(void);
void config_write(void);

#endif /* __libavr_config_h__ */
