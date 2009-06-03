/*
 * Copyright 2009 Flaming Lotus Girls
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

#ifndef __Link_hh__
#define __Link_hh__

#include <stdint.h>

class Link {
public:
	virtual ~Link() {}

	virtual bool send(uint8_t *data, int len) = 0;
	virtual int wait(int timeout) = 0;
	virtual bool recv(uint8_t *data, int len) = 0;
};

#endif /* __Link_hh__ */

