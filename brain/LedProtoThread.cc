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

#include "LedProtoThread.hh"
#include "Soma.hh"

LedProtoThread::LedProtoThread(Soma *s, Link *l) :
	Thread()
{
	int i;

	soma = s;
	link = l;

	minAddr = 0xff;
	maxAddr = 0x0;
	for (i = 0; i < Soma::nLights; i++) {
		uint8_t addr = soma->getLightAddr(i);
		minAddr = addr < minAddr ? addr : minAddr;
		maxAddr = addr > maxAddr ? addr : maxAddr;
	}

	ledState = new uint32_t[maxAddr - minAddr];
	memset(ledState, 0x0, (maxAddr - minAddr) * sizeof(uint32_t));

	proto = new Proto(link, this, NULL, 0, 0);


}

LedProtoThread::~LedProtoThread()
{
	delete proto;
	delete ledState;
}

int LedProtoThread::run(void)
{
	int i;
	map<uint8_t, uint8_t>::iterator it;

	while (1) {
		soma->ledSync();

		for (i = 0; i < Soma::nLights; i++) {
			uint8_t addr = soma->getLightAddr(i);
			ledState[addr - minAddr] = soma->getLight(i);
		}

		proto->setLights(minAddr, ledState, maxAddr - minAddr);
		proto->sync(PROTO_ADDR_BCAST);
		proto->sync(PROTO_ADDR_BCAST);
		proto->sync(PROTO_ADDR_BCAST);
	}
}

void LedProtoThread::relay(uint8_t idx, uint8_t state)
{
}

void LedProtoThread::light(uint8_t idx, uint8_t val)
{
}

void LedProtoThread::dpot(uint8_t idx, uint8_t val)
{
}

void LedProtoThread::resp(struct proto_packet *pkt)
{
}

