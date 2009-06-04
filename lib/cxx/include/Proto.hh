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

#ifndef __Proto_hh__
#define __Proto_hh__

#include <stdint.h>

#include <proto.h>

#include <Link.hh>

class Proto {
private:
	struct proto p;
	Link *link;

public:
	Proto(Link *link, uint8_t addr);

	bool sendMsg(uint8_t addr, uint8_t cmd, uint8_t val);
	int waitForMsg(struct proto_packet *p, int timeout);
};


#endif /* __Proto_hh__ */
