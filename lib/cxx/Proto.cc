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

#include <stdio.h>
#include <string.h>

#include <Proto.hh>

extern "C" void handle_relay(void *data, uint8_t idx, uint8_t state)
{
	ProtoHandler *p = (ProtoHandler *)data;

	if (p)
		p->relay(idx, state);

}

extern "C" void handle_light(void *data, uint8_t idx, uint8_t val)
{
	ProtoHandler *p = (ProtoHandler *)data;

	if (p)
		p->light(idx, val);
}

Proto::Proto(Link *link, ProtoHandler *ph,
	     struct proto_widget *widgets, int n_widgets,
	     uint8_t addr)
{
	this->link = link;

	handlers.relay = handle_relay;
	handlers.light = handle_light;

	p.handlers = &handlers;
	p.handler_data = ph;
	p.widgets = widgets;
	p.n_widgets = n_widgets;

	proto_init(&p, addr);
}

bool Proto::sendMsg(uint8_t addr, uint8_t cmd, uint8_t val)
{
	proto_packet pkt;
	pkt.addr = addr;
	pkt.cmd = cmd;
	pkt.val = val;
	proto_packet_seal(&pkt);

	return link->send(&pkt, sizeof(pkt));
}

bool Proto::setRelay(uint8_t addr, uint8_t relay)
{
	return sendMsg(addr, PROTO_CMD_RELAY_SET, relay);
}

bool Proto::clearRelay(uint8_t addr, uint8_t relay)
{
	return sendMsg(addr, PROTO_CMD_RELAY_CLEAR, relay);
}

bool Proto::setLight(uint8_t addr, int light, uint8_t val)
{
	if (light < 0 || light > 0xf)
		return false;

	return sendMsg(addr, PROTO_CMD_LIGHT0_SET + light, val);
}


int Proto::waitForMsg(int timeout)
{
	int len;
	int err;
	uint8_t data[32];
	uint8_t *dp = data;

	err = link->wait(timeout);
	if (err <= 0) {
		return err;
	}

	len = 32;
	if (link->recv(data,&len)) {
		while (len--)
			proto_recv(&p, *dp++);
	} else {
		return -1;
	}

	return timeout;
}

void Proto::ping()
{
	proto_ping(&p);
}
