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
#include <ProtoHandler.hh>

class Proto {
private:
	struct proto p;
	Link *link;
	ProtoHandler *protoHandler;
	struct proto_handlers handlers;

	bool sendMsg(uint8_t addr, uint8_t cmd, uint8_t val);

public:
	Proto(Link *link, ProtoHandler *protoHander,
	     struct proto_widget *widgets, int n_widgets,
	     uint8_t addr);

	void handleLight(uint8_t idx, uint8_t val);

	bool setRelay(uint8_t addr, uint8_t relay);
	bool clearRelay(uint8_t addr, uint8_t relay);
	bool setLight(uint8_t addr, int light, uint8_t cal);
	bool setAddr(uint8_t addr, uint8_t newAddr);

	int waitForMsg(int timeout);
	void ping(void);
};


#endif /* __Proto_hh__ */
