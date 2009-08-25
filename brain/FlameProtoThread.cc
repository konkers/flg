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

#include "FlameProtoThread.hh"
#include "Soma.hh"

FlameProtoThread::FlameProtoThread(Soma *s, Link *l) :
	Thread()
{
	int i;

	soma = s;
	link = l;

	for (i = 0; i < Soma::nRelays; i++)
		relayBoardState[soma->getRelayAddr(i)] = 0x0;

	for (i = 0; i < Soma::nButtons; i++)
		buttonState[soma->getButtonAddr(i)] = 0x0;

	proto = new Proto(link, this, NULL, 0, 0);
}

FlameProtoThread::~FlameProtoThread()
{
	delete proto;
}


int FlameProtoThread::run(void)
{
	int i;
	map<uint8_t, uint8_t>::iterator it;

	while (1) {
		soma->flameSync();

		for (it = relayBoardState.begin();
		     it != relayBoardState.end();
		     it++)
			relayBoardState[it->first]= 0x0;

		for (i = 0; i < Soma::nRelays; i++) {
			if (soma->getRelay(i)) {
				relayBoardState[soma->getRelayAddr(i)] |=
					1 << soma->getRelayIdx(i);
			} else {
				relayBoardState[soma->getRelayAddr(i)] &=
					~(1 << soma->getRelayIdx(i));
			}
		}

		for (it = relayBoardState.begin();
		     it != relayBoardState.end();
		     it++) {
			setRelay(it->first, it->second);
		}

		for (i = 0; i < Soma::nDpots; i++)
			setDpot(soma->getDpotAddr(i), soma->getDpotIdx(i),
				soma->getDpot(i));

		for (it = buttonState.begin();
		     it != buttonState.end();
		     it++)
			getButtons(it->first);

		for (i = 0; i < Soma::nButtons; i++)
			soma->setButton(i, buttonState[soma->getButtonAddr(i)] &
				       (1 << soma->getButtonIdx(i)));

		for (i = 0; i < Soma::nKnobs; i++)
			soma->setKnob(i, getKnob(soma->getKnobAddr(i),
						soma->getKnobIdx(i)));

	}
}

bool FlameProtoThread::setRelay(uint8_t addr, uint8_t val)
{
	int retries = nRetries;

	do {
		if (!proto->setRelay(addr, val))
			continue;

		if (!proto->clearRelay(addr, ~val))
			continue;

		return true;
	} while(--retries);

	return false;
}

bool FlameProtoThread::setDpot(uint8_t addr, uint8_t dpot, uint8_t val)
{
	int retries = nRetries;

	do {
		if (proto->setDpot(addr, dpot, val))
			return true;
	} while(--retries);

	return false;
}

bool FlameProtoThread::getButtons(uint8_t addr)
{
	int retries = nRetries;

	do {
		dataValid = false;
		proto->getSwitch(addr);
		proto->waitForMsg(timeoutUs);
		if (dataValid) {
			buttonState[addr] = data;
			return true;
		}
	} while(--retries);

	fprintf(stderr, "button timeout %02x\n", addr);
	return false;
}

uint16_t FlameProtoThread::getKnob(uint8_t addr, uint8_t idx)
{
	int retries = nRetries;
	uint16_t val;

	do {
		dataValid = false;
		proto->getAdcLo(addr, idx);
		proto->waitForMsg(timeoutUs);
		if (!dataValid)
			continue;
		val = data;

		dataValid = false;
		proto->getAdcHi(addr, idx);
		proto->waitForMsg(timeoutUs);
		if (!dataValid)
			continue;
		val |= data << 8;

		return val;
	} while(--retries);
	fprintf(stderr, "knob timeout\n");
	return 0x0;
}



void FlameProtoThread::relay(uint8_t idx, uint8_t state)
{
}

void FlameProtoThread::light(uint8_t idx, uint8_t val)
{
}

void FlameProtoThread::dpot(uint8_t idx, uint8_t val)
{
}

void FlameProtoThread::resp(struct proto_packet *pkt)
{
	data = pkt->val;
	dataValid = true;
}

