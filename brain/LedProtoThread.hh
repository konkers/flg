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

#ifndef __LedProtoThread_hh__
#define __LedProtoThread_hh__

#include <map>

#include <Thread.hh>

class Soma;

class LedProtoThread : public Thread, public ProtoHandler
{
private:
	uint32_t *ledState;
	uint8_t minAddr;
	uint8_t maxAddr;

	Soma *soma;
	Link *link;

	Proto *proto;

protected:
	virtual int run(void);

public:
	LedProtoThread(Soma *s, Link *l);
	virtual ~LedProtoThread();

	virtual void relay(uint8_t idx, uint8_t state);
	virtual void light(uint8_t idx, uint8_t val);
	virtual void dpot(uint8_t idx, uint8_t val);

	virtual void resp(struct proto_packet *pkt);

};

#endif /* __LedProtoThread_hh__ */
