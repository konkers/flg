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
 *
 * SMBus 8 bit crc.  Code adapted from Atmel app note AVR316
 *
 */

#ifndef __crc8_h__
#define __crc8_h__

#ifdef AVR
typedef prog_uint8_t crc8_table_t;
#else
typedef const uint8_t crc8_table_t;
#endif

extern crc8_table_t crc8_table[];

#ifdef AVR
static inline uint8_t crc8_table_get(uint8_t idx)
{
	return pgm_read_byte(crc8_table + idx);
}
#else
static inline uint8_t crc8_table_get(uint8_t idx)
{
	return crc8_table[idx];
}
#endif


static inline uint8_t crc8_start()
{
	return 0x00;
}

static inline uint8_t crc8_calc(uint8_t crc, uint8_t data)
{
	return crc8_table_get(crc ^ data);
}

static inline uint8_t crc8_end(crc)
{
	return crc;
}

#endif /* __crc8_h__ */
