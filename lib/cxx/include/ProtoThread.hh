/*
 * Copyright 2010 Erik Gilling
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

#ifndef __ProtoThread_hh__
#define __ProtoThread_hh__

#include <Thread.hh>
#include <Link.hh>
#include <Proto.hh>

class Bus;
class Board;
class AnalogIn;

class ProtoThread : public Thread, public ProtoHandler
{
private:
	static const int nRetries = 4;
	static const int timeoutUs = 100000;

	Link *link;
	Bus *bus;
	Proto *proto;

	bool done;
	Condition cond;

	int page;

	uint8_t minLedAddr;
	uint8_t maxLedAddr;
	uint32_t *ledData;

	uint8_t data;
	bool dataValid;

	void sendBoard(Board *b);
	void sendLeds(void);
	void pollBoard(Board *b);
	bool getDigitalIns(Board *b);
	bool getAnalogIn(AnalogIn *in);

protected:
	virtual int run(void);

public:
	ProtoThread(Bus *b, Link *l);
	virtual ~ProtoThread();

	void setPage(int p) {
		page = p;
	}

	void clearDone(void) {
		cond.lock();
		done = false;
		cond.signal();
		cond.unlock();
	}

	void waitDone(void) {
		cond.lock();
		while (!done) {
			cond.wait();
		}
		cond.unlock();
	}

	virtual void relay(uint8_t idx, uint8_t state);
	virtual void light(uint8_t idx, uint8_t val);
	virtual void dpot(uint8_t idx, uint8_t val);
	virtual void resp(struct proto_packet *pkt);

};
#endif /* __ProtoThread_hh__ */
