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

#define RELAY_TIMEOUT 10

struct relay {
	uint8_t mask;
	uint8_t timeout;
};

static inline void relay_ping(struct relay *r)
{
	if (r->timeout > 0)
		r->timeout--;
}

void relay_set(struct relay *r, uint8_t val)
{
	if (r->mask & val) {
		r->timeout = RELAY_TIMEOUT;
	}
}

void relay_clear(struct relay *r, uint8_t val)
{
	if (r->mask & val) {
		r->timeout = 0;
	}
}

static inline uint8_t relay_state(struct relay *r)
{
	if (r->timeout > 0)
		return mask;
	else
		return 0;
}

